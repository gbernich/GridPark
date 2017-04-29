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
  int i, count, x, y, x0, y0, x3, y3;
  int regionId;
  int imgNum = 0;
  char imgFn[80] = {0};
  int spot_id = 0;

  // Matrices
  Mat src, src_gray, overlay;

  // Corner Detection
  int thresh = IP_CORNERS_THRESH_INIT;
  int max_thresh = 255;
  Mat corners;

  // Edge Detection
  Mat edges;

  //Sliding Window
  Window startWin, endWin;
  vector<int> sums, sumsLeft, sumsRight, windowSizes;
  vector< vector<int> > sumsVector, sumsVectorLeft, sumsVectorRight;
  vector<Opening> openings, spaces;
  vector<float> sumsNorm;
  vector<OPEN_SPOT_T> spaces_db, spaces_db_all;

  //Suspicious Activity 
  vector<ParkedCar> cars;
  int edgeList[10] = {0};
  bool carParked = true;
  bool monitorON = true;
  bool resetCount = false;
  bool justParked = false;
  bool pedDetected = false;
  bool alert = false;
  bool haveBaseImg = false;
  int  pedCount = 0;
  bool haveBC = false;
  Window carWindow;
  carWindow.tl.x = 830;
  carWindow.tl.y = 870;
  carWindow.br.x = 1265;
  carWindow.br.y = 1040;

  int baseCount;
  int bcSum;
  int bcAvg;
  int actCount = 0;
  int purgeCount = 0;
  int width;
  int height;
  Corner topLeft;
  topLeft.x = 1070;
  topLeft.y = 650;
  width = 165;
  height = 50;
  vector<SuspAct> alertList;
  Mat baseImg, subImg, cln;
  int firstSum;

  Rect roi(carWindow.tl.x, carWindow.tl.y, carWindow.br.x - carWindow.tl.x, carWindow.br.y - carWindow.tl.y);
  SuspAct act = {0};

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
    sprintf(imgFn, "%s", "img.jpg"); // argv[1]
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
      sums     = GetSlidingSum(edges, 0, startWin, endWin, regionId);
      sumsNorm = GetNormalizedSlidingSum(edges, 0, startWin, endWin, regionId);
      openings = GetOpeningsFromSumsNormalized(sumsNorm, regionId);
//      WriteSlidingWindowFloat((char *)("../matlab/edges.txt"), " ", sumsNorm);
//break;
//      cout << "region " << regionId << endl;
      //for (i = 0; i < openings.size(); i++)
      //  cout << "opening at " << openings.at(i).start << " " << openings.at(i).length << endl;


      spaces = GetOpenParkingSpaces(openings, regionId);
//      for (i = 0; i < spaces.size(); i++)
//        cout << "space at " << spaces.at(i).start << " " << spaces.at(i).length << endl;

      if (spaces.size() > 0)
      {
        // Convert spaces to usable format
        spaces_db = FormatSpacesForDB(spaces, regionId, &spot_id);
        spaces_db_all.insert(spaces_db_all.end(), spaces_db.begin(), spaces_db.end());
        //cout << "all " << spaces_db_all.size() << endl;
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

//    if(!justParked)
    if (cars.size() > 0)
    {
      for(i = 0; i < cars.size(); i++)
      {
        if(cars[i].susp_activity == 1)
        {
          justParked = true;
          topLeft = cars[i].tl;
          roi.x = cars[i].tl.x;
          roi.y = cars[i].tl.y;
          roi.width = cars[i].br.x - cars[i].tl.x;
          roi.height = cars[i].br.y - cars[i].tl.y;
          width = 165;
          height = 50;
          act.car_id = cars[i].id;
          act.time_of_detect = 0;
          act.length_of_activity = 0;
        }
        break;
      }

//    if (cars.size() == 0)
//      justParked = false;

    }else {
      justParked = false;
      haveBC = false;
      haveBaseImg = false;
    }

    //else if(justParked and !haveBC)
    if(justParked and !haveBC and haveBaseImg)
    {
      // take second "base" to compare
      //cv::subtract(edges(roi), baseImg, subImg);
      //subImg = abs(subImg);
      PseudoSubtract(edges(roi), baseImg, subImg);
      baseCount = (int)cv::sum(subImg)[0];
      cout << "base " << baseCount << endl;
      haveBC = true;
      // if(loopCount == 0)
      // {
      //   carWindow = CreateWindow(topLeft, width, height, 0);
      // }
      // baseCount = GetBaseCount(edges, carWindow);
      // cout << "curr frame " << baseCount << endl;
      // bcAvg = UpdateEdgeList(edgeList, baseCount);
      // cout << "BC AVG" << bcAvg << endl;

      // if(loopCount == 10)
      // {
      //   justParked = false;
      //   haveBC = true;
      //   baseCount = bcAvg;
      // }
    }

    // Get Base Image
    if (justParked and !haveBaseImg)
    {
      cln = edges.clone();
      baseImg = cln(roi);
      haveBaseImg = true;
    }

    if(haveBC)
    {
//      cout << "haveBC" << endl;

      //cv::subtract(edges(roi), baseImg, subImg);
      //subImg = abs(subImg);
      PseudoSubtract(edges(roi), baseImg, subImg);
      cout << "sub sum " << sum(subImg)[0] << endl;
      imwrite("b_base.jpg", baseImg);
      imwrite("b_sub.jpg", subImg);
      imwrite("b_new.jpg", edges(roi));

//      alert = RunSusActivity(carParked, monitorON, resetCount, &actCount, baseCount, subImg, carWindow, edgeList);
      pedDetected = DetectActivity(subImg, carWindow, baseCount, NULL);
      cout << "Detected Ped " << pedDetected << endl;

      // if Pedestrian is detected, increment count
      // only write to DB once, until it purges
      if (pedDetected)
      {
        if (++pedCount == K_PED_CONSECUTIVE_DETECTS)
        {
          #ifdef __arm__
            cout << "                         writing" << endl;
            alertList.push_back(act);
            InsertSuspActivity(alertList, conn);
          #endif
        }
        purgeCount  = 0;
        pedDetected = false;
      }
      else // no detect, purge DB 
      {
        pedCount = 0;
        #ifdef __arm__
          if (++purgeCount >= K_PURGE_THRESHOLD)
          {
            PurgeAllSuspActivity(conn);
            alertList.clear();
            purgeCount = 0;
            cout << "                       purging" << endl;
          }
        #endif
      }

      // if(alert)
      // {
      //   alertList.push_back(act);
      //   #ifdef __arm__
      //     InsertSuspActivity(alertList, conn);
      //   #endif
      //   alert = false;
      //   loopCount = 1; //start counting
      // }
    }

    // loop count is set to 1 when there is an alert, so count up
    //if (loopCount > 0)
    //    loopCount++;

    // stop at a value to purge the database, so that the system
    // can send a new alert
    //#ifdef __arm__
    //  if (loopCount == K_PURGE_THRESHOLD)
    //  {
    //    PurgeAllSuspActivity(conn);
    //  }
    //#endif

//    alert = RunSusActivity(carParked, monitorON, resetCount, &actCount, baseCount, edges, carWindow);
    clock_gettime(CLOCK_MONOTONIC, &finish_suspact);
//    cout << "base " << baseCount << endl;
//    cout << alert << endl;

    // Capture time
    clock_gettime(CLOCK_MONOTONIC, &finish);
/*    elapsed = (finish_cam.tv_sec - start_cam.tv_sec); elapsed += (finish_cam.tv_nsec - start_cam.tv_nsec) / 1000000000.0;
    cout << "Camera:      " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_edges.tv_sec - start_edges.tv_sec); elapsed += (finish_edges.tv_nsec - start_edges.tv_nsec) / 1000000000.0;
    cout << "Edge Detect: " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_parking.tv_sec - start_parking.tv_sec); elapsed += (finish_parking.tv_nsec - start_parking.tv_nsec) / 1000000000.0;
    cout << "Parking:     " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_suspact.tv_sec - start_suspact.tv_sec); elapsed += (finish_suspact.tv_nsec - start_suspact.tv_nsec) / 1000000000.0;
    cout << "Suspicious:  " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_db.tv_sec - start_db.tv_sec); elapsed += (finish_db.tv_nsec - start_db.tv_nsec) / 1000000000.0;
    cout << "Database:    " << elapsed * 1000.0 << " ms" << endl; */
    elapsed = (finish.tv_sec - start.tv_sec); elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Total:       " << elapsed * 1000.0 << " ms" << endl;

    // Save edges image for DEBUG USE ONLY (REMOVE THIS)
//    imwrite("./testimg/edges.jpg", edges);

    // Go to sleep
//    break; // for development lets only run the loop once
//    sleep(5);
  }

  #ifdef __arm__
    CloseDB(conn);
  #endif

  return 0;

}
