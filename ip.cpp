//-----------------------------------------------------------------------------
// File:        ip.c
// Author:      Garrett Bernichon
// Function:    Provide functions to do the low level image processing.
//-----------------------------------------------------------------------------
#include "ip.h"

int debug = 0;

// Corner Detection ////////////////////////////////////////////////////////////
Mat GetOptimalCorners(Mat src, int * thresh)
{
  vector<Corner> corners, corners_tmp;
  Corner currCorner;
  Mat dst, dst_norm, dst_norm_scaled;
  dst = Mat::zeros( src.size(), CV_32FC1 );
  int iters = 0;

  // Detector parameters
  int blockSize = 2;
  int apertureSize = 3;
  double k = 0.04;

  /// Detecting corners
  cornerHarris( src, dst, blockSize, apertureSize, k, BORDER_DEFAULT );

  /// Normalizing
  normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
  convertScaleAbs( dst_norm, dst_norm_scaled );

  do {
    corners = corners_tmp;
    corners_tmp.clear();

    for( int j = 0; j < dst_norm.rows ; j++ ) {
      for( int i = 0; i < dst_norm.cols; i++ ) {
        if( (int) dst_norm.at<float>(j,i) > *thresh )
        {
          currCorner.x = i;
          currCorner.y = j;
          corners_tmp.push_back(currCorner);
        }
      }
    }
    iters++;
    (*thresh)--;
  } while (corners_tmp.size() < IP_CORNERS_FOUND_SATISFIED);

  (*thresh)++;
  return dst_norm;  

  // // Get the last good corner vector
  // if (iters == 1)
  //    corners = corners_tmp;

  // // Debugging
  // printf("Thresh = %d, corners = %lu\n", *thresh+1, corners.size());

  // // Set up the threshold for the next iteration
  // *thresh += 1 + IP_CORNERS_NEXT_THRESH_DIFF;

  // return corners;
}

// Edge Detection //////////////////////////////////////////////////////////////
Mat GetEdges(Mat src, int lowThreshold, int ratio, int kernelSize)
{
    Mat blurred, edges, edges_norm;  

    /// Reduce noise with a kernel 3x3
    blur( src, blurred, Size(3,3) );

    /// Canny detector
    Canny( blurred, edges, lowThreshold, lowThreshold * ratio, kernelSize );

    normalize( edges, edges_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    //convertScaleAbs( edges_norm, dst_norm_scaled );

    /// Using Canny's output as a mask, we display our result
    // dst = Scalar::all(0);

    // src.copyTo( dst, edges);
    // imshow( window_name, egdes );

    return edges_norm;
}

int CountMeaningfulEdges(Window win, Mat edges, float minDiff, int edge)
{
  ImgPoint currPos, startPos, endPos;

  if (edge == K_LEFT_EDGE){
    startPos.x = win.tl.x;
    startPos.y = win.tl.y;
  }
  else{
    startPos.x = win.tl.x + win.width * cos(Degrees2Radians(win.theta));
    startPos.y = win.tl.y + win.width * sin(Degrees2Radians(win.theta));
  }
  
  currPos = startPos;
  int count = 0;

  //printf("1\n");
  // Start at the top left of the window
  // and move down the edge of the window
  while (currPos.x != -1)
  {
    //printf("2\n");
    // If there is an edge, traverse it
    if (edges.at<float>(currPos.y, currPos.x) > 0) 
    {
      //printf("3\n");
      endPos = TraverseEdge(win, edges, currPos, edge);
      if (endPos.x != -1){
        //cout << "start " << currPos.x << " " << currPos.y << endl;
        //cout << "end " << endPos.x << " " << endPos.y << endl;

        if (IsOnStartingEdge(endPos, win, edge))
        {
          // cout << "start " << currPos.y << " end " << endPos.y << endl;
          // if ((float)(endPos.y - currPos.y) >= minDiff * (float)(win.br.y - win.tl.y))
          // printf("dist = %f\n", GetDistance(currPos, endPos));
          if (GetDistance(currPos, endPos) >= minDiff * (float)(win.height))
          //if (GetDistance(currPos, endPos) >= 2)
          {
            count++;
            //cout << "counted, dist " << GetDistance(currPos, endPos) << endl;
          }
        }
      }
    }

    // Move along the window edge
    currPos = GetNextStartingPoint(currPos, win, edge);
  }
  //cout << count << endl;
  return count;
}

bool IsOnStartingEdge(ImgPoint pos, Window win, int edge)
{
  if (edge == K_LEFT_EDGE){
    //printf("expected %d, actual %d\n", (int)((float)win.tl.x + (pos.y - win.tl.y) * tan(Degrees2Radians(win.theta))),pos.x);
    if (pos.x == (int)((float)win.tl.x + (pos.y - win.tl.y) * tan(Degrees2Radians(win.theta))))
      return true;
  }else{
    if (pos.x == (int)((float)win.tr.x + (pos.y - win.tr.y) * tan(Degrees2Radians(win.theta))))
      return true;
  }

  return false;
}

ImgPoint GetNextStartingPoint(ImgPoint currPos, Window win, int edge)
{
  ImgPoint nextPos;
  nextPos.y = currPos.y + 1;

  if (edge == K_LEFT_EDGE){
    nextPos.x = win.tl.x + (nextPos.y - win.tl.y) * tan(Degrees2Radians(win.theta));
    if (nextPos.y > win.bl.y)
    {
      nextPos.x = -1;
      nextPos.y = -1;
    }
  }else{
    nextPos.x = win.tr.x + (nextPos.y - win.tr.y) * tan(Degrees2Radians(win.theta));
    if (nextPos.y > win.br.y)
    {
      nextPos.x = -1;
      nextPos.y = -1;
    }
  }
  //printf("start x %d, y %d\n", nextPos.x, nextPos.y);
  return nextPos;
}

ImgPoint TraverseEdge(Window win, Mat edges, ImgPoint startPos, int edge)
{
  int front = 0;
  int edgeLength = 0;
  ImgPoint lastPos[9] = {{-2,-2}};
  ImgPoint currPos = startPos;
  lastPos[front++] = currPos;
  ImgPoint nextPos = GetNextPos(win, edges, currPos, lastPos);
  if (nextPos.x == -1)
    currPos = nextPos;
  //printf("4\n");
  while (nextPos.x != -1)
  {
    //printf("5\n");
    //lastPos.push_front(currPos);
    currPos = nextPos;

    // if ((currPos.x == win.tl.x && edge == K_LEFT_EDGE) ||
    //     (currPos.x == win.br.x && edge == K_RIGHT_EDGE))
    if (IsOnStartingEdge(currPos, win, edge))
      break;
    nextPos = GetNextPos(win, edges, currPos, lastPos);
    
    if(edgeLength++ >= K_MAX_EDGE_LENGTH) // prevent infinite loops
    {
      currPos.x = -1;
      currPos.y = -1;
      break;
    }

    lastPos[front++] = currPos;
    if (front > 8)
      front = 0;
    //cout << nextPos.x << " " << nextPos.y << endl;
  }
  //printf("finished edge, length %d\n", edgeLength);

  return currPos;
}

ImgPoint GetNextPos(Window win, Mat edges, ImgPoint currPos, ImgPoint* lastPos)
{
  ImgPoint nextPos, tryPos;
  int attemptedPos[3][3] = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}}; // only for vertical
  int xDiff, yDiff;

  // Mark last positions so that we don't go in a circle forever
  for(int i = 0; i < 9; i++)
  {
    xDiff = (lastPos[i].x - currPos.x) + 1;
    yDiff = (lastPos[i].y - currPos.y) + 1;
    if (0 <= xDiff && xDiff <= 2 && 0 <= yDiff && yDiff <= 2)
      attemptedPos[yDiff][xDiff] = 1;
  }

  // Init to null
  nextPos.x = -1;
  nextPos.y = -1;

  for (int j = 2; j >= 0; j--){
    for (int i = 0; i <= 2; i++){
      tryPos.x = currPos.x + i - 1;
      tryPos.y = currPos.y + j - 1;
      if( attemptedPos[j][i] == 0 &&
          edges.at<float>(tryPos.y, tryPos.x) > 0 &&
          IsInsideWindow(win, tryPos))
      {
        nextPos = tryPos;
        j = -1; i = 3; break;
      }
    }
  }
  //if(nextPos.x == -1)
    //printf("GetNext is null\n");
  return nextPos;

}

bool IsInsideWindow(Window win, ImgPoint pos)
{  
  float top, bottom, left, right;

  // if theta ~0.0 then treat as a vertical rectangle
  if (-0.0001 <= win.theta && win.theta <= 0.0001)
  {
    if (win.tl.x <= pos.x && pos.x <= win.br.x &&
        win.tl.y <= pos.y && pos.y <= win.br.y)
      return true;
    else
      return false; 
  }

  // Create a slope for each edge
  top     = (float)(win.tr.y - win.tl.y)/(float)(win.tr.x - win.tl.x);
  bottom  = (float)(win.br.y - win.bl.y)/(float)(win.br.x - win.bl.x);
  left    = (float)(win.bl.y - win.tl.y)/(float)(win.bl.x - win.tl.x);
  right   = (float)(win.br.y - win.tr.y)/(float)(win.br.x - win.tr.x);
  
  // might only work for theta > 0
  if (pos.y < win.tl.y + top    * (pos.x - win.tl.x))  { return false; }
  if (pos.y > win.bl.y + bottom * (pos.x - win.bl.x))  { return false; }
  if (pos.y > win.tl.y + left   * (pos.x - win.tl.x))  { return false; }
  if (pos.y < win.tr.y + right  * (pos.x - win.tr.x))  { return false; }

  return true;
}
////////////////////////////////////////////////////////////////////////////////


// Utilities ///////////////////////////////////////////////////////////////////
vector<int> GetSlidingSum(Mat img, int thresh, Window startWindow, Window endWindow)
{
  vector<Window> windows;
  Window win;
  vector<int> sums;
  int sum, i, x, y = 0;
  int pixel;

  windows = GetSlidingWindow(startWindow, endWindow, img.rows, img.cols);

  // Sum
  for (i = 0; i < windows.size(); i++)
  {
    win = windows.at(i);

    sum = GetSumOfWindow(img, windows.at(i), thresh);

    sums.push_back(sum);
  }
  return sums;
}

vector<float> GetNormalizedSlidingSum(Mat img, int thresh, Window startWindow, Window endWindow)
{
  vector<Window> windows;
  Window win;
  vector<float> sums;
  int sum, i, x, y = 0;
  int pixel;

  windows = GetSlidingWindow(startWindow, endWindow, img.rows, img.cols);
  //cout << img.rows << endl;
  // Sum
  for (i = 0; i < windows.size(); i++)
  {
    win = windows.at(i);

    sum = GetSumOfWindow(img, windows.at(i), thresh);

    sums.push_back(sum / (float)windows.at(i).height);
  }
  return sums;
}

int GetSumOfWindow(Mat img, Window win, int thresh)
{ // ONLY SUPPORTS A LINE //
  
  int sum = 0;
  int x = win.tl.x;
  int y = win.tl.y;
  int pixel = 0;
  int ylimit = win.bl.y;
  int xlimit = 0;
  ImgPoint tmpPoint;

  // Traverse down the left edge of the winow
  while (y <= ylimit)
  {
    // Get X limit
    xlimit = (int)(win.tr.x + (y - win.tr.y) * tan(win.theta));

    while (x <= xlimit)
    {
      tmpPoint.x = x;
      tmpPoint.y = y;

      // Check the image
      if (IsInsideWindow(win, tmpPoint)){
        pixel = (int)img.at<float>(y, x);
        if (pixel > thresh)
          sum++;
      }
      x++;
    }



    // Update y position
    y++;

    // Update x position
    x = (int)(win.tl.x + (y - win.tl.y) * tan(win.theta));
  }

  return sum;
}

vector<int> GetSlidingEdges(Mat edges, Window startWindow, Window endWindow, float minDiff, int edge)
{
  vector<Window> windows;
  Window win;
  vector<int> sums;
  int sum, i, x, y = 0;
  int pixel;

  windows = GetSlidingWindow(startWindow, endWindow, edges.rows, edges.cols);
  //printf("window size %lu\n", windows.size());
  // Sum
  for (i = 0; i < windows.size(); i++)
  {
    win = windows.at(i);
    sum = CountMeaningfulEdges(win, edges, minDiff, edge);    
    sums.push_back(sum);
  }
  return sums;
}

vector<Opening> GetOpeningsFromSums(vector<int> sums, int regionId)
{
  vector<Opening> openings;
  Opening newOpening;
  int aboveCount = 0;
  int belowCount = 0;
  bool activeBelow = false;
  int startBelow = 0;
  int thresh = (int)GetThresholdFromRegionId(regionId);

  // Slide across the sums and count for consecutive points above/below a threshold
  for (int i = 0; i < sums.size(); i++)
  {
    // if below threshold, mark it as active and record the index
    if (sums.at(i) < thresh)
    {
      //cout << "below " << sums.at(i) << endl;
      if (!activeBelow){
        activeBelow = true;
        startBelow = i;
      }
      aboveCount = 0;
    }
    // if above the threshold, count number of consecutive samples above it.
    // if we have consecutive samples above the threshold, reset the below vars
    // and record the opening
    else
    {
      if (++aboveCount >= K_SUMS_THRESHOLD_CONSECUTIVE)
      {
        //cout << "above " << i << " " << startBelow << endl;
        activeBelow = false;
        newOpening.start = startBelow;
        newOpening.length = i - startBelow - aboveCount + 1;
        openings.push_back(newOpening);
      }
    }
  }

  return openings;
}

vector<Opening> GetOpeningsFromSumsNormalized(vector<float> sums, int regionId)
{
  vector<Opening> openings;
  Opening newOpening;
  int aboveCount = 0;
  int belowCount = 0;
  bool activeBelow = false;
  bool added = false;
  int startBelow = 0;
  float thresh = GetThresholdFromRegionId(regionId);

  // Slide across the sums and count for consecutive points above/below a threshold
  for (int i = 0; i < sums.size(); i++)
  {
    // if below threshold, mark it as active and record the index
    if (sums.at(i) < thresh)
    {
      //cout << "below " << sums.at(i) << endl;
      if (!activeBelow){
        activeBelow = true;
        startBelow = i;
        added = false;
      }
      aboveCount = 0;
    }
    // if above the threshold, count number of consecutive samples above it.
    // if we have consecutive samples above the threshold, reset the below vars
    // and record the opening
    else
    {
      if (++aboveCount >= K_SUMS_THRESHOLD_CONSECUTIVE && not added)
      {
        //cout << "above " << i << " " << startBelow << endl;
        activeBelow = false;
        newOpening.start = startBelow;
        newOpening.length = i - startBelow - aboveCount + 1;
        openings.push_back(newOpening);
        added = true;
      }
    }
  }

  return openings;
}

vector<Opening> GetOpenParkingSpaces(vector<Opening> openings, int regionId)
{
  vector<Opening> spaces;

  for (int i = 0; i < openings.size(); i++)
  {
    if (IsOpeningLargeEnough(openings.at(i), regionId))
      spaces.push_back(openings.at(i));
  }

  return spaces;
}

bool IsOpeningLargeEnough(Opening opening, int regionId)
{
  int result = false;
  int min = 10000;
  switch(regionId){
    case (K_BEASON_NE_ID):
      //min = 180 - (int)(opening.start * 0.25);
      return true;
      break;
    case (K_BEASON_SE_ID):
      min = 250 - (int)(opening.start * 0.5);
      break;
    case (K_BEASON_SW_ID):
      break;
    case (K_BEASON_NW_ID):
      break;
    case (K_COOKSIE_NW_ID):
      break;
    case (K_COOKSIE_SW_ID):
      break;
  }

  if (opening.length >= min)
    return true;
  else
    return false;
}

vector<Opening> GetOpenings(vector<int> leftEdges, vector<int> rightEdges)
{
  vector<Opening> openings;
  Opening newOpening;
  int i = 0;
  int startInd = 0;
  int endInd = 0;
  bool activeOpening = true;

  while ( i < (leftEdges.size()-1 ))
  { 
    // Consider the beginning of the image to be the start of an opening
    // Wait until we hit a right edge to close the opening
    if (activeOpening && (rightEdges.at(i) > 0 || leftEdges.at(i) > 0))
    {
      endInd = i-1;
      newOpening.start = startInd;
      newOpening.length = endInd - startInd + 1;
      if (newOpening.length > K_MINIMUM_OPENING_LENGTH)
        openings.push_back(newOpening);
      activeOpening = false;
    }
    else
    {
      // Wait for a left edge to start an opening
      if (!activeOpening && leftEdges.at(i) == 0 && leftEdges.at(i-1) > 0)
      {
        startInd = i;
        activeOpening = true;
      }
    }
    i++;
  }
  return openings;
}

vector<Window> GetSlidingWindow(Window startWindow, Window endWindow, int imgHeight, int imgWidth)
{
    int stepCount = endWindow.tl.x - startWindow.tl.x;
    vector<Window> windows;
    Window currWindow = startWindow;
    Window diffWindow;
    float dx0, dy0, dtheta, dheight;
    float tlx, tly, theta, height;
    Corner tmp;
    
    // Initial values (as floats)
    tlx = (float)startWindow.tl.x;
    tly = (float)startWindow.tl.y;
    theta = startWindow.theta;
    height = startWindow.height;

    // Calculate the step size between each window
    dx0 = float(endWindow.tl.x - startWindow.tl.x) / (float)stepCount;
    dy0 = float(endWindow.tl.y - startWindow.tl.y) / (float)stepCount;
    dtheta = float(endWindow.theta - startWindow.theta) / (float)stepCount;
    dheight = float(endWindow.height - startWindow.height) / (float)stepCount;

    while (IsWithinBounds(imgHeight, imgWidth, currWindow) && stepCount > -1)
    {
        // add window
        windows.push_back(currWindow);

        // update window
        tlx += dx0;
        tly += dy0;
        theta += dtheta;
        height += dheight;
        //printf("%d %d %f %d\n", (int)tlx, (int)tly, theta, (int)height);
        
        // convert back to integers to save window
        tmp.x = tlx;
        tmp.y = tly;
        currWindow = CreateWindow(tmp, startWindow.width, height, theta);
        // printf("%d %d   %d %d   %d %d   %d %d\n",
        //   currWindow.tl.x, currWindow.tl.y,
        //   currWindow.tr.x, currWindow.tr.y,
        //   currWindow.bl.x, currWindow.bl.y,
        //   currWindow.br.x, currWindow.br.y);

        stepCount--;
    }
    //printf("length %lu\n", windows.size());
    return windows;
}

bool IsWithinBounds(int imgHeight, int imgWidth, Window win)
{
  // Check all four corners
  if (0 > win.tl.x || win.tl.x > imgWidth-1)  { return false; }
  if (0 > win.tl.y || win.tl.y > imgHeight-1) { return false; }

  if (0 > win.tr.x || win.tr.x > imgWidth-1)  { return false; }
  if (0 > win.tr.y || win.tr.y > imgHeight-1) { return false; }

  if (0 > win.bl.x || win.bl.x > imgWidth-1)  { return false; }
  if (0 > win.bl.y || win.bl.y > imgHeight-1) { return false; }

  if (0 > win.br.x || win.br.x > imgWidth-1)  { return false; }
  if (0 > win.br.y || win.br.y > imgHeight-1) { return false; }
  
  return true;
}

Mat GetSubRegionImage(Mat original, int regionId)
{
  Rect roi(0,0,0,0);
  regionId = 0;
  switch(regionId){
    case (K_BEASON_NE_ID):
      roi += Point(K_BEASON_NE_X, K_BEASON_NE_Y);
      roi += Size(K_BEASON_NE_WIDTH, K_BEASON_NE_HEIGHT);
      break;
    case (K_BEASON_SE_ID):
      roi += Point(K_BEASON_SE_X, K_BEASON_SE_Y);
      roi += Size(K_BEASON_SE_WIDTH, K_BEASON_SE_HEIGHT);
      break;
    case (K_BEASON_SW_ID):
      roi += Point(K_BEASON_SW_X, K_BEASON_SW_Y);
      roi += Size(K_BEASON_SW_WIDTH, K_BEASON_SW_HEIGHT);
      break;
    case (K_BEASON_NW_ID):
      roi += Point(K_BEASON_NW_X, K_BEASON_NW_Y);
      roi += Size(K_BEASON_NW_WIDTH, K_BEASON_NW_HEIGHT);
      break;
    case (K_COOKSIE_NW_ID):
      roi += Point(K_COOKSIE_NW_X, K_COOKSIE_NW_Y);
      roi += Size(K_COOKSIE_NW_WIDTH, K_COOKSIE_NW_HEIGHT);
      break;
    case (K_COOKSIE_SW_ID):
      roi += Point(K_COOKSIE_SW_X, K_COOKSIE_SW_Y);
      roi += Size(K_COOKSIE_SW_WIDTH, K_COOKSIE_SW_HEIGHT);
      break;
  }
  Mat crop = original(roi);
  return crop;
}

Window GetStartWindow(int regionId)
{
  Window win;
  Corner tl;
  switch(regionId){
    case (K_BEASON_NE_ID):
      tl.x = K_BEASON_NE_WIN_START_TP_X;
      tl.y = K_BEASON_NE_WIN_START_TP_Y;
      win = CreateWindow( tl,
                          K_BEASON_NE_WIN_START_WIDTH,
                          K_BEASON_NE_WIN_START_HEIGHT,
                          K_BEASON_NE_WIN_START_THETA);
      break;
    case (K_BEASON_SE_ID):
      tl.x = K_BEASON_SE_WIN_START_TP_X;
      tl.y = K_BEASON_SE_WIN_START_TP_Y;
      win = CreateWindow( tl,
                          K_BEASON_SE_WIN_START_WIDTH,
                          K_BEASON_SE_WIN_START_HEIGHT,
                          K_BEASON_SE_WIN_START_THETA);
      break;
    case (K_BEASON_SW_ID):
      tl.x = K_BEASON_SW_WIN_START_TP_X;
      tl.y = K_BEASON_SW_WIN_START_TP_Y;
      win = CreateWindow( tl,
                          K_BEASON_SW_WIN_START_WIDTH,
                          K_BEASON_SW_WIN_START_HEIGHT,
                          K_BEASON_SW_WIN_START_THETA);
      break;
    case (K_BEASON_NW_ID):
      tl.x = K_BEASON_NW_WIN_START_TP_X;
      tl.y = K_BEASON_NW_WIN_START_TP_Y;
      win = CreateWindow( tl,
                          K_BEASON_NW_WIN_START_WIDTH,
                          K_BEASON_NW_WIN_START_HEIGHT,
                          K_BEASON_NW_WIN_START_THETA);
      break;
    case (K_COOKSIE_NW_ID):
      tl.x = K_COOKSIE_NW_WIN_START_TP_X;
      tl.y = K_COOKSIE_NW_WIN_START_TP_Y;
      win = CreateWindow( tl,
                          K_COOKSIE_NW_WIN_START_WIDTH,
                          K_COOKSIE_NW_WIN_START_HEIGHT,
                          K_COOKSIE_NW_WIN_START_THETA);
      break;
    case (K_COOKSIE_SW_ID):
      tl.x = K_COOKSIE_SW_WIN_START_TP_X;
      tl.y = K_COOKSIE_SW_WIN_START_TP_Y;
      win = CreateWindow( tl,
                          K_COOKSIE_SW_WIN_START_WIDTH,
                          K_COOKSIE_SW_WIN_START_HEIGHT,
                          K_COOKSIE_SW_WIN_START_THETA);
      break;
    }
  return win;
}

Window GetEndWindow(int regionId)
{
  Window win;
  Corner tl;
  switch(regionId){
    case (K_BEASON_NE_ID):
      tl.x = K_BEASON_NE_WIN_END_TP_X;
      tl.y = K_BEASON_NE_WIN_END_TP_Y;
      win = CreateWindow( tl,
                          K_BEASON_NE_WIN_END_WIDTH,
                          K_BEASON_NE_WIN_END_HEIGHT,
                          K_BEASON_NE_WIN_END_THETA);

      break;
    case (K_BEASON_SE_ID):
      tl.x = K_BEASON_SE_WIN_END_TP_X;
      tl.y = K_BEASON_SE_WIN_END_TP_Y;
      win = CreateWindow( tl,
                          K_BEASON_SE_WIN_END_WIDTH,
                          K_BEASON_SE_WIN_END_HEIGHT,
                          K_BEASON_SE_WIN_END_THETA);
      break;
    case (K_BEASON_SW_ID):
      tl.x = K_BEASON_SW_WIN_END_TP_X;
      tl.y = K_BEASON_SW_WIN_END_TP_Y;
      win = CreateWindow( tl,
                          K_BEASON_SW_WIN_END_WIDTH,
                          K_BEASON_SW_WIN_END_HEIGHT,
                          K_BEASON_SW_WIN_END_THETA);
      break;
    case (K_BEASON_NW_ID):
      tl.x = K_BEASON_NW_WIN_END_TP_X;
      tl.y = K_BEASON_NW_WIN_END_TP_Y;
      win = CreateWindow( tl,
                          K_BEASON_NW_WIN_END_WIDTH,
                          K_BEASON_NW_WIN_END_HEIGHT,
                          K_BEASON_NW_WIN_END_THETA);
      break;
    case (K_COOKSIE_NW_ID):
      tl.x = K_COOKSIE_NW_WIN_END_TP_X;
      tl.y = K_COOKSIE_NW_WIN_END_TP_Y;
      win = CreateWindow( tl,
                          K_COOKSIE_NW_WIN_END_WIDTH,
                          K_COOKSIE_NW_WIN_END_HEIGHT,
                          K_COOKSIE_NW_WIN_END_THETA);
      break;
    case (K_COOKSIE_SW_ID):
      tl.x = K_COOKSIE_SW_WIN_END_TP_X;
      tl.y = K_COOKSIE_SW_WIN_END_TP_Y;
      win = CreateWindow( tl,
                          K_COOKSIE_SW_WIN_END_WIDTH,
                          K_COOKSIE_SW_WIN_END_HEIGHT,
                          K_COOKSIE_SW_WIN_END_THETA);
      break;
    }
  return win;
}

float GetThresholdFromRegionId(int regionId)
{
  float threshold = 0.0;
  switch(regionId){
    case (K_BEASON_NE_ID):
      threshold = K_BEASON_NE_SUMS_THRESHOLD;
      break;
    case (K_BEASON_SE_ID):
      threshold = K_BEASON_SE_SUMS_THRESHOLD;
      break;
    case (K_BEASON_SW_ID):
      threshold = K_BEASON_SW_SUMS_THRESHOLD;
      break;
    case (K_BEASON_NW_ID):
      threshold = K_BEASON_NW_SUMS_THRESHOLD;
      break;
    case (K_COOKSIE_NW_ID):
      threshold = K_COOKSIE_NW_SUMS_THRESHOLD;
      break;
    case (K_COOKSIE_SW_ID):
      threshold = K_COOKSIE_SW_SUMS_THRESHOLD;
      break;
  }
  return threshold;
}

float GetDistance(ImgPoint a, ImgPoint b)
{
  float xDiff = (float)a.x - (float)b.x;
  float yDiff = (float)a.y - (float)b.y;
  //printf("xDiff %f, yDiff %f\n", xDiff,yDiff);
  return sqrt(pow(xDiff, 2) + pow(yDiff, 2));
}

Window CreateWindow(Corner topLeft, int width, int height, float theta)
{
  Window win;

  win.width = width;
  win.height = height;
  win.theta = theta;

  // top left
  win.tl.x = topLeft.x;
  win.tl.y = topLeft.y;

  // top right
  win.tr.x = topLeft.x + (int)(width * cos(Degrees2Radians(theta)));
  win.tr.y = topLeft.y + (int)(width * sin(Degrees2Radians(theta)));

  // bottom left
  win.bl.x = topLeft.x + (int)(height * sin(Degrees2Radians(theta)));
  win.bl.y = topLeft.y + (int)(height * cos(Degrees2Radians(theta)));

  // bottom right
  win.br.x = topLeft.x + (int)(height * sin(Degrees2Radians(theta))) + (int)(width * cos(Degrees2Radians(theta)));
  win.br.y = topLeft.y + (int)(height * cos(Degrees2Radians(theta))) + (int)(width * sin(Degrees2Radians(theta)));

  // printf("CreateWindow %d %d   %d %d   %d %d   %d %d\n",
  //         win.tl.x, win.tl.y,
  //         win.tr.x, win.tr.y,
  //         win.bl.x, win.bl.y,
  //         win.br.x, win.br.y);
  // printf("%d %d   %d   %d   %f\n",
  //         topLeft.x, topLeft.y,
  //         width, height, theta);

  return win;
}

double Degrees2Radians(double deg)
{
  return deg * M_PI / 180.0;
}
////////////////////////////////////////////////////////////////////////////////


// Database ////////////////////////////////////////////////////////////////////
vector<OPEN_SPOT_T> FormatSpacesForDB(vector<Opening> spaces, int region, int * spot_id)
{
  vector<OPEN_SPOT_T> spaces_db;
  OPEN_SPOT_T spot;
  int i;
  Corner corners[2] = {0};

  for (i = 0; i < spaces.size(); i++)
  {
    spot.spot_id  = (*spot_id)++; // global counter
    spot.region   = GetXPositionOfSpot(region, spaces.at(i).start); // really x percentage represented by an integer (50 means 50%)
    spot.distance = GetYPositionOfSpot(region, spaces.at(i).start);; // really y percentage represented by an integer (50 means 50%)
    
    GetCornersOfSpot(corners, region, spaces.at(i).start);
    spot.corner0  = corners[0].x; // top left x
    spot.corner1  = corners[0].y; // top left y
    spot.corner2  = corners[1].x; // bottom right x
    spot.corner3  = corners[1].y; // bottom right y

    spaces_db.push_back(spot);
  }

  return spaces_db;
}

// in development
int GetXPositionOfSpot(int regionId, int start)
{
  int x = 0;

  switch(regionId){
  case (K_BEASON_NE_ID):
    x = start;
    break;
  case (K_BEASON_SE_ID):
    x = start;
    break;
  case (K_BEASON_SW_ID):
    x = start;
    break;
  case (K_BEASON_NW_ID):
    x = start;
    break;
  case (K_COOKSIE_NW_ID):
    x = start;
    break;
  case (K_COOKSIE_SW_ID):
    x = start;
    break;
  }
  return x;
}

// in development
int GetYPositionOfSpot(int regionId, int start)
{
  int y = 0;

  switch(regionId){
  case (K_BEASON_NE_ID):
    y = start;
    break;
  case (K_BEASON_SE_ID):
    y = start;
    break;
  case (K_BEASON_SW_ID):
    y = start;
    break;
  case (K_BEASON_NW_ID):
    y = start;
    break;
  case (K_COOKSIE_NW_ID):
    y = start;
    break;
  case (K_COOKSIE_SW_ID):
    y = start;
    break;
  }
  return y;
}

// in development
void GetCornersOfSpot(Corner * corners, int regionId, int start)
{
  Corner tl = {0}; // top left
  Corner br = {0}; // bottom right

  switch(regionId){
    case (K_BEASON_NE_ID):

      break;
    case (K_BEASON_SE_ID):

      break;
    case (K_BEASON_SW_ID):

      break;
    case (K_BEASON_NW_ID):

      break;
    case (K_COOKSIE_NW_ID):

      break;
    case (K_COOKSIE_SW_ID):

      break;
  }

  corners[0] = tl;
  corners[1] = br;
}

#ifdef __arm__
void InsertOpenParking(vector<OPEN_SPOT_T> spaces_db, MYSQL * conn)
{
  int i;
  char query[120];

  // Lock table
  WaitForLock(conn, K_TBL_OPEN_PARKING);

  // Clear table
  ClearTable(conn, K_TBL_OPEN_PARKING);

  // Make insertions
  for(i = 0; i < spaces_db.size(); i++)
  {
    // from db_utils
    FormatInsertForOpenParking(query, K_TBL_OPEN_PARKING,
      spaces_db.at(i).spot_id,
      spaces_db.at(i).region,   // x
      spaces_db.at(i).distance, // y
      spaces_db.at(i).corner0,
      spaces_db.at(i).corner1,
      spaces_db.at(i).corner2,
      spaces_db.at(i).corner3
    );
    InsertEntry(conn, query);
  }

  // Unlock table
  UnlockTable(conn, K_TBL_OPEN_PARKING);
}
#endif
////////////////////////////////////////////////////////////////////////////////


// Output ///////////////////////////////////////////////////////////////////
void WriteSlidingWindow(char * fn, char * imgfn, vector<int> sums)
{
    int i = 0;

    // Write header
    ofstream myfile;
    myfile.open(fn);
    myfile << imgfn << "\n"; // image name

    for(i = 0; i < sums.size(); i++)
    {
      myfile << sums.at(i) << endl;
    }
    myfile.close();
}

void WriteSlidingWindowFloat(char * fn, char * imgfn, vector<float> sums)
{
    int i = 0;

    // Write header
    ofstream myfile;
    myfile.open(fn);
    myfile << imgfn << "\n"; // image name

    for(i = 0; i < sums.size(); i++)
    {
      myfile << sums.at(i) << endl;
    }
    myfile.close();
}

void WriteOpenings(char * fn, char * imgfn, vector<Opening> openings)
{
    vector<int> sums;
    int i;
    int length = openings.size();

    // Write header
    ofstream myfile;
    myfile.open(fn);
    myfile << imgfn << "\n"; // image name

    for(i = 0; i < length; i++)
    {
      myfile << openings.at(i).start << "," << openings.at(i).length << "\n";
    }
    myfile.close();
}

/////////////////////////////////////////////////////////////////////////////


// System ///////////////////////////////////////////////////////////////////
void TakeNewImage(char * fn, unsigned int num)
{
  char cmd[50];
  sprintf(fn, "img_%04u.jpg", num);
  sprintf(cmd, "fswebcam -r 1920x1080 %s -S 20", fn);
  cout << cmd << endl;
  //system(cmd);
}
