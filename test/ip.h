//-----------------------------------------------------------------------------
// File:        ip.h
// Author:      Garrett Bernichon
// Function:    Provide functions to do the low level image processing.
//-----------------------------------------------------------------------------

#ifndef IP_H
#define IP_H

#include <stdio.h>
#include <cv.hpp>
#include <time.h>
#include <iostream>

using namespace cv;
using namespace std;


// Corners
#define IP_CORNERS_THRESH_INIT      200
#define IP_CORNERS_FOUND_SATISFIED  300
#define IP_CORNERS_NEXT_THRESH_DIFF 5

// Edges


// Sliding Window
#define IP_WINDOW_WIDTH     10

struct Corner {
  int x;
  int y;
};

struct Window {
    Corner c0;
    Corner c1;
    Corner c2;
    Corner c3;
};

Mat GetOptimalCorners(Mat src_gray, int * thresh);

// Edges
Mat GetEdges(Mat src, int lowThreshold, int ratio, int kernelSize);

// Utilities
vector<int> GetSlidingSum(Mat img, int count, int thresh, Window startWindow, Window endWindow);
vector<Window> GetSlidingWindow(int stepCount, Window startWindow, Window endWindow, int imgHeight, int imgWidth);
bool IsWithinBounds(int imgHeight, int imgWidth, Window win);

#endif
