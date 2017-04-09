//-----------------------------------------------------------------------------
// File:        ip.h
// Author:      Garrett Bernichon
// Function:    Provide functions to do the low level image processing.
//-----------------------------------------------------------------------------

#ifndef IP_H
#define IP_H

#include <stdio.h>
#include <cmath>
#include <cv.hpp>
#include <time.h>
#include <iostream>
#include <fstream>
#include "common.h"

#ifdef __arm__
  #include <mysql/mysql.h>
  #include "db_utils.h"
#endif

using namespace cv;
using namespace std;


// Corners
#define IP_CORNERS_THRESH_INIT      200
#define IP_CORNERS_FOUND_SATISFIED  500
#define IP_CORNERS_NEXT_THRESH_DIFF 5

// Edges
#define K_LEFT_EDGE     1
#define K_RIGHT_EDGE    0
#define K_MAX_EDGE_LENGTH 200

// Sliding Window
#define IP_WINDOW_WIDTH     10

// Openings
#define K_MINIMUM_OPENING_LENGTH      3
#define K_SUMS_THRESHOLD_CONSECUTIVE  20

///// REGION DIMENSIONS /////
#define K_NUM_SUBREGIONS    6

// Beason Northeast
#define K_BEASON_NE_ID      0
#define K_BEASON_NE_X       830
#define K_BEASON_NE_Y       470
#define K_BEASON_NE_WIDTH   720
#define K_BEASON_NE_HEIGHT  115
#define K_BEASON_NE_WIN_START_TP_X      0
#define K_BEASON_NE_WIN_START_TP_Y      75//10
#define K_BEASON_NE_WIN_START_WIDTH     5
#define K_BEASON_NE_WIN_START_HEIGHT    30//95
#define K_BEASON_NE_WIN_START_THETA     0.0
#define K_BEASON_NE_WIN_END_TP_X        K_BEASON_NE_WIDTH-K_BEASON_NE_WIN_START_WIDTH-1
#define K_BEASON_NE_WIN_END_TP_Y        75//60
#define K_BEASON_NE_WIN_END_WIDTH       5
#define K_BEASON_NE_WIN_END_HEIGHT      30//50
#define K_BEASON_NE_WIN_END_THETA       0.0//20.0
#define K_BEASON_NE_SUMS_THRESHOLD      0.1//30

// Beason Southeast
#define K_BEASON_SE_ID      1
#define K_BEASON_SE_X       760
#define K_BEASON_SE_Y       640
#define K_BEASON_SE_WIDTH   810
#define K_BEASON_SE_HEIGHT  250
#define K_BEASON_SE_WIN_START_TP_X    0
#define K_BEASON_SE_WIN_START_TP_Y    150
#define K_BEASON_SE_WIN_START_WIDTH   5
#define K_BEASON_SE_WIN_START_HEIGHT  99
#define K_BEASON_SE_WIN_START_THETA   0.0//15.0
#define K_BEASON_SE_WIN_END_TP_X      750
#define K_BEASON_SE_WIN_END_TP_Y      0
#define K_BEASON_SE_WIN_END_WIDTH     5
#define K_BEASON_SE_WIN_END_HEIGHT    60
#define K_BEASON_SE_WIN_END_THETA     0.0//35.0
#define K_BEASON_SE_SUMS_THRESHOLD    0.1//150

// Beason Southwest
#define K_BEASON_SW_ID      2
#define K_BEASON_SW_X       310
#define K_BEASON_SW_Y       615
#define K_BEASON_SW_WIDTH   220
#define K_BEASON_SW_HEIGHT  175
#define K_BEASON_SW_WIN_START_TP_X    20
#define K_BEASON_SW_WIN_START_TP_Y    0
#define K_BEASON_SW_WIN_START_WIDTH   1
#define K_BEASON_SW_WIN_START_HEIGHT  50
#define K_BEASON_SW_WIN_START_THETA   -10.0
#define K_BEASON_SW_WIN_END_TP_X      220
#define K_BEASON_SW_WIN_END_TP_Y      70
#define K_BEASON_SW_WIN_END_WIDTH     1
#define K_BEASON_SW_WIN_END_HEIGHT    80
#define K_BEASON_SW_WIN_END_THETA     -10.0
#define K_BEASON_SW_SUMS_THRESHOLD    50

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
#define K_BEASON_NW_SUMS_THRESHOLD    50

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
#define K_COOKSIE_NW_SUMS_THRESHOLD    50

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
#define K_COOKSIE_SW_SUMS_THRESHOLD    50

/////  END  DIMENSIONS  /////

// Misc
#define source_window   "Source image"
#define corners_window  "Corners detected"
#define window_name     "Edge Map"

struct Corner {
  int x;
  int y;
};

struct ImgPoint {
  int x;
  int y;
};

struct xy {
  int x;
  int y;
};

struct Window {
    Corner tl; // top left
    Corner tr; // top right
    Corner bl; // bottom left
    Corner br; // bottom right
    int width;
    int height;
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
ImgPoint GetNextStartingPoint(ImgPoint currPos, Window win, int edge);
ImgPoint TraverseEdge(Window win, Mat edges, ImgPoint startPos, int edge);
ImgPoint GetNextPos(Window win, Mat edges, ImgPoint currPos, ImgPoint* lastPos);
bool IsInsideWindow(Window win, ImgPoint pos);

// Utilities
vector<int>     GetSlidingSum(Mat img, int thresh, Window startWindow, Window endWindow);
vector<float>   GetNormalizedSlidingSum(Mat img, int thresh, Window startWindow, Window endWindow);
int             GetSumOfWindow(Mat img, Window win, int thresh);
vector<int>     GetSlidingEdges(Mat edges, Window startWindow, Window endWindow, float minDiff, int edge);
vector<Opening> GetOpeningsFromSums(vector<int> sums, int regionId);
vector<Opening> GetOpeningsFromSumsNormalized(vector<float> sums, int regionId);
vector<Opening> GetOpenParkingSpaces(vector<Opening> openings, int regionId);
bool            IsOpeningLargeEnough(Opening opening, int regionId);
vector<Opening> GetOpenings(vector<int> leftEdges, vector<int> rightEdges);
vector<Window>  GetSlidingWindow(Window startWindow, Window endWindow, int imgHeight, int imgWidth);
bool            IsWithinBounds(int imgHeight, int imgWidth, Window win);

Mat     GetSubRegionImage(Mat original, int regionId);
Window  GetStartWindow(int regionId);
Window  GetEndWindow(int regionId);
float   GetThresholdFromRegionId(int regionId);
float   GetDistance(ImgPoint a, ImgPoint b);
Window  CreateWindow(Corner topLeft, int width, int height, float theta);
double  Degrees2Radians(double deg);

//Pedestrian Detection
bool RunSusActivity(bool carParked, bool monitorON, bool resetCount, 
int* actCount, int baseCount, Mat image, Window carWindow);
int DetectActivity(Mat image, Window carWindow, int baseCount);
int GetBaseCount(Mat image, Window carWindow);


// Database 
vector<OPEN_SPOT_T> FormatSpacesForDB(vector<Opening> spaces, int region, int * spot_id);
int GetXPositionOfSpot(int regionId, int start);
int GetYPositionOfSpot(int regionId, int start);
void GetCornersOfSpot(Corner * corners, int regionId, int start);
#ifdef __arm__
void InsertOpenParking(vector<OPEN_SPOT_T> spaces_db, MYSQL * conn);
#endif

// Output
void WriteSlidingWindow(char * fn, char * imgfn, vector<int> sums);
void WriteSlidingWindowFloat(char * fn, char * imgfn, vector<float> sums);
void WriteOpenings(char * fn, char * imgfn, vector<Opening> openings);

// System
void TakeNewImage(char * fn, unsigned int num);

#endif
