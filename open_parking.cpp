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

using namespace std;

int main(int argc, char** argv )
{
  struct timespec start, finish;
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
  bool carParked = true;
  bool monitorON = true;
  bool resetCount = false;
  bool justParked = true;
  bool alert = false;
  Window carWindow;
  int baseCount;
  int actCount = 0;
  int width;
  int height;
  Corner topLeft;
  topLeft.x = 1200;
  topLeft.y = 625;
  width = 150;
  height = 100;


  #ifdef __arm__
  MYSQL * conn = OpenDB((char*)K_DB);
  #endif

  // Main loop that will continue forever
  while (true)
  {
    // Let's time this
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Take an image
    TakeNewImage(imgFn, imgNum++);
    
    // Load source image
    sprintf(imgFn, "%s", argv[1]);
    src = imread(imgFn, 1);

    // Get edges
    thresh   = 100;
    cvtColor(src, src_gray, CV_BGR2GRAY);
    edges    = GetEdges(src_gray, thresh, 3, 3);

    // Loop through the subregions
    for (regionId = 0; regionId < K_NUM_SUBREGIONS; regionId++)
    {
      // Crop for subregion
      //roi = GetSubRegionImage(src, regionId);

      // Convert to grayscale
      //cvtColor(roi, roi_gray, CV_BGR2GRAY);

      // Main Image Processing
      //thresh   = 100;
      //edges    = GetEdges(roi_gray, thresh, 3, 3);
      startWin = GetStartWindow(regionId);
      endWin   = GetEndWindow(regionId);
      sums     = GetSlidingSum(edges, 0, startWin, endWin);
      sumsNorm = GetNormalizedSlidingSum(edges, 0, startWin, endWin);
      openings   = GetOpeningsFromSumsNormalized(sumsNorm, regionId);

      cout << "region " << regionId << endl;
      for (i = 0; i < openings.size(); i++)
        cout << "opening at " << openings.at(i).start << " " << openings.at(i).length << endl;

      spaces   = GetOpenParkingSpaces(openings, regionId);
      for (i = 0; i < spaces.size(); i++)
        cout << "space at " << spaces.at(i).start << " " << spaces.at(i).length << endl;

      if (spaces.size() > 0)
      {
        // Convert spaces to usable format
        spaces_db = FormatSpacesForDB(spaces, regionId, &spot_id);
        spaces_db_all.insert(spaces_db_all.end(), spaces_db.begin(), spaces_db.end());
        cout << "all " << spaces_db_all.size() << endl;
      }
    }


    #ifdef __arm__  // only on raspberry pi
      // Write to database (blocking)
      InsertOpenParking(spaces_db_all, conn);
      spaces_db_all.clear();
    #endif

    if(justParked)
    {
      //thresh   = 100;
      //cvtColor(src, src_gray, CV_BGR2GRAY);
      //edges    = GetEdges(src_gray, thresh, 3, 3);
      carWindow = CreateWindow(topLeft, width, height, 0);
      baseCount = GetBaseCount(edges, carWindow);
      justParked = false;
    }

    alert = RunSusActivity(carParked, monitorON, resetCount, &actCount, baseCount, edges, carWindow);
    cout << alert << endl;

    // Save edges image for debug use
    imwrite("./testimg/edges.jpg", edges);


    // Capture time
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Edge Detection: " << elapsed * 1000.0 << " ms" << endl;

    // Go to sleep
    break; // for development lets only run the loop once
    sleep(5);
  }

  #ifdef __arm__
    CloseDB(conn);
  #endif

  return 0;

}
