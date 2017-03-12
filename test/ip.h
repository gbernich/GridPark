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
#include <fstream>

using namespace cv;
using namespace std;


// Corners
#define IP_CORNERS_THRESH_INIT      200
#define IP_CORNERS_FOUND_SATISFIED  500
#define IP_CORNERS_NEXT_THRESH_DIFF 5

// Edges
#define K_LEFT_EDGE     1
#define K_RIGHT_EDGE    0

// Sliding Window
#define IP_WINDOW_WIDTH     5

///// REGION DIMENSIONS /////

// Beason Northeast
#define K_BEASON_NE_ID      0
#define K_BEASON_NE_X       830
#define K_BEASON_NE_Y       470
#define K_BEASON_NE_WIDTH   720
#define K_BEASON_NE_HEIGHT  115
#define K_BEASON_NE_WIN_START_TP_X      0
#define K_BEASON_NE_WIN_START_TP_Y      10
#define K_BEASON_NE_WIN_START_WIDTH     5
#define K_BEASON_NE_WIN_START_HEIGHT    100
#define K_BEASON_NE_WIN_START_THETA     0.0
#define K_BEASON_NE_WIN_END_TP_X        K_BEASON_NE_WIDTH-K_BEASON_NE_WIN_START_WIDTH-1
#define K_BEASON_NE_WIN_END_TP_Y        60
#define K_BEASON_NE_WIN_END_WIDTH       5
#define K_BEASON_NE_WIN_END_HEIGHT      54
#define K_BEASON_NE_WIN_END_THETA       0.0

// Beason Southeast
#define K_BEASON_SE_ID      1
#define K_BEASON_SE_X       760
#define K_BEASON_SE_Y       640
#define K_BEASON_SE_WIDTH   810
#define K_BEASON_SE_HEIGHT  250
#define K_BEASON_SE_WIN_START_TP_X    0
#define K_BEASON_SE_WIN_START_TP_Y    0
#define K_BEASON_SE_WIN_START_WIDTH   0
#define K_BEASON_SE_WIN_START_HEIGHT  0
#define K_BEASON_SE_WIN_START_THETA   0.0
#define K_BEASON_SE_WIN_END_TP_X      0
#define K_BEASON_SE_WIN_END_TP_Y      0
#define K_BEASON_SE_WIN_END_WIDTH     5
#define K_BEASON_SE_WIN_END_HEIGHT    115
#define K_BEASON_SE_WIN_END_THETA     0.0

// Beason Southwest
#define K_BEASON_SW_ID      2
#define K_BEASON_SW_X       310
#define K_BEASON_SW_Y       615
#define K_BEASON_SW_WIDTH   220
#define K_BEASON_SW_HEIGHT  175
#define K_BEASON_SW_WIN_START_TP_X    0
#define K_BEASON_SW_WIN_START_TP_Y    0
#define K_BEASON_SW_WIN_START_WIDTH   0
#define K_BEASON_SW_WIN_START_HEIGHT  0
#define K_BEASON_SW_WIN_START_THETA   0.0
#define K_BEASON_SW_WIN_END_TP_X      0
#define K_BEASON_SW_WIN_END_TP_Y      0
#define K_BEASON_SW_WIN_END_WIDTH     5
#define K_BEASON_SW_WIN_END_HEIGHT    115
#define K_BEASON_SW_WIN_END_THETA     0.0

// Beason Northwest
#define K_BEASON_NW_ID      3
#define K_BEASON_NW_X       390
#define K_BEASON_NW_Y       490
#define K_BEASON_NW_WIDTH   280
#define K_BEASON_NW_HEIGHT  80
#define K_BEASON_NW_WIN_START_TP_X    0
#define K_BEASON_NW_WIN_START_TP_Y    0
#define K_BEASON_NW_WIN_START_WIDTH   0
#define K_BEASON_NW_WIN_START_HEIGHT  0
#define K_BEASON_NW_WIN_START_THETA   0.0
#define K_BEASON_NW_WIN_END_TP_X      0
#define K_BEASON_NW_WIN_END_TP_Y      0
#define K_BEASON_NW_WIN_END_WIDTH     5
#define K_BEASON_NW_WIN_END_HEIGHT    115
#define K_BEASON_NW_WIN_END_THETA     0.0

// Cooksie Northwest
#define K_COOKSIE_NW_ID     4
#define K_COOKSIE_NW_X      690
#define K_COOKSIE_NW_Y      310
#define K_COOKSIE_NW_WIDTH  180
#define K_COOKSIE_NW_HEIGHT 170
#define K_COOKSIE_NW_WIN_START_TP_X    0
#define K_COOKSIE_NW_WIN_START_TP_Y    0
#define K_COOKSIE_NW_WIN_START_WIDTH   0
#define K_COOKSIE_NW_WIN_START_HEIGHT  0
#define K_COOKSIE_NW_WIN_START_THETA   0.0
#define K_COOKSIE_NW_WIN_END_TP_X      0
#define K_COOKSIE_NW_WIN_END_TP_Y      0
#define K_COOKSIE_NW_WIN_END_WIDTH     5
#define K_COOKSIE_NW_WIN_END_HEIGHT    115
#define K_COOKSIE_NW_WIN_END_THETA     0.0

// Cooksie Southwest
#define K_COOKSIE_SW_ID     5
#define K_COOKSIE_SW_X      0
#define K_COOKSIE_SW_Y      0
#define K_COOKSIE_SW_WIDTH  0
#define K_COOKSIE_SW_HEIGHT 0
#define K_COOKSIE_SW_WIN_START_TP_X    0
#define K_COOKSIE_SW_WIN_START_TP_Y    0
#define K_COOKSIE_SW_WIN_START_WIDTH   0
#define K_COOKSIE_SW_WIN_START_HEIGHT  0
#define K_COOKSIE_SW_WIN_START_THETA   0.0
#define K_COOKSIE_SW_WIN_END_TP_X      0
#define K_COOKSIE_SW_WIN_END_TP_Y      0
#define K_COOKSIE_SW_WIN_END_WIDTH     5
#define K_COOKSIE_SW_WIN_END_HEIGHT    115
#define K_COOKSIE_SW_WIN_END_THETA     0.0

/////  END  DIMENSIONS  /////

struct Corner {
  int x;
  int y;
};

struct ImgPoint {
  int x;
  int y;
};

struct Window {
    Corner tl; // top left
    Corner br; // bottom right
    float theta;    // tilt from vertical, with tl as anchor
                    // 0.0 is vertical, 90.0 horizontal
};

struct Opening {
    int start;
    int length;
};

Mat GetOptimalCorners(Mat src_gray, int * thresh);

// Edges
Mat GetEdges(Mat src, int lowThreshold, int ratio, int kernelSize);

int CountMeaningfulEdges(Window win, Mat edges, float minDiff, int edge);
bool IsOnStartingEdge(ImgPoint pos, Window win, int edge);
ImgPoint GetNextStartingPoint(ImgPoint currPos, Window win);
ImgPoint TraverseEdge(Window win, Mat edges, ImgPoint currPos, int edge);
ImgPoint GetNextPos(Window win, Mat edges, ImgPoint currPos, ImgPoint lastPos);
bool IsInsideWindow(Window win, ImgPoint pos);

// Utilities
vector<int>     GetSlidingSum(Mat img, int thresh, Window startWindow, Window endWindow);
vector<int>     GetSlidingEdges(Mat edges, Window startWindow, Window endWindow, float minDiff, int edge);
vector<Opening> GetOpenings(vector<int> leftEdges, vector<int> rightEdges);
vector<Window>  GetSlidingWindow(Window startWindow, Window endWindow, int imgHeight, int imgWidth);
bool            IsWithinBounds(int imgHeight, int imgWidth, Window win);

Mat     GetSubRegionImage(Mat original, int regionId);
Window  GetStartWindow(int regionId);
Window  GetEndWindow(int regionId);

// Output
void WriteSlidingWindow(char * fn, char * imgfn, vector<int> sumsVector, int windowSizes);
void WriteOpenings(char * fn, char * imgfn, vector<Opening> openings);

#endif
