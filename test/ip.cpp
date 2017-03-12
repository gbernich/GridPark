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
  if (edge == K_LEFT_EDGE)
    startPos.x = win.tl.x;
  else
    startPos.x = win.br.x; // ONLY WORKS WHEN THETA = 0.0
  startPos.y = win.tl.y; // ONLY WORKS WHEN THETA = 0.0
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
      if (IsOnStartingEdge(endPos, win, edge))
      {
        //cout << "start " << currPos.y << " end " << endPos.y << endl;
        if ((float)(endPos.y - currPos.y) >= minDiff * (float)(win.br.y - win.tl.y))
        {
          count++;
          //cout << "counted" << endl;
        }
      }
    }

    // Move along the window edge
    currPos = GetNextStartingPoint(currPos, win);
  }
  //cout << count << endl;
  return count;
}

bool IsOnStartingEdge(ImgPoint pos, Window win, int edge)
{
  if (edge == K_LEFT_EDGE){
    if (pos.x == win.tl.x) // ONLY WORKS WHEN THETA = 0.0
      return true;
  }else{
    if (pos.x == win.br.x) // ONLY WORKS WHEN THETA = 0.0
      return true;
  }

  return false;
}

ImgPoint GetNextStartingPoint(ImgPoint currPos, Window win)
{
  ImgPoint nextPos;
  nextPos.x = currPos.x;
  nextPos.y = currPos.y + 1;

  if (nextPos.y > win.br.y)
  {
    nextPos.x = -1;
    nextPos.y = -1;
  }
  return nextPos;
}

ImgPoint TraverseEdge(Window win, Mat edges, ImgPoint currPos, int edge)
{
  ImgPoint lastPos = currPos;
  ImgPoint nextPos = GetNextPos(win, edges, currPos, lastPos);
  //cout << nextPos.x << " " << nextPos.y << endl;
  //printf("4\n");
  while (nextPos.x != -1)
  {
    //printf("5\n");
    lastPos = currPos;
    currPos = nextPos;

    if ((currPos.x == win.tl.x && edge == K_LEFT_EDGE) ||
        (currPos.x == win.br.x && edge == K_RIGHT_EDGE))
      break;
    nextPos = GetNextPos(win, edges, currPos, lastPos);

    //cout << nextPos.x << " " << nextPos.y << endl;
  }

  return currPos;
}

ImgPoint GetNextPos(Window win, Mat edges, ImgPoint currPos, ImgPoint lastPos)
{
  ImgPoint nextPos, tryPos;
  int attemptedPos[3][3] = {{1, 1, 1}, {0, 1, 0}, {0, 0, 0}};
  int xDiff, yDiff;

  // Mark Last position
  xDiff = (lastPos.x - currPos.x) + 1;
  yDiff = (lastPos.y - currPos.y) + 1;
  attemptedPos[yDiff][xDiff] = 1;

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
  //printf("7\n");
  return nextPos;

}

bool IsInsideWindow(Window win, ImgPoint pos)
{
  if (win.tl.x <= pos.x && pos.x <= win.br.x &&
      win.tl.y <= pos.y && pos.y <= win.br.y)
    return true;
  else
    return false; 
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
    sum = 0;

    // Horizontal
    for (x = win.tl.x; x < win.br.x; x++)
    {
      // Vertical
      for (y = win.tl.y; y < win.br.y; y++)
      {
        pixel = (int)img.at<float>(y, x);
        if (pixel > thresh)
        {
          sum += 1;
        }
      }
    }
    sums.push_back(sum);
  }
  return sums;
}

vector<int> GetSlidingEdges(Mat edges, Window startWindow, Window endWindow, float minDiff, int edge)
{
  vector<Window> windows;
  Window win;
  vector<int> sums;
  int sum, i, x, y = 0;
  int pixel;

  windows = GetSlidingWindow(startWindow, endWindow, edges.rows, edges.cols);

  // Sum
  for (i = 0; i < windows.size(); i++)
  {
    win = windows.at(i);
    sum = CountMeaningfulEdges(win, edges, minDiff, edge);    
    sums.push_back(sum);
  }
  return sums;
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
    int stepCount = endWindow.br.x - startWindow.tl.x;  // ONLY WORKS WHEN THETA = 0.0
    vector<Window> windows;
    Window currWindow = startWindow;
    Window diffWindow;
    float dx0, dx1, dy0, dy1;
    float tlx, tly, brx, bry;
    
    // Initial values (as floats)
    tlx = (float)startWindow.tl.x;
    tly = (float)startWindow.tl.y;
    brx = (float)startWindow.br.x;
    bry = (float)startWindow.br.y;

    // Calculate the step size between each window
    dx0 = float(endWindow.tl.x - startWindow.tl.x) / (float)stepCount;
    dy0 = float(endWindow.tl.y - startWindow.tl.y) / (float)stepCount;
    dx1 = float(endWindow.br.x - startWindow.br.x) / (float)stepCount;
    dy1 = float(endWindow.br.y - startWindow.br.y) / (float)stepCount;

    while (IsWithinBounds(imgHeight, imgWidth, currWindow) && stepCount > -1)
    {
        // add window
        windows.push_back(currWindow);

        // update window
        tlx += dx0;
        tly += dy0;
        brx += dx1;
        bry += dy1;
        //printf("%d %d, %d %d\n", (int)tlx, (int)tly, (int)brx, (int)bry);
        
        // convert back to integers to save window
        currWindow.tl.x = (int)tlx; currWindow.tl.y = (int)tly;
        currWindow.br.x = (int)brx; currWindow.br.y = (int)bry;

        stepCount--;
    }
    //printf("length %lu\n", windows.size());
    return windows;
}

bool IsWithinBounds(int imgHeight, int imgWidth, Window win)
{
  // ONLY WORKS WHEN THETA = 0.0
  if (win.tl.x > imgWidth-1) { return false; }
  if (win.br.x > imgWidth-1) { return false; }
  if (win.tl.y > imgHeight-1){ return false; }
  if (win.br.y > imgHeight-1){ return false; }

  return true;
}

Mat GetSubRegionImage(Mat original, int regionId)
{
  Rect roi(0,0,0,0);
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
  switch(regionId){
    case (K_BEASON_NE_ID):
      win.tl.x  = K_BEASON_NE_WIN_START_TP_X;
      win.tl.y  = K_BEASON_NE_WIN_START_TP_Y; 
      win.br.x  = K_BEASON_NE_WIN_START_TP_X + K_BEASON_NE_WIN_START_WIDTH;
      win.br.y  = K_BEASON_NE_WIN_START_TP_Y + K_BEASON_NE_WIN_START_HEIGHT;
      win.theta = K_BEASON_NE_WIN_START_THETA;
      break;
    case (K_BEASON_SE_ID):
      win.tl.x  = K_BEASON_SE_WIN_START_TP_X;
      win.tl.y  = K_BEASON_SE_WIN_START_TP_Y; 
      win.br.x  = K_BEASON_SE_WIN_START_TP_X + K_BEASON_SE_WIN_START_WIDTH;
      win.br.y  = K_BEASON_SE_WIN_START_TP_Y + K_BEASON_SE_WIN_START_HEIGHT;
      win.theta = K_BEASON_SE_WIN_START_THETA;
      break;
    case (K_BEASON_SW_ID):
      win.tl.x  = K_BEASON_SW_WIN_START_TP_X;
      win.tl.y  = K_BEASON_SW_WIN_START_TP_Y; 
      win.br.x  = K_BEASON_SW_WIN_START_TP_X + K_BEASON_SW_WIN_START_WIDTH;
      win.br.y  = K_BEASON_SW_WIN_START_TP_Y + K_BEASON_SW_WIN_START_HEIGHT;
      win.theta = K_BEASON_SW_WIN_START_THETA;
      break;
    case (K_BEASON_NW_ID):
      win.tl.x  = K_BEASON_NW_WIN_START_TP_X;
      win.tl.y  = K_BEASON_NW_WIN_START_TP_Y; 
      win.br.x  = K_BEASON_NW_WIN_START_TP_X + K_BEASON_NW_WIN_START_WIDTH;
      win.br.y  = K_BEASON_NW_WIN_START_TP_Y + K_BEASON_NW_WIN_START_HEIGHT;
      win.theta = K_BEASON_NW_WIN_START_THETA;
      break;
    case (K_COOKSIE_NW_ID):
      win.tl.x  = K_COOKSIE_NW_WIN_START_TP_X;
      win.tl.y  = K_COOKSIE_NW_WIN_START_TP_Y; 
      win.br.x  = K_COOKSIE_NW_WIN_START_TP_X + K_COOKSIE_NW_WIN_START_WIDTH;
      win.br.y  = K_COOKSIE_NW_WIN_START_TP_Y + K_COOKSIE_NW_WIN_START_HEIGHT;
      win.theta = K_COOKSIE_NW_WIN_START_THETA;
      break;
    case (K_COOKSIE_SW_ID):
      win.tl.x  = K_COOKSIE_SW_WIN_START_TP_X;
      win.tl.y  = K_COOKSIE_SW_WIN_START_TP_Y; 
      win.br.x  = K_COOKSIE_SW_WIN_START_TP_X + K_COOKSIE_SW_WIN_START_WIDTH;
      win.br.y  = K_COOKSIE_SW_WIN_START_TP_Y + K_COOKSIE_SW_WIN_START_HEIGHT;
      win.theta = K_COOKSIE_SW_WIN_START_THETA;
      break;
    }
  return win;
}

Window GetEndWindow(int regionId)
{
  Window win;
  switch(regionId){
    case (K_BEASON_NE_ID):
      win.tl.x  = K_BEASON_NE_WIN_END_TP_X;
      win.tl.y  = K_BEASON_NE_WIN_END_TP_Y;
      win.br.x  = K_BEASON_NE_WIN_END_TP_X + K_BEASON_NE_WIN_END_WIDTH;
      win.br.y  = K_BEASON_NE_WIN_END_TP_Y + K_BEASON_NE_WIN_END_HEIGHT;
      win.theta = K_BEASON_NE_WIN_END_THETA;
      break;
    case (K_BEASON_SE_ID):
      win.tl.x  = K_BEASON_SE_WIN_END_TP_X;
      win.tl.y  = K_BEASON_SE_WIN_END_TP_Y; 
      win.br.x  = K_BEASON_SE_WIN_END_TP_X + K_BEASON_SE_WIN_END_WIDTH;
      win.br.y  = K_BEASON_SE_WIN_END_TP_Y + K_BEASON_SE_WIN_END_HEIGHT;
      win.theta = K_BEASON_SE_WIN_END_THETA;
      break;
    case (K_BEASON_SW_ID):
      win.tl.x  = K_BEASON_SW_WIN_END_TP_X;
      win.tl.y  = K_BEASON_SW_WIN_END_TP_Y; 
      win.br.x  = K_BEASON_SW_WIN_END_TP_X + K_BEASON_SW_WIN_END_WIDTH;
      win.br.y  = K_BEASON_SW_WIN_END_TP_Y + K_BEASON_SW_WIN_END_HEIGHT;
      win.theta = K_BEASON_SW_WIN_END_THETA;
      break;
    case (K_BEASON_NW_ID):
      win.tl.x  = K_BEASON_NW_WIN_END_TP_X;
      win.tl.y  = K_BEASON_NW_WIN_END_TP_Y; 
      win.br.x  = K_BEASON_NW_WIN_END_TP_X + K_BEASON_NW_WIN_END_WIDTH;
      win.br.y  = K_BEASON_NW_WIN_END_TP_Y + K_BEASON_NW_WIN_END_HEIGHT;
      win.theta = K_BEASON_NW_WIN_END_THETA;
      break;
    case (K_COOKSIE_NW_ID):
      win.tl.x  = K_COOKSIE_NW_WIN_END_TP_X;
      win.tl.y  = K_COOKSIE_NW_WIN_END_TP_Y; 
      win.br.x  = K_COOKSIE_NW_WIN_END_TP_X + K_COOKSIE_NW_WIN_END_WIDTH;
      win.br.y  = K_COOKSIE_NW_WIN_END_TP_Y + K_COOKSIE_NW_WIN_END_HEIGHT;
      win.theta = K_COOKSIE_NW_WIN_END_THETA;
      break;
    case (K_COOKSIE_SW_ID):
      win.tl.x  = K_COOKSIE_SW_WIN_END_TP_X;
      win.tl.y  = K_COOKSIE_SW_WIN_END_TP_Y; 
      win.br.x  = K_COOKSIE_SW_WIN_END_TP_X + K_COOKSIE_SW_WIN_END_WIDTH;
      win.br.y  = K_COOKSIE_SW_WIN_END_TP_Y + K_COOKSIE_SW_WIN_END_HEIGHT;
      win.theta = K_COOKSIE_SW_WIN_END_THETA;
      break;
    }
  return win;
}
////////////////////////////////////////////////////////////////////////////////


// Output ///////////////////////////////////////////////////////////////////
void WriteSlidingWindow(char * fn, char * imgfn, vector<int> sums, int windowSizes)
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
