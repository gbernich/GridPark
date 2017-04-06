#include <stdio.h>
#include <time.h>
#include <iostream>
#include <unistd.h>
#include "ip.h"

using namespace std;


Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int max_lowThreshold = 250;
int ratio1 = 3;
int kernel_size = 3;

char imgFn[80] = {0};
char imgNum = 0;

int main(int argc, char** argv )
{  
  struct timespec start, finish;
  double elapsed;
  int i, count, x, y, x0, y0, x3, y3;
  int regionId;

  // Matrices
  Mat src, roi, roi_gray, overlay;

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


  // Main loop that will continue forever
  while (true)
  {
    // Let's time this
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Take an image
    TakeNewImage(imgFn, imgNum++);
    
    // Load source image
    sprintf(imgFn, "%s", "/Users/garrettbernichon/Desktop/image8_s50.jpg");
    src = imread(imgFn, 1);

    // Loop through the subregions
    for (regionId = 0; regionId < K_NUM_SUBREGIONS; regionId++)
    {
      // Crop for subregion
      roi = GetSubRegionImage(src, regionId);

      // Convert to grayscale
      cvtColor(roi, roi_gray, CV_BGR2GRAY);

      // Main Image Processing
      thresh   = 100;
      edges    = GetEdges(roi_gray, thresh, 3, 3);
      startWin = GetStartWindow(regionId);
      endWin   = GetEndWindow(regionId);
      sums     = GetSlidingSum(edges, 0, startWin, endWin);
      sumsNorm = GetNormalizedSlidingSum(edges, 0, startWin, endWin);
      openings = GetOpeningsFromSumsNormalized(sumsNorm, regionId);
      spaces   = GetOpenParkingSpaces(openings, regionId);

      // Convert spaces to usable format

      // Write to database

    }

    // Capture Time
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Edge Detection: " << elapsed * 1000.0 << " ms" << endl;

    // Go to sleep
    sleep(5);
  }

  return 0;

}
