#include <stdio.h>
#include <time.h>
#include <iostream>
#include "ip.h"

using namespace std;



//void thresh_callback(int, void* );
/// Global variables
RNG rng(12345);
//int thresh = 100;
//int max_thresh = 255;


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

  // Check for number of arguments
  if ( argc != 2 )
  {
      printf("usage: DisplayImage.out <Image_Path>\n");
      return -1;
  }

  /// Load source image
  src = imread( argv[1], 1 );

  // Set subregion
  regionId = 0;

  // Crop for subregion
  roi = GetSubRegionImage(src, regionId);

  // Convert to grayscale
  cvtColor( roi, roi_gray, CV_BGR2GRAY );

  // Get Edges
  thresh = 100;
  clock_gettime(CLOCK_MONOTONIC, &start);
  edges = GetEdges(roi_gray, thresh, 3, 3);
  clock_gettime(CLOCK_MONOTONIC, &finish);
  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  cout << "Edge Detection: " << elapsed * 1000.0 << " ms" << endl;

  // Get Sliding Window
  startWin    = GetStartWindow(regionId);
  endWin      = GetEndWindow(regionId);

  // Sum all edges
  sums = GetSlidingSum(edges, 0, startWin, endWin);
  cout << sums.size() << endl;
  sumsNorm = GetNormalizedSlidingSum(edges, 0, startWin, endWin);
  openings = GetOpeningsFromSumsNormalized(sumsNorm, regionId);
  spaces = GetOpenParkingSpaces(openings, regionId);


  // Count Meaningful Edges
  //sumsLeft = GetSlidingEdges(edges, startWin, endWin, 0.0, K_LEFT_EDGE);
  //sumsRight = GetSlidingEdges(edges, startWin, endWin, 0.0, K_RIGHT_EDGE);

  // Combine Meaningful Edges
  // openings = GetOpenings(sumsLeft, sumsRight);
  // openings = MergeParkingData(openings, sums);

  // Write output
  imwrite( "../../matlab/edges.jpg", edges );
  WriteSlidingWindowFloat("../../matlab/edges.txt", argv[1], sumsNorm);
  WriteSlidingWindow("../../matlab/edges_left.txt", argv[1], sumsLeft);
  WriteSlidingWindow("../../matlab/edges_right.txt", argv[1], sumsRight);
  WriteOpenings("../../matlab/openings.txt", argv[1], spaces);

  // Output image to screen
  // namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  // imshow( source_window, roi );
  // waitKey(0);

  return 0;
/*
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
  startWindow.c1.x = 0+IP_WINDOW_WIDTH; startWindow.c1.y = 0;
  startWindow.c2.x = 0;                   startWindow.c2.y = 100;
  startWindow.c3.x = 0+IP_WINDOW_WIDTH; startWindow.c3.y = 100;
  endWindow.c0.x = 704-IP_WINDOW_WIDTH; endWindow.c0.y = 60;
  endWindow.c1.x = 704;                   endWindow.c1.y = 60;
  endWindow.c2.x = 704-IP_WINDOW_WIDTH; endWindow.c2.y = 124;
  endWindow.c3.x = 704;                   endWindow.c3.y = 124;
  // beason4
  startWindow.c0.x = 0;                   startWindow.c0.y = 100;
  startWindow.c1.x = 0+IP_WINDOW_WIDTH; startWindow.c1.y = 100;
  startWindow.c2.x = 0;                   startWindow.c2.y = 252;
  startWindow.c3.x = 0+IP_WINDOW_WIDTH; startWindow.c3.y = 252;
  endWindow.c0.x = 784-IP_WINDOW_WIDTH; endWindow.c0.y = 50;
  endWindow.c1.x = 784;                   endWindow.c1.y = 50;
  endWindow.c2.x = 784-IP_WINDOW_WIDTH; endWindow.c2.y = 175;
  endWindow.c3.x = 784;                   endWindow.c3.y = 175;


//////////////// EDGES /////////////////////
  // Edge Detection
  for (i = 0; i < 1; i++)
  {
    clock_gettime(CLOCK_MONOTONIC, &start);
    edges = GetEdges(roi_gray, 70, 3, 3);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Edge Detection: " << elapsed * 1000.0 << " ms" << endl;
  }
  sumsVector.clear();
  windowSizes.clear();
  //windowSizes.push_back(5);
  windowSizes.push_back(5);
  //windowSizes.push_back(15);
  //windowSizes.push_back(20);
  //windowSizes.push_back(30);

  for (i = 0; i < windowSizes.size(); i++)
  {
    startWindow.c0.x = 0;                   startWindow.c0.y = 0;
    startWindow.c1.x = 0+windowSizes.at(i); startWindow.c1.y = 0;
    startWindow.c2.x = 0;                   startWindow.c2.y = 100;
    startWindow.c3.x = 0+windowSizes.at(i); startWindow.c3.y = 100;
    endWindow.c0.x = 704-windowSizes.at(i); endWindow.c0.y = 60;
    endWindow.c1.x = 704;                   endWindow.c1.y = 60;
    endWindow.c2.x = 704-windowSizes.at(i); endWindow.c2.y = 124;
    endWindow.c3.x = 704;                   endWindow.c3.y = 124;
  // startWindow.c0.x = 0;                   startWindow.c0.y = 100;
  // startWindow.c1.x = 0+windowSizes.at(i); startWindow.c1.y = 100;
  // startWindow.c2.x = 0;                   startWindow.c2.y = 252;
  // startWindow.c3.x = 0+windowSizes.at(i); startWindow.c3.y = 252;
  // endWindow.c0.x = 784-windowSizes.at(i); endWindow.c0.y = 50;
  // endWindow.c1.x = 784;                   endWindow.c1.y = 50;
  // endWindow.c2.x = 784-windowSizes.at(i); endWindow.c2.y = 175;
  // endWindow.c3.x = 784;                   endWindow.c3.y = 175;

  // startWindow.c0.x = 280;                   startWindow.c0.y = 100;
  // startWindow.c1.x = 280+windowSizes.at(i); startWindow.c1.y = 100;
  // startWindow.c2.x = 280;                   startWindow.c2.y = 252;
  // startWindow.c3.x = 0+windowSizes.at(i); startWindow.c3.y = 252;
  // endWindow.c0.x = 784-windowSizes.at(i); endWindow.c0.y = 50;
  // endWindow.c1.x = 784;                   endWindow.c1.y = 50;
  // endWindow.c2.x = 784-windowSizes.at(i); endWindow.c2.y = 175;
  // endWindow.c3.x = 784;                   endWindow.c3.y = 175;
    clock_gettime(CLOCK_MONOTONIC, &start);
    //sums = GetSlidingSum(edges, 0, startWindow, endWindow);
    sumsLeft = GetSlidingEdges(edges, startWindow, endWindow, 0.14, K_LEFT_EDGE);
    sumsRight = GetSlidingEdges(edges, startWindow, endWindow, 0.14, K_RIGHT_EDGE);
    openings = GetOpenings(sumsLeft, sumsRight);
    //sumsVector.push_back(sums);
    sumsVectorLeft.push_back(sumsLeft);
    sumsVectorRight.push_back(sumsRight);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Sliding Sum: " << elapsed * 1000.0 << " ms" << endl;
  }
  //WriteSlidingWindow("../../matlab/edges.txt", argv[1], sumsVector, windowSizes);
  imwrite( "../../matlab/edges.jpg", edges );
  WriteSlidingWindow("../../matlab/edges_left.txt", argv[1], sumsVectorLeft, windowSizes);
  WriteSlidingWindow("../../matlab/edges_right.txt", argv[1], sumsVectorRight, windowSizes);
  WriteOpenings("../../matlab/openings.txt", argv[1], openings);
  //////////////// EDGES /////////////////////


  // for (i = 0; i < windows.size(); i++)
  // {
  //   x0 = windows.at(i).c0.x; y0 = windows.at(i).c0.y;
  //   x3 = windows.at(i).c3.x; y3 = windows.at(i).c3.y;
  //   rectangle(overlay, Point(x0, y0), Point(x3, y3), Scalar(255, 0, 0), -1);  
  // } 
  // addWeighted(overlay, 0.05, src, 1.0 - 0.05, 0, src);

  /////////////// Meaningful Edges /////////////////////////////
  /*startWindow.c0.x = 553;     startWindow.c0.y = 50;
  startWindow.c1.x = 553+11;  startWindow.c1.y = 50;
  startWindow.c2.x = 553;     startWindow.c2.y = 202;
  startWindow.c3.x = 553+11;  startWindow.c3.y = 202;
  count = CountMeaningfulEdges(startWindow, edges, 0.1, K_LEFT_EDGE);
  cout << "found " << count << " left edges" << endl;
  count = CountMeaningfulEdges(startWindow, edges, 0.1, K_RIGHT_EDGE);
  cout << "found " << count << " right edges" << endl;
  /////////////// Meaningful Edges /////////////////////////////

  rectangle(overlay, Point(startWindow.c0.x, startWindow.c0.y),
                     Point(startWindow.c3.x, startWindow.c3.y),
                     Scalar(255, 0, 0), -1);  
  
  addWeighted(overlay, 0.15, src, 1.0 - 0.15, 0, src);
*/
  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  imshow( source_window, edges );
  waitKey(0);


  return 0;
}



 // command for image
 // fswebcam -r 1920x1080 image.jpg -S 20

// //////////////// CORNERS /////////////////////
//   // Corner Detection
//   for (i = 0; i < 2; i++)
//   {
//     clock_gettime(CLOCK_MONOTONIC, &start);
//     corners = GetOptimalCorners(roi_gray, &thresh);
//     clock_gettime(CLOCK_MONOTONIC, &finish);
//     elapsed = (finish.tv_sec - start.tv_sec);
//     elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
//     cout << "Corner Detection: " << elapsed * 1000.0 << " ms" << endl;
//   }
//   windowSizes.clear();
//   windowSizes.push_back(20);
//   windowSizes.push_back(30);
//   windowSizes.push_back(40);
//   windowSizes.push_back(50);
//   windowSizes.push_back(60);

//   for (i = 0; i < windowSizes.size(); i++)
//   {
//   startWindow.c0.x = 0;                   startWindow.c0.y = 100;
//   startWindow.c1.x = 0+windowSizes.at(i); startWindow.c1.y = 100;
//   startWindow.c2.x = 0;                   startWindow.c2.y = 252;
//   startWindow.c3.x = 0+windowSizes.at(i); startWindow.c3.y = 252;
//   endWindow.c0.x = 784-windowSizes.at(i); endWindow.c0.y = 50;
//   endWindow.c1.x = 784;                   endWindow.c1.y = 50;
//   endWindow.c2.x = 784-windowSizes.at(i); endWindow.c2.y = 175;
//   endWindow.c3.x = 784;                   endWindow.c3.y = 175;

//     clock_gettime(CLOCK_MONOTONIC, &start);
//     sums = GetSlidingSum(corners, thresh, startWindow, endWindow);
//     sumsVector.push_back(sums);
//     clock_gettime(CLOCK_MONOTONIC, &finish);

//     elapsed = (finish.tv_sec - start.tv_sec);
//     elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
//     cout << "Sliding Sum: " << elapsed * 1000.0 << " ms" << endl;
//   }
//   WriteSlidingWindow("../../matlab/corners.txt", argv[1], sumsVector, windowSizes);

//   // if (1)
//   // {
//   //   for (i = 0; i < corners.size(); i++)
//   //   {
//   //     x = corners.at(i).x;
//   //     y = corners.at(i).y;
//   //     printf("%d,%d\n", x, y);
//   //     circle( roi_gray, Point( x, y ), 5,  Scalar(0), 2, 8, 0 );
//   //   } 
//   //   namedWindow( source_window, CV_WINDOW_AUTOSIZE );
//   //   imshow( source_window, roi_gray );
//   //   waitKey(0);
//   // }
// //////////////// CORNERS /////////////////////
