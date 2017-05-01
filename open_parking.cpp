//-----------------------------------------------------------------------------
// File:        open_parking.c
// Author:      Garrett Bernichon
// Function:    This is the main.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <unistd.h>
#include "ip.h"
#include "common.h"

#ifdef __arm__
#include <mysql/mysql.h>
extern "C" {
    #include "db_utils.h"
}
#endif

#define USE_DB 1

using namespace std;

int main(int argc, char** argv )
{
  struct timespec start,         finish,
                  start_cam,     finish_cam,
                  start_edges,   finish_edges,
                  start_db,      finish_db,
                  start_parking, finish_parking,
                  start_suspact, finish_suspact;
  double elapsed;
  int i, regionId;
  int imgNum = 0;
  char imgFn[80] = {0};
  int spot_id = 0;

  // Matrices
  Mat src, src_gray;
  Mat baseImg, subImg, cln;

  // Corner Detection
  int thresh = IP_CORNERS_THRESH_INIT;
  int max_thresh = 255;
  Mat corners;

  // Edge Detection
  Mat edges;

  //Sliding Window
  Window startWin, endWin;
  vector<Opening> openings, spaces;
  vector<float> sumsNorm;
  vector<OPEN_SPOT_T> spaces_db, spaces_db_all;

  //Suspicious Activity 
  vector<ParkedCar> cars;
  bool justParked = false;
  bool pedDetected = false;
  bool haveBaseImg = false;
  int  pedCount = 0;
  bool haveBC = false;
  Window carWindow;
  int baseCount;
  int purgeCount = 0;
  vector<SuspAct> alertList;
  int firstSum;
  SuspAct act = {0};

  carWindow.tl.x = 830;
  carWindow.tl.y = 870;
  carWindow.br.x = 1265;
  carWindow.br.y = 1040;
  Rect roi(carWindow.tl.x, carWindow.tl.y, carWindow.br.x - carWindow.tl.x, carWindow.br.y - carWindow.tl.y);
  

  #ifdef __arm__
    MYSQL * conn = OpenDB((char*)K_DB);
  #endif

  // Main loop that will continue forever
  while (true)
  {
    // Let's time this
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Take an image
    clock_gettime(CLOCK_MONOTONIC, &start_cam);
    TakeNewImage();
    clock_gettime(CLOCK_MONOTONIC, &finish_cam);

    // Load source image
    clock_gettime(CLOCK_MONOTONIC, &start_edges);
    sprintf(imgFn, "%s", "img.jpg");
    src = imread(imgFn, 1);

    // Get edges
    thresh = 50;
    cvtColor(src, src_gray, CV_BGR2GRAY);
    edges = GetEdges(src_gray, thresh, 3, 3);
    clock_gettime(CLOCK_MONOTONIC, &finish_edges);

    // Loop through the subregions
    clock_gettime(CLOCK_MONOTONIC, &start_parking);
    for (regionId = 0; regionId < K_NUM_SUBREGIONS; regionId++)
    {
      startWin = GetStartWindow(regionId);
      endWin   = GetEndWindow(regionId);
      sumsNorm = GetNormalizedSlidingSum(edges, 0, startWin, endWin, regionId);
      openings = GetOpeningsFromSumsNormalized(sumsNorm, regionId);
      //WriteSlidingWindowFloat((char *)("../matlab/edges.txt"), " ", sumsNorm);
      spaces = GetOpenParkingSpaces(openings, regionId);

      if (spaces.size() > 0)
      {
        // Convert spaces to usable format
        spaces_db = FormatSpacesForDB(spaces, regionId, &spot_id);
        spaces_db_all.insert(spaces_db_all.end(), spaces_db.begin(), spaces_db.end());
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &finish_parking);

    // Write to database (blocking)
    #ifdef __arm__  // only on raspberry pi
      clock_gettime(CLOCK_MONOTONIC, &start_db);
      if (USE_DB)
        InsertOpenParking(spaces_db_all, conn);
      spaces_db_all.clear();
      clock_gettime(CLOCK_MONOTONIC, &finish_db);
   #endif


    // Suspicious Activity
    #ifdef __arm__  // only on raspberry pi
      cars = GetParkedCars(conn);
    #endif
    clock_gettime(CLOCK_MONOTONIC, &start_suspact);

    // Check if someone (using the system) is parked
    if (cars.size() > 0)
    {
      for(i = 0; i < cars.size(); i++)
      {
        if(cars[i].susp_activity == 1)
        {
          justParked = true;
          //topLeft = cars[i].tl;
          roi.x = cars[i].tl.x;
          roi.y = cars[i].tl.y;
          roi.width = cars[i].br.x - cars[i].tl.x;
          roi.height = cars[i].br.y - cars[i].tl.y;
          act.car_id = cars[i].id;
          act.time_of_detect = 0;
          act.length_of_activity = 0;
        }
        break;
      }
    }
    // otherwise reset flags
    else
    {
      justParked = false;
      haveBC = false;
      haveBaseImg = false;
    }

    // This is for the second image to get the threshold for susp activity
    if(justParked and !haveBC and haveBaseImg)
    {
      // take second "base" to compare
      subImg = PseudoSubtract(baseImg, edges(roi));
      baseCount = (int)cv::sum(subImg)[0];
      cout << "base " << baseCount << endl;
      haveBC = true;
    }

    // Get Base Image
    if (justParked and !haveBaseImg)
    {
      cln = edges.clone();
      baseImg = cln(roi);
      haveBaseImg = true;
    }

    // This is the case where we have the base image, and the threshold set
    if(haveBC)
    {
      subImg = PseudoSubtract(baseImg, edges(roi));
      cout << "sub sum " << sum(subImg)[0] << endl;
      imwrite("b_base.jpg", baseImg);
      imwrite("b_sub.jpg", subImg);
      imwrite("b_new.jpg", edges(roi));

      pedDetected = DetectActivity(subImg, baseCount);
      cout << "Detected Ped " << pedDetected << endl;

      // if Pedestrian is detected, increment count
      // only write to DB once, until it purges
      if (pedDetected)
      {
        if (++pedCount == K_PED_CONSECUTIVE_DETECTS)
        {
          #ifdef __arm__
            cout << "              writing" << endl;
            alertList.push_back(act);
            InsertSuspActivity(alertList, conn);
          #endif
        }
        purgeCount  = 0;
        pedDetected = false;
      }
      else // no detect, purge database 
      {
        pedCount = 0;
        #ifdef __arm__
          if (++purgeCount >= K_PURGE_THRESHOLD)
          {
            PurgeAllSuspActivity(conn);
            alertList.clear();
            purgeCount = 0;
            cout << "              purging" << endl;
          }
        #endif
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &finish_suspact);

    // Capture time
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec); elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Total:       " << elapsed * 1000.0 << " ms" << endl;

    // Save edges image for DEBUG USE ONLY (REMOVE THIS)
    // imwrite("./testimg/edges.jpg", edges);
  }

  #ifdef __arm__
    CloseDB(conn);
  #endif

  return 0;
}

// For timing individual functions
/*    ela
psed = (finish_cam.tv_sec - start_cam.tv_sec); elapsed += (finish_cam.tv_nsec - start_cam.tv_nsec) / 1000000000.0;
    cout << "Camera:      " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_edges.tv_sec - start_edges.tv_sec); elapsed += (finish_edges.tv_nsec - start_edges.tv_nsec) / 1000000000.0;
    cout << "Edge Detect: " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_parking.tv_sec - start_parking.tv_sec); elapsed += (finish_parking.tv_nsec - start_parking.tv_nsec) / 1000000000.0;
    cout << "Parking:     " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_suspact.tv_sec - start_suspact.tv_sec); elapsed += (finish_suspact.tv_nsec - start_suspact.tv_nsec) / 1000000000.0;
    cout << "Suspicious:  " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_db.tv_sec - start_db.tv_sec); elapsed += (finish_db.tv_nsec - start_db.tv_nsec) / 1000000000.0;
    cout << "Database:    " << elapsed * 1000.0 << " ms" << endl;
    */
