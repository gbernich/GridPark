#include <stdio.h>
#include <cv.hpp>
#include <time.h>
#include <iostream>

#include "ip.h"

using namespace cv;
using namespace std;


#define source_window   "Source image"
#define corners_window  "Corners detected"
#define window_name     "Edge Map"

void cornerHarris_demo( int, void* );
void CannyThreshold(int, void*);

/// Global variables
Mat src, src_gray;
Mat dst, detected_edges;

// Corner Detection
int thresh = IP_CORNERS_THRESH_INIT;

// Edge Detection
int edgeThresh = 2;
int lowThreshold = 100;
int max_lowThreshold = 255;
int ratio1 = 3;
int kernel_size = 3;

int main(int argc, char** argv )
{  
  struct timespec start, finish;
  double elapsed;
  int i;

  // Corner Detection
  int thresh = IP_CORNERS_THRESH_INIT;
  //int max_thresh = 255;
  vector<Corner> corners;

  // Check for number of arguments
  if ( argc != 2 )
  {
      printf("usage: DisplayImage.out <Image_Path>\n");
      return -1;
  }

  /// Load source image and convert it to gray
  src = imread( argv[1], 1 );
  cvtColor( src, src_gray, CV_BGR2GRAY );

  // Create a window and a trackbar
  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  //createTrackbar( "Threshold: ", source_window, &thresh, max_thresh, cornerHarris_demo );
  createTrackbar( "Threshold:", source_window, &lowThreshold, max_lowThreshold, CannyThreshold );
  imshow( source_window, src );

  // for (i = 0; i < 4; i++)
  // {
  //   clock_gettime(CLOCK_MONOTONIC, &start);
  //   corners = GetOptimalCorners(src_gray, &thresh);
  //   clock_gettime(CLOCK_MONOTONIC, &finish);
  //   elapsed = (finish.tv_sec - start.tv_sec);
  //   elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  //   cout << elapsed * 1000.0 << " ms" << endl;
  // }




  CannyThreshold(0, 0);

  waitKey(0);
  return 0;
}



 // command for image
 // fswebcam -r 1920x1080 image.jpg -S 20


void cornerHarris_demo( int, void* )
{

  Mat dst, dst_norm, dst_norm_scaled;
  dst = Mat::zeros( src.size(), CV_32FC1 );
  int count = 0;

  // Detector parameters
  // int blockSize = 2;
  // int apertureSize = 5;
  // double k = 0.1005;
  int blockSize = 2;
  int apertureSize = 3;
  double k = 0.05;

  /// Detecting corners
  cornerHarris( src_gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT );

  /// Normalizing
  normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
  convertScaleAbs( dst_norm, dst_norm_scaled );

  /// Drawing a circle around corners
  for( int j = 0; j < dst_norm.rows ; j++ )
     { for( int i = 0; i < dst_norm.cols; i++ )
          {
            if( (int) dst_norm.at<float>(j,i) > thresh )
              {
                circle( dst_norm_scaled, Point( i, j ), 5,  Scalar(0), 2, 8, 0 );
                count++;
              }
          }
     }

  /// Showing the result
  printf("Thresh = %d, corners = %d\n", thresh, count);
  //namedWindow( corners_window, CV_WINDOW_AUTOSIZE );
  //imshow( corners_window, dst_norm_scaled );
}

void CannyThreshold(int, void*)
{
  /// Reduce noise with a kernel 3x3
  blur( src_gray, detected_edges, Size(3,3) );

  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio1, kernel_size );

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);

  src.copyTo( dst, detected_edges);
  imshow( window_name, detected_edges );
 }

