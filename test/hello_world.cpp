#include <stdio.h>
#include <cv.hpp>

using namespace cv;
using namespace std;

/// Global variables
Mat src, src_gray;
int thresh = 200;
int max_thresh = 255;

Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int max_lowThreshold = 250;
int ratio1 = 3;
int kernel_size = 3;


#define source_window   "Source image"
#define corners_window  "Corners detected"
#define window_name     "Edge Map"


void cornerHarris_demo( int, void* );
void CannyThreshold(int, void*);

int main(int argc, char** argv )
{   
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
/*
    Mat img;
    img = imread( argv[1], 1 );

    if ( !img.data )
    {
        printf("No image data \n");
        return -1;
    }

    // create a window
    cvNamedWindow("mainWin"); 
    cvMoveWindow("mainWin", 100, 100);
    cvResizeWindow("mainWin", 800, 600);

    namedWindow("Display Image");
    imshow("Display Image", img);

    waitKey(0);*/

    /// Load source image and convert it to gray
    src = imread( argv[1], 1 );
    cvtColor( src, src_gray, CV_BGR2GRAY );

    /// Create a window and a trackbar
    namedWindow( source_window, CV_WINDOW_AUTOSIZE );
    createTrackbar( "Threshold: ", source_window, &thresh, max_thresh, cornerHarris_demo );
    //createTrackbar( "Threshold:", source_window, &lowThreshold, max_lowThreshold, CannyThreshold );
    imshow( source_window, src );

    cornerHarris_demo(0, 0);
    //CannyThreshold(0, 0);

    waitKey(0);

    return 0;
}


void cornerHarris_demo( int, void* )
{

  Mat dst, dst_norm, dst_norm_scaled;
  dst = Mat::zeros( src.size(), CV_32FC1 );
  int count = 0;

  // Detector parameters
  int blockSize = 2;
  int apertureSize = 5;
  double k = 0.1005;
  // int blockSize = 2;
  // int apertureSize = 3;
  // double k = 0.04;

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
  printf("Found %d corners\n", count);
  namedWindow( corners_window, CV_WINDOW_AUTOSIZE );
  imshow( corners_window, dst_norm_scaled );
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
  imshow( window_name, dst );
 }
