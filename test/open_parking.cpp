#include <stdio.h>
#include <time.h>
#include <iostream>
#include "ip.h"

using namespace std;

#define source_window   "Source image"
#define corners_window  "Corners detected"
#define window_name     "Edge Map"


/// Global variables


Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int max_lowThreshold = 250;
int ratio1 = 3;
int kernel_size = 3;

int main(int argc, char** argv )
{  
  struct timespec start, finish;
  double elapsed;
  int i, count, x, y, x0, y0, x3, y3;

  // Matrices
  Mat src, src_gray, overlay;

  // Corner Detection
  int thresh = IP_CORNERS_THRESH_INIT;
  //int max_thresh = 255;
  Mat corners;

  // Edge Detection
  Mat edges;

  //Sliding Window
  Window startWindow, endWindow;
  vector<int> sums;

  // Check for number of arguments
  if ( argc != 2 )
  {
      printf("usage: DisplayImage.out <Image_Path>\n");
      return -1;
  }

  /// Load source image and convert it to gray
  src = imread( argv[1], 1 );
  cvtColor( src, src_gray, CV_BGR2GRAY );

  // Overlay for slinding window
  overlay = src.clone();

  /// Create a window and a trackbar
  // namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  //createTrackbar( "Threshold: ", source_window, &thresh, max_thresh, cornerHarris_demo );
  //createTrackbar( "Threshold:", source_window, &lowThreshold, max_lowThreshold, CannyThreshold );
  // imshow( source_window, src );

  
  // for (thresh = 80; thresh < 90; thresh += 1)
  // {
  //   clock_gettime(CLOCK_MONOTONIC, &start);
  //   cornerHarris_demo(0, 0);
  //   clock_gettime(CLOCK_MONOTONIC, &finish);

  //   elapsed = (finish.tv_sec - start.tv_sec);
  //   elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  //   cout << elapsed * 1000.0 << " ms" << endl;
  // }

  // Sliding Sum
  // Sliding Window
  // beason5
  startWindow.c0.x = 0;                   startWindow.c0.y = 0;
  startWindow.c1.x = 0+IP_WINDOW_WIDTH;   startWindow.c1.y = 0;
  startWindow.c2.x = 0;                   startWindow.c2.y = 100;
  startWindow.c3.x = 0+IP_WINDOW_WIDTH;   startWindow.c3.y = 100;
  endWindow.c0.x = 704-IP_WINDOW_WIDTH;   endWindow.c0.y = 60;
  endWindow.c1.x = 704;                   endWindow.c1.y = 60;
  endWindow.c2.x = 704-IP_WINDOW_WIDTH;   endWindow.c2.y = 124;
  endWindow.c3.x = 704;                   endWindow.c3.y = 124;
  // beason4
  // startWindow.c0.x = 0;                   startWindow.c0.y = 100;
  // startWindow.c1.x = 0+IP_WINDOW_WIDTH;   startWindow.c1.y = 100;
  // startWindow.c2.x = 0;                   startWindow.c2.y = 252;
  // startWindow.c3.x = 0+IP_WINDOW_WIDTH;   startWindow.c3.y = 252;
  // endWindow.c0.x = 784-IP_WINDOW_WIDTH;   endWindow.c0.y = 50;
  // endWindow.c1.x = 784;                   endWindow.c1.y = 50;
  // endWindow.c2.x = 784-IP_WINDOW_WIDTH;   endWindow.c2.y = 175;
  // endWindow.c3.x = 784;                   endWindow.c3.y = 175;


  // Corner Detection
  for (i = 0; i < 2; i++)
  {
    clock_gettime(CLOCK_MONOTONIC, &start);
    corners = GetOptimalCorners(src_gray, &thresh);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Corner Detection: " << elapsed * 1000.0 << " ms" << endl;
  }

  sums = GetSlidingSum(corners, 100, thresh, startWindow, endWindow);

  for (i = 0; i < sums.size(); i++)
  {
    printf("%d\n", sums.at(i));
  }

  // if (1)
  // {
  //   for (i = 0; i < corners.size(); i++)
  //   {
  //     x = corners.at(i).x;
  //     y = corners.at(i).y;
  //     printf("%d,%d\n", x, y);
  //     circle( src_gray, Point( x, y ), 5,  Scalar(0), 2, 8, 0 );
  //   } 
  //   namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  //   imshow( source_window, src_gray );
  //   waitKey(0);
  // }

  // Edge Detection
  for (i = 0; i < 1; i++)
  {
    clock_gettime(CLOCK_MONOTONIC, &start);
    edges = GetEdges(src_gray, 100, 3, 3);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Edge Detection: " << elapsed * 1000.0 << " ms" << endl;
  }

  clock_gettime(CLOCK_MONOTONIC, &start);
  sums = GetSlidingSum(edges, 200, 0, startWindow, endWindow);
  clock_gettime(CLOCK_MONOTONIC, &finish);

  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  cout << "Sliding Sum: " << elapsed * 1000.0 << " ms" << endl;

  for (i = 0; i < sums.size(); i++)
  {
    printf("%d\n", sums.at(i));
  }

  // for (i = 0; i < windows.size(); i++)
  // {
  //   x0 = windows.at(i).c0.x; y0 = windows.at(i).c0.y;
  //   x3 = windows.at(i).c3.x; y3 = windows.at(i).c3.y;
  //   rectangle(overlay, Point(x0, y0), Point(x3, y3), Scalar(255, 0, 0), -1);  
  // } 
  // addWeighted(overlay, 0.05, src, 1.0 - 0.05, 0, src);

  // namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  // imshow( source_window, edges );
  // waitKey(0);

  //CannyThreshold(0, 0);

  
  return 0;
}



 // command for image
 // fswebcam -r 1920x1080 image.jpg -S 20
