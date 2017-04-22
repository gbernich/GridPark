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

//static xy x_vals_init[] = {{1, 0}, {2, 10}, {4, 25}, {6, 40}};
//static vector<xy> x_vals(x_vals_init, x_vals_init + sizeof(x_vals_init) / sizeof(xy));

int main(int argc, char** argv )
{
  struct timespec start,         finish,
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
  Mat src, src_gray, roi, roi_gray, overlay;

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
  bool justParked = true;
  bool alert = false;
  bool haveBC = false;
  Window carWindow;
  int baseCount;
  int bcSum;
  int bcAvg;
  int actCount = 0;
  int loopCount = 0;
  int width;
  int height;
  Corner topLeft;
  topLeft.x = 1070;
  topLeft.y = 650;
  width = 165;
  height = 50;

  // testing
  //cout << "interpolate " << Interpolate(3, x_vals) << endl;
  //cout << "interpolate " << Interpolate(5, x_vals) << endl;

  #ifdef __arm__
    MYSQL * conn = OpenDB((char*)K_DB);
    cars = GetParkedCars(conn);

    for(i = 0; i < cars.size(); i++)
      cout << cars.at(i).br.y << endl;

  #endif

  // Main loop that will continue forever
  while (true)
  {
    // Let's time this
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Take an image
//    TakeNewImage();

    // Load source image
    clock_gettime(CLOCK_MONOTONIC, &start_edges);
    sprintf(imgFn, "%s", argv[1]); //"img.jpg");
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
      WriteSlidingWindowFloat((char *)("../matlab/edges.txt"), argv[1], sumsNorm);


      cout << "region " << regionId << endl;
      //for (i = 0; i < openings.size(); i++)
      //  cout << "opening at " << openings.at(i).start << " " << openings.at(i).length << endl;


      spaces = GetOpenParkingSpaces(openings, regionId);
      for (i = 0; i < spaces.size(); i++)
        cout << "space at " << spaces.at(i).start << " " << spaces.at(i).length << endl;

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
    cout << "susp start" << endl;
    clock_gettime(CLOCK_MONOTONIC, &start_suspact);
    if(justParked)
    {
      if(loopCount == 0)
      {
        carWindow = CreateWindow(topLeft, width, height, 0);
      }
      baseCount = GetBaseCount(edges, carWindow);
      cout << "curr frame " << baseCount << endl;
      bcAvg = UpdateEdgeList(edgeList, baseCount);
      cout << "BC AVG" << bcAvg << endl;
      if(loopCount == 10)
      {
        justParked = false;
        haveBC = true;
        baseCount = bcAvg;
      }
    }

    if(haveBC)
    {
      cout << "haveBC" << endl;
      alert = RunSusActivity(carParked, monitorON, resetCount, &actCount, baseCount, edges, carWindow, edgeList);
      clock_gettime(CLOCK_MONOTONIC, &finish_suspact);
      cout << "Alert" << alert << endl;
    }
 
    loopCount = loopCount + 1;
    cout << "Loop Count" << loopCount << endl;
    if(loopCount > 10)
    {
      loopCount = 0;
    }

//    alert = RunSusActivity(carParked, monitorON, resetCount, &actCount, baseCount, edges, carWindow);
    clock_gettime(CLOCK_MONOTONIC, &finish_suspact);
    cout << "base " << baseCount << endl;
    cout << alert << endl;

    // Capture time
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish_edges.tv_sec - start_edges.tv_sec); elapsed += (finish_edges.tv_nsec - start_edges.tv_nsec) / 1000000000.0;
    cout << "Edge Detect: " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_parking.tv_sec - start_parking.tv_sec); elapsed += (finish_parking.tv_nsec - start_parking.tv_nsec) / 1000000000.0;
    cout << "Parking:     " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_suspact.tv_sec - start_suspact.tv_sec); elapsed += (finish_suspact.tv_nsec - start_suspact.tv_nsec) / 1000000000.0;
    cout << "Suspicious:  " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish_db.tv_sec - start_db.tv_sec); elapsed += (finish_db.tv_nsec - start_db.tv_nsec) / 1000000000.0;
    cout << "Database:    " << elapsed * 1000.0 << " ms" << endl;
    elapsed = (finish.tv_sec - start.tv_sec); elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Total:       " << elapsed * 1000.0 << " ms" << endl;

    // Save edges image for DEBUG USE ONLY (REMOVE THIS)
    imwrite("./testimg/edges.jpg", edges);

    // Go to sleep
    break; // for development lets only run the loop once
    sleep(2);
  }

  #ifdef __arm__
    CloseDB(conn);
  #endif

  return 0;

}
