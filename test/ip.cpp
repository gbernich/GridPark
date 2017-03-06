//-----------------------------------------------------------------------------
// File:        ip.c
// Author:      Garrett Bernichon
// Function:    Provide functions to do the low level image processing.
//-----------------------------------------------------------------------------
#include "ip.h"

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

////////////////////////////////////////////////////////////////////////////////


// Utilities ///////////////////////////////////////////////////////////////////
vector<int> GetSlidingSum(Mat img, int count, int thresh, Window startWindow, Window endWindow)
{
  vector<Window> windows;
  Window win;
  vector<int> sums;
  int sum, i, x, y = 0;
  int pixel;

  windows = GetSlidingWindow(count, startWindow, endWindow, img.rows, img.cols);

  // Sum
  for (i = 0; i < windows.size(); i++)
  {
    win = windows.at(i);
    sum = 0;

    // Horizontal
    for (x = win.c0.x; x < win.c1.x; x++)
    {
      // Vertical
      for (y = win.c0.y; y < win.c2.y; y++)
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

vector<Window> GetSlidingWindow(int stepCount, Window startWindow, Window endWindow, int imgHeight, int imgWidth)
{
    vector<Window> windows;
    Window currWindow = startWindow;
    Window diffWindow;
    float dx0, dx1, dx2, dx3;
    float dy0, dy1, dy2, dy3;
    float cx0, cx1, cx2, cx3;
    float cy0, cy1, cy2, cy3;
    
    // Initial values (as floats)
    cx0 = (float)startWindow.c0.x;
    cy0 = (float)startWindow.c0.y;
    cx1 = (float)startWindow.c1.x;
    cy1 = (float)startWindow.c1.y;
    cx2 = (float)startWindow.c2.x;
    cy2 = (float)startWindow.c2.y;
    cx3 = (float)startWindow.c3.x;
    cy3 = (float)startWindow.c3.y;

    // Calculate the step size between each window
    dx0 = float(endWindow.c0.x - startWindow.c0.x) / (float)stepCount;
    dy0 = float(endWindow.c0.y - startWindow.c0.y) / (float)stepCount;
    dx1 = float(endWindow.c1.x - startWindow.c1.x) / (float)stepCount;
    dy1 = float(endWindow.c1.y - startWindow.c1.y) / (float)stepCount;
    dx2 = float(endWindow.c2.x - startWindow.c2.x) / (float)stepCount;
    dy2 = float(endWindow.c2.y - startWindow.c2.y) / (float)stepCount;
    dx3 = float(endWindow.c3.x - startWindow.c3.x) / (float)stepCount;
    dy3 = float(endWindow.c3.y - startWindow.c3.y) / (float)stepCount;

    while (IsWithinBounds(imgHeight, imgWidth, currWindow) && stepCount > -1)
    {
        // add window
        windows.push_back(currWindow);

        // update window
        cx0 += dx0;
        cy0 += dy0;
        cx1 += dx1;
        cy1 += dy1;
        cx2 += dx2;
        cy2 += dy2;
        cx3 += dx3;
        cy3 += dy3;

        // convert back to integers to save window
        currWindow.c0.x = (int)cx0; currWindow.c0.y = (int)cy0;
        currWindow.c1.x = (int)cx1; currWindow.c1.y = (int)cy1;
        currWindow.c2.x = (int)cx2; currWindow.c2.y = (int)cy2;
        currWindow.c3.x = (int)cx3; currWindow.c3.y = (int)cy3;

        stepCount--;
    }
    return windows;
}

bool IsWithinBounds(int imgHeight, int imgWidth, Window win)
{
    if (win.c0.x > imgWidth) { return false; }
    if (win.c1.x > imgWidth) { return false; }
    if (win.c2.x > imgWidth) { return false; }
    if (win.c3.x > imgWidth) { return false; }
    if (win.c0.y > imgHeight){ return false; }
    if (win.c1.y > imgHeight){ return false; }
    if (win.c2.y > imgHeight){ return false; }
    if (win.c3.y > imgHeight){ return false; }

    return true;
}
