//-----------------------------------------------------------------------------
// File:        ip.c
// Author:      Garrett Bernichon
// Function:    Provide functions to do the low level image processing.
//-----------------------------------------------------------------------------
#include "ip.h"
#include <unistd.h>

int debug = 0;

// Parking Data (for interpolation)
// a pair is in the format of x position, length needed
static xy beason_ne_init[] = {{800, 195}, {865, 195}, {1040, 160}, {1215, 120}, {1550, 37}, {1600, 37}};
static xy beason_se_init[] = {{800, 280}, {870, 280}, {1280, 160}, {1455, 70}, {1550, 70}};
static xy beason_nw_init[] = {{400, 50}, {430, 50}, {485, 62}, {585, 80}, {600, 80}};
static xy beason_sw_init[] = {{250, 20}, {318, 20}, {360, 55}, {413, 80}, {600,80}};
static xy cooksie_nw_init[] = {{700, 40}, {745, 40}, {790, 25}, {840, 17}, {900, 17}};
static vector<xy> beason_ne_data(beason_ne_init, beason_ne_init + sizeof(beason_ne_init) / sizeof(xy));
static vector<xy> beason_se_data(beason_se_init, beason_se_init + sizeof(beason_se_init) / sizeof(xy));
static vector<xy> beason_nw_data(beason_nw_init, beason_nw_init + sizeof(beason_nw_init) / sizeof(xy));
static vector<xy> beason_sw_data(beason_sw_init, beason_sw_init + sizeof(beason_sw_init) / sizeof(xy));
static vector<xy> cooksie_nw_data(cooksie_nw_init, cooksie_nw_init + sizeof(cooksie_nw_init) / sizeof(xy));

// windows
static const int beason_ne_win[700] = {698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,699,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,698,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,697,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,696,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,695,694,694,694,694,694,694,694,694,694,694,693,693,693,693,693,693,693,693,693,693,693,692,692,692,692,692,692,692,692,692,692,692,691,691,691,691,691,691,691,691,691,691,691,690,690,690,690,690,690,690,690,690,690,690,689,689,689,689,689,689,689,689,689,689,689,688,688,688,688,688,688,688,688,688,688,687,687,687,687,687,687,687,687,687,687,687,686,686,686,686,686,686,686,686,686,686,686,685,685,685,685,685,685,685,685,685,685,685,684,684,684,684,684,684,684,684,684,684,684,683,683,683,683,683,683,683,683,683,683,683,682,682,682,682,682,681,681,681,681,681,681,680,680,680,680,680,679,679,679,679,679,679,678,678,678,678,678,678,677,677,677,677,677,676,676,676,676,676,676,675,675,675,675,675,675,674,674,674,674,674,673,673,673,673,673,673,672,672,672,672,672,672,671,671,671,671,671,670,670,670,670,670,670,669,669,669,669,669,669,668,668,668,668,668,667,667,667,667,667,667,666,666,666,666,666,666,665,665,665,665,665,664,664,664,664,664,664,663,663,663,663,663,663,662,662,662,662,662,661,661,661,661,661,661,660,660,660,660,660,660,659,659,659,659,658,658,658,658,657,657,657,657,657,656,656,656,656,655,655,655,655,655,654,654,654,654,653,653,653,653,653,652,652,652,652,651,651,651,651,651,650,650,650,650,649,649,649,649,648,648,648,648,648,647,647,647,647,646,646,646,646,646,645,645,645,645,644,644,644,644,644,643,643,643,643,642,642,642,642,642,641,641,640,640,640,639,639,638,638,638,637,637,636,636,636,635,635,634,634,634,633,633,632,632,632,631,631,630,630,630,629,629,628,628,628,627,627,627,626,626,625,625,625,624,624,624,623,623,622,622,622,621,621,621,620,620,619,619,619,618,618,618,617,617,616,616,616,615,615,615,614,614,613,613,613,612,612,612,611,611,610,610,610,609,609,609,608,608,607,607,607,606,606,606,605,605,604,604,604,603,603,603,602,602,601,601,601,600,600,600};
static const int beason_se_win[715] = {904,904,904,904,904,904,904,903,903,903,903,903,903,903,902,902,902,902,902,902,902,901,901,901,901,901,901,901,901,900,900,900,900,900,900,900,899,899,899,899,899,899,899,898,898,898,898,898,898,898,897,897,897,897,897,897,897,897,896,896,896,896,896,896,896,895,895,895,895,895,895,895,894,894,894,894,894,894,894,893,893,893,893,893,893,893,893,892,892,892,892,892,892,892,891,891,891,891,891,891,891,890,890,890,890,890,890,890,889,889,889,889,889,889,889,889,888,888,888,888,888,888,888,887,887,887,887,887,887,887,886,886,886,886,886,886,886,885,885,885,885,885,885,885,885,884,884,884,884,884,884,884,883,883,883,883,883,883,883,883,882,882,882,882,882,882,882,881,881,881,881,881,881,881,881,880,880,880,880,880,880,880,880,879,879,879,879,879,879,879,878,878,878,878,878,878,878,878,877,877,877,877,877,877,877,876,876,876,876,876,876,876,876,875,875,875,875,875,875,875,875,874,874,874,874,874,874,874,873,873,873,873,873,873,873,873,872,872,872,872,872,872,872,871,871,871,871,871,871,871,871,870,870,870,870,870,870,870,870,869,869,869,869,869,869,869,868,868,868,868,868,868,868,868,867,867,867,867,867,867,867,866,866,866,866,866,866,866,866,865,865,865,865,865,865,865,865,864,864,864,864,864,864,864,863,863,863,863,863,863,863,863,862,862,862,862,862,862,862,861,861,861,861,861,861,861,861,860,860,860,860,860,860,860,860,859,859,859,858,858,858,857,857,857,856,856,856,855,855,855,854,854,854,853,853,853,852,852,852,851,851,851,851,850,850,850,849,849,849,848,848,848,847,847,847,846,846,846,845,845,845,844,844,844,843,843,843,842,842,842,842,841,841,841,840,840,840,839,839,839,838,838,838,837,837,837,836,836,836,835,835,835,834,834,834,833,833,833,833,832,832,832,831,831,831,830,830,830,829,829,829,828,828,828,827,827,827,826,826,826,825,825,825,824,824,824,824,823,823,823,822,822,822,821,821,821,820,820,820,819,819,819,818,818,818,817,817,817,816,816,816,815,815,815,815,814,814,813,813,812,812,812,811,811,810,810,810,809,809,808,808,808,807,807,806,806,806,805,805,804,804,803,803,803,802,802,801,801,801,800,800,799,799,799,798,798,797,797,797,796,796,795,795,794,794,794,793,793,792,792,792,791,791,790,790,790,789,789,788,788,788,787,787,786,786,785,785,785,784,784,783,783,783,782,782,781,781,781,780,780,779,779,779,778,778,777,777,776,776,776,775,775,774,774,774,773,773,772,772,772,771,771,770,770,770,769,768,768,767,766,766,765,764,764,763,762,762,761,760,760,759,759,758,757,757,756,755,755,754,753,753,752,751,751,750,749,749,748,748,747,746,746,745,744,744,743,742,742,741,740,740,739,738,738,737,737,736,735,735,734,733,733,732,731,731,730,729,729,728,727,727,726,726,725,724,724,723,722,722,721,720,720,719,718,718,717,716,716,715,715,714,713,712,711,711,710,709,708,708,707,706,705,704,704,703,702,701,701,700,699,698,697,697,696,695,694,694,693,692,691,690,690,689,688,687,687,686,685,684,683,683,682,681,680,680};
static const int beason_nw_win[250] = {630,630,630,630,631,631,631,631,631,632,632,632,632,633,633,633,633,633,634,634,634,634,634,635,635,635,635,636,636,636,636,636,637,637,637,637,637,638,638,638,638,639,639,639,639,639,640,640,640,640,640,641,641,641,641,642,642,642,642,642,643,643,643,643,643,644,644,644,644,645,645,645,645,646,646,646,647,647,647,647,648,648,648,649,649,649,650,650,650,650,651,651,651,652,652,652,652,653,653,653,654,654,654,655,655,655,655,656,656,656,657,657,657,657,658,658,658,659,659,659,660,660,660,660,661,661,661,662,662,662,662,663,663,663,664,664,664,665,665,665,665,666,666,666,667,667,667,667,668,668,668,669,669,669,670,670,670,670,670,670,670,671,671,671,671,671,671,672,672,672,672,672,672,673,673,673,673,673,673,673,674,674,674,674,674,674,675,675,675,675,675,675,676,676,676,676,676,676,676,677,677,677,677,677,677,678,678,678,678,678,678,679,679,679,679,679,679,679,680,680,680,680,680,680,681,681,681,681,681,681,682,682,682,682,682,682,682,683,683,683,683,683,683,684,684,684,684,684,684,685};
//static const int beason_sw_win[195] = {674,673,672,671,671,670,669,668,668,667,666,665,665,664,663,662,661,661,660,659,658,658,657,656,655,655,654,653,652,651,651,650,649,648,648,647,646,645,645,644,643,642,641,641,640,639,638,638,637,636,635,635,634,633,632,631,631,630,629,628,628,627,626,625,625,627,629,631,633,636,638,640,642,644,647,649,651,653,656,658,660,662,664,667,669,671,673,675,678,680,682,684,687,689,691,693,695,698,700,702,704,706,709,711,713,715,718,720,722,724,726,729,731,733,735,737,740,742,744,746,749,751,753,755,757,760,762,764,766,768,771,773,775,777,780,780,781,781,782,782,783,784,784,785,785,786,787,787,788,788,789,789,790,791,791,792,792,793,794,794,795,795,796,796,797,798,798,799,799,800,801,801,802,802,803,803,804,805,805,806,806,807,808,808,809,809,810,810,811,812,812,813,813,814,815};
static const int beason_sw_win[240] = {665,666,667,668,669,670,671,671,672,673,674,675,676,677,677,678,679,680,681,682,683,683,684,685,686,687,688,689,689,690,691,692,693,694,695,695,696,697,698,699,700,701,701,702,703,704,705,706,707,707,708,709,710,711,712,713,713,714,715,716,717,718,719,719,720,721,722,723,724,725,725,726,726,727,728,728,729,730,730,731,731,732,733,733,734,735,735,736,736,737,738,738,739,740,740,741,741,742,743,743,744,745,745,746,746,747,748,748,749,750,750,751,751,752,753,753,754,755,755,756,756,757,758,758,759,760,760,761,761,762,763,763,764,765,765,766,766,767,768,768,769,770,770,771,771,772,773,773,774,775,775,776,776,777,777,778,778,779,779,780,780,781,781,782,782,783,783,784,784,785,785,786,786,787,787,788,788,789,789,790,790,791,791,792,792,793,793,794,794,795,795,796,796,797,797,798,798,799,799,800,800,801,801,802,802,803,803,804,804,805,805,806,806,807,807,808,808,809,809,810,810,811,811,812,812,813,813,814,814,815,815,816,816,817,817,818,818,819,819,820};
static const int cooksie_nw_win[125] = {609,608,607,606,605,605,604,603,602,601,600,600,599,598,597,596,595,595,594,593,592,591,590,590,589,588,587,586,585,585,584,583,582,581,580,580,579,578,577,576,575,575,574,573,572,571,570,570,569,568,567,566,565,565,564,563,562,561,560,560,558,557,556,555,553,552,551,550,548,547,546,545,544,542,541,540,539,537,536,535,534,532,531,530,529,528,526,525,524,523,521,520,519,518,516,515,514,513,512,510,509,508,507,505,504,503,502,500,499,498,497,496,494,493,492,491,489,488,487,486,484,483,482,481,480};


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
    blur(src, blurred, Size(3,3) );

    /// Canny detector
    Canny(blurred, edges, lowThreshold, lowThreshold * ratio, kernelSize);

    normalize(edges, edges_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
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
vector<int> GetSlidingSum(Mat img, int thresh, Window startWindow, Window endWindow, int regionId)
{
  vector<Window> windows;
  Window win;
  vector<int> sums;
  int sum, i, x, y = 0;
  int pixel;

  windows = GetSlidingWindow(startWindow, endWindow, img.rows, img.cols, regionId);

  // Sum
  for (i = 0; i < windows.size(); i++)
  {
    win = windows.at(i);

    sum = GetSumOfWindow(img, windows.at(i), thresh);

    sums.push_back(sum);
  }
  return sums;
}

vector<float> GetNormalizedSlidingSum(Mat img, int thresh, Window startWindow, Window endWindow, int regionId)
{
  vector<Window> windows;
  Window win;
  vector<float> sums;
  int sum, i, x, y = 0;
  int pixel;

  windows = GetSlidingWindow(startWindow, endWindow, img.rows, img.cols, regionId);
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

vector<int> GetSlidingEdges(Mat edges, Window startWindow, Window endWindow, float minDiff, int edge, int regionId)
{
  vector<Window> windows;
  Window win;
  vector<int> sums;
  int sum, i, x, y = 0;
  int pixel;

  windows = GetSlidingWindow(startWindow, endWindow, edges.rows, edges.cols, regionId);
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
  int aboveCount   = 0;
  int belowCount   = 0;
  bool activeBelow = false;
  bool added       = false;
  int startBelow   = 0;
  float thresh     = GetThresholdFromRegionId(regionId);
  int offset       = GetStartingXOffsetFromRegionId(regionId);
  int i;

  // Slide across the sums and count for consecutive points above/below a threshold
  for (i = 0; i < sums.size(); i++)
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
      //cout << "above " << i << " " << startBelow << endl;
      if (++aboveCount >= K_SUMS_THRESHOLD_CONSECUTIVE && not added)
      {
        //cout << "above in " << i << " " << startBelow << endl;
        activeBelow = false;
        newOpening.start = startBelow + offset;
        newOpening.length = i - startBelow - aboveCount + 1;
        openings.push_back(newOpening);
        added = true;
      }
    }
  }

  // Handle incomplete opening
  if (not added)
  {
    //cout << "adding last: start = " << startBelow + offset << endl;
    newOpening.start = startBelow + offset;
    newOpening.length = i - startBelow - aboveCount + 1;
    openings.push_back(newOpening);
  }

  return openings;
}

vector<Opening> GetOpenParkingSpaces(vector<Opening> openings, int regionId)
{
  vector<Opening> spaces;
  int reqLength = 0;

  for (int i = 0; i < openings.size(); i++)
  {
    while (IsOpeningLargeEnough(openings.at(i), regionId, &reqLength))
    {
      // Add the spot to the vector
      spaces.push_back(openings.at(i));
      if (regionId == K_COOKSIE_NW_ID)
        break; // lets skip this for cooksie because performance is low due to its distance

      // Update the starting position of the spot 
      openings.at(i).start += reqLength;

      // Update the length of this spot
      openings.at(i).length -= reqLength;
    }
  }

  return spaces;
}

bool IsOpeningLargeEnough(Opening opening, int regionId, int * reqLength)
{
  int result = false;
  int min = 10000;
  switch(regionId){
    case (K_BEASON_NE_ID):
      min = Interpolate(opening.start, beason_ne_data);
      break;
    case (K_BEASON_SE_ID):
      min = Interpolate(opening.start, beason_se_data);
      break;
    case (K_BEASON_SW_ID):
      min = Interpolate(opening.start, beason_sw_data);
      break;
    case (K_BEASON_NW_ID):
      min = Interpolate(opening.start, beason_nw_data);
      break;
    case (K_COOKSIE_NW_ID):
      min = Interpolate(opening.start, cooksie_nw_data);
      break;
    case (K_COOKSIE_SW_ID):
      min = 100000;
      break;
  }

  // return the required length
  *reqLength = min;

  if (min == -1)
    return false;

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

vector<Window> GetSlidingWindow(Window startWindow, Window endWindow, int imgHeight, int imgWidth, int regionId)
{
    int stepCount = endWindow.tl.x - startWindow.tl.x;
    vector<Window> windows;
    Window currWindow = startWindow;
    Window diffWindow;
    float dx0, dy0, dtheta, dheight;
    float tlx, tly, theta, height;
    Corner tmp;
    int i = 0;

    int * ydata = NULL;
    switch(regionId){
      case (K_BEASON_NE_ID):
        ydata = (int*)beason_ne_win;
        break;
      case (K_BEASON_SE_ID):
        ydata = (int*)beason_se_win;
        break;
      case (K_BEASON_SW_ID):
        ydata = (int*)beason_sw_win;
        break;
      case (K_BEASON_NW_ID):
        ydata = (int*)beason_nw_win;
        break;
      case (K_COOKSIE_NW_ID):
        ydata = (int*)cooksie_nw_win;
        break;
      case (K_COOKSIE_SW_ID):
        ydata = (int*)cooksie_nw_win;
        break;
    }

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
        tmp.y = ydata[i++];//tly;
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

int Interpolate(int x, vector<xy> data)
{
  int i = 0;
  int x0, x1, y0, y1;

  // Make sure x falls within the end points, otherwise return
  if (x < data.front().x || data.back().x < x)
  {
    return -1;
  }

  // find where our index "falls" within the data we have
  for(i = 0; i < data.size()-1; i++)
  {
    // find the two points x falls between
    if(data.at(i).x <= x && x < data.at(i + 1).x)
    {
      x0 = data.at(i).x;
      y0 = data.at(i).y;
      x1 = data.at(i + 1).x;
      y1 = data.at(i + 1).y;

      // interpolate
      return y0 + (int)(((float)(y1 - y0) / (float)(x1 - x0)) * (float)(x - x0));
    }
  }

  return -1;
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

int GetStartingXOffsetFromRegionId(int regionId)
{
  int offset = 0;
  switch(regionId){
    case (K_BEASON_NE_ID):
      offset = K_BEASON_NE_WIN_START_TP_X;
      break;
    case (K_BEASON_SE_ID):
      offset = K_BEASON_SE_WIN_START_TP_X;
      break;
    case (K_BEASON_SW_ID):
      offset = K_BEASON_SW_WIN_START_TP_X;
      break;
    case (K_BEASON_NW_ID):
      offset = K_BEASON_NW_WIN_START_TP_X;
      break;
    case (K_COOKSIE_NW_ID):
      offset = K_COOKSIE_NW_WIN_START_TP_X;
      break;
    case (K_COOKSIE_SW_ID):
      offset = K_COOKSIE_SW_WIN_START_TP_X;
      break;
  }
  return offset;
}

float GetDistance(ImgPoint a, ImgPoint b)
{
  float xDiff = (float)a.x - (float)b.x;
  float yDiff = (float)a.y - (float)b.y;
  //printf("xDiff %f, yDiff %f\n", xDiff,yDiff);
  return sqrt(pow(xDiff, 2) + pow(yDiff, 2));
}

bool RunSusActivity(bool carParked, bool monitorON, bool resetCount, 
  int* actCount, int baseCount, Mat image, Window carWindow, int* edgeList)
{
  int sus_thresh = 5;
  int new_detect = 0;
  bool alert = false;

  if(carParked && monitorON)
  {
    if(resetCount) {*actCount = 0;}
    new_detect = DetectActivity(image, carWindow, baseCount, edgeList);
    *actCount = *actCount + new_detect;
    if(*actCount > sus_thresh) {alert = true;}
    return alert;
  }
  else
  {
    return alert;
  }
}

int DetectActivity(Mat image, Window carWindow, int baseCount, int* edgeList)
{
  int edgeSum, edgeAvg;
  int activity;
  int thresh = 0;

  edgeSum = GetSumOfWindow(image, carWindow, thresh);
  edgeAvg = UpdateEdgeList(edgeList, edgeSum);
  if (edgeAvg > 1.02 * baseCount) {activity = 1;}
  else {activity = 0;}
  cout << "Base Count" << baseCount << endl;
  cout << "New Sum" << edgeSum << endl;
  cout << "Edge Avg" << edgeAvg << endl;
  return activity;
}

int GetBaseCount(Mat image, Window carWindow)
{
  int edgeSum;
  int thresh = 0;

  edgeSum = GetSumOfWindow(image, carWindow, thresh);
  return edgeSum;
}

int UpdateEdgeList(int* edgeList, int newSum)
{
  int avgSum;
  for(int i=9; i > 0; i--)
  {
    edgeList[i] = edgeList[i-1];
  }
  edgeList[0] = newSum;
  avgSum = (edgeList[0] + edgeList[1] + edgeList[2] + edgeList[3] + edgeList[4] 
    + edgeList[5] + edgeList[6] + edgeList[7] + edgeList[8] + edgeList[9])/10;
  return avgSum;
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
    spot.corner0  = spaces.at(i).start - 20; // top left x
    spot.corner1  = GetYPositionOfWindow(region, spaces.at(i).start, 0); // top left y
    spot.corner2  = spaces.at(i).start + spaces.at(i).length + 20; // bottom right x
    spot.corner3  = GetYPositionOfWindow(region, spaces.at(i).start, 1); // bottom right y

    spaces_db.push_back(spot);
  }

  return spaces_db;
}

// in development
int GetXPositionOfSpot(int regionId, int start)
{
  float x = 0;
  float img_width = 1129;
  float lengthOfSub = 0;
  float withinSub = 0;
  float regionStart = 0;

  switch(regionId){
  case (K_BEASON_NE_ID):
    lengthOfSub = 500;
    regionStart = 487;
    withinSub = (start - K_BEASON_NE_WIN_START_TP_X) / (float)(K_BEASON_NE_WIN_END_TP_X - K_BEASON_NE_WIN_START_TP_X);
    x = (lengthOfSub / img_width) * withinSub + (regionStart / img_width);
    x *= 100;
    break;
  case (K_BEASON_SE_ID):
    lengthOfSub = 500 + 100;
    regionStart = 487;
    withinSub = (start - K_BEASON_SE_WIN_START_TP_X) / (float)(K_BEASON_SE_WIN_END_TP_X - K_BEASON_SE_WIN_START_TP_X);
    x = (lengthOfSub / img_width) * withinSub + (regionStart / img_width);
    x *= 100;
    break;
  case (K_BEASON_SW_ID):
    lengthOfSub = 195;
    regionStart = 335-80;
    withinSub = (start - K_BEASON_SW_WIN_START_TP_X) / (float)(K_BEASON_SW_WIN_END_TP_X - K_BEASON_SW_WIN_START_TP_X);
    x = (lengthOfSub / img_width) * withinSub + (regionStart / img_width);
    x *= 100;
    break;
  case (K_BEASON_NW_ID):
    lengthOfSub = 250;
    regionStart = 430-215;
    withinSub = (start - K_BEASON_NW_WIN_START_TP_X) / (float)(K_BEASON_NW_WIN_END_TP_X - K_BEASON_NW_WIN_START_TP_X);
    x = (lengthOfSub / img_width) * withinSub + (regionStart / img_width);
    x *= 100;
    break;
  case (K_COOKSIE_NW_ID):
    x = 33000 / img_width;
    // lengthOfSub = 500;
    // regionStart = 487;
    // withinSub = (start - K_COOKSIE_NW_WIN_START_TP_X) / (float)(K_COOKSIE_NW_WIN_END_TP_X - K_COOKSIE_NW_WIN_START_TP_X);
    // x = (lengthOfSub / img_width) * withinSub + (regionStart / img_width);
    // x *= 100;
    break;
  case (K_COOKSIE_SW_ID):
    x = 39000 / img_width;
    // lengthOfSub = 500;
    // regionStart = 487;
    // withinSub = (start - K_COOKSIE_SW_WIN_START_TP_X) / (float)(K_COOKSIE_SW_WIN_END_TP_X - K_COOKSIE_SW_WIN_START_TP_X);
    // x = (lengthOfSub / img_width) * withinSub + (regionStart / img_width);
    // x *= 100;
    break;
  }
  return x;
}

// in development
int GetYPositionOfSpot(int regionId, int start)
{
  float y = 0;
  float pseudoStart = 0;
  float img_width = 1129;
  float img_height = 1183;
  float lengthOfSub = 0;
  float withinSub = 0;
  float regionEnd = 0;

  switch(regionId){
  case (K_BEASON_NE_ID):
    y = (int)(49000 / img_height);
    break;
  case (K_BEASON_SE_ID):
    y = (int)(63500 / img_height);
    break;
  case (K_BEASON_SW_ID):
    y = (int)(63500 / img_height);
    break;
  case (K_BEASON_NW_ID):
    y = (int)(49000 / img_height);
    break;
  case (K_COOKSIE_NW_ID):
    lengthOfSub = 320;
    regionEnd = 490;
    pseudoStart = (K_COOKSIE_NW_WIN_START_TP_X - start) * (K_COOKSIE_NW_WIN_END_TP_Y - K_COOKSIE_NW_WIN_START_TP_Y) / (float)(K_COOKSIE_NW_WIN_END_TP_X - K_COOKSIE_NW_WIN_START_TP_X) + K_COOKSIE_NW_WIN_START_TP_Y;
    withinSub = (pseudoStart - K_COOKSIE_NW_WIN_START_TP_Y) / (float)(K_COOKSIE_NW_WIN_END_TP_Y - K_COOKSIE_NW_WIN_START_TP_Y);
//    lengthOfSub = K_COOKSIE_NW_WIN_END_TP_X - K_COOKSIE_NW_WIN_START_TP_X;
//    regionEnd = K_COOKSIE_NW_WIN_END_TP_Y;
//    withinSub = (start - K_COOKSIE_NW_WIN_START_TP_X) / (float)(K_COOKSIE_NW_WIN_END_TP_X - K_COOKSIE_NW_WIN_START_TP_X);
    y = (lengthOfSub / img_height) * withinSub + (regionEnd / img_height);
    y *= 100;
    break;
  case (K_COOKSIE_SW_ID):
    y = start;
    break;
  }
  return y;
}

int GetYPositionOfWindow(int regionId, int start, int topBot)
{
  int y = 0;

  switch(regionId){
  case (K_BEASON_NE_ID):
    y = beason_ne_win[start - K_BEASON_NE_WIN_START_TP_X];
    break;
  case (K_BEASON_SE_ID):
    y = beason_se_win[start - K_BEASON_SE_WIN_START_TP_X];
    break;
  case (K_BEASON_SW_ID):
    y = beason_sw_win[start - K_BEASON_SW_WIN_START_TP_X];
    break;
  case (K_BEASON_NW_ID):
    y = beason_nw_win[start - K_BEASON_NW_WIN_START_TP_X];
    break;
  case (K_COOKSIE_NW_ID):
    y = cooksie_nw_win[start - K_COOKSIE_NW_WIN_START_TP_X];
    break;
  case (K_COOKSIE_SW_ID):
    y = 0;
    break;
  }

  if (topBot == 0) // upper
    return y - 110;
  else             // lower
    return y + 110;
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
  char query[120] = {0};

  // Lock table
//  cout << "before lock" << endl;
//  WaitForLockForWrite(conn, (char*)K_TBL_OPEN_PARKING);
//  cout << "got lock" << endl;

  // Clear table
  ClearTable(conn, (char*)K_TBL_OPEN_PARKING);
  cout << "cleared table" << endl;

  // Make insertions
  for(i = 0; i < spaces_db.size(); i++)
  {
    // from db_utils
    FormatInsertForOpenParking(query, (char*)K_TBL_OPEN_PARKING,
      spaces_db.at(i).spot_id,
      spaces_db.at(i).region,   // x
      spaces_db.at(i).distance, // y
      spaces_db.at(i).corner0,
      spaces_db.at(i).corner1,
      spaces_db.at(i).corner2,
      spaces_db.at(i).corner3
    );
//    printf("%s\n", query);
    if(InsertEntry(conn, query))
      cout << "error inserting" << endl;
  }
  cout << "made insertions (parking)" << endl;

  // Unlock table
//  if (UnlockTable(conn, (char*)K_TBL_OPEN_PARKING))
//      cout << "could not unlock" << endl;
//  else
//      cout << "unlocked table" << endl;

}

vector<ParkedCar> GetParkedCars(MYSQL * conn)
{
  vector<ParkedCar> cars;
  ParkedCar car_cpp;

  // Get a linked list from the C function
  PARKED_CAR_T * car_c = GetParkedCars(conn, (char *)K_TBL_PARKED_CARS);

  // Convert the linked list to a vector so that its user friendly
  if (car_c == NULL)
  {
      return cars;
  }

  do {
      car_cpp.id = car_c->car_id;
      car_cpp.susp_activity = car_c->susp_activity;
      car_cpp.tl.x = car_c->corner0;
      car_cpp.tl.y = car_c->corner1;
      car_cpp.br.x = car_c->corner2;
      car_cpp.br.y = car_c->corner3;
      cars.push_back(car_cpp);
      car_c = car_c -> next;
  } while(car_c != NULL);

  return cars;
}

void InsertSuspActivity(vector<SUSP_ACTIVITY_T> activity_db, MYSQL * conn)
{
  int i;
  char query[120] = {0};

  // Make insertions
  for(i = 0; i < activity_db.size(); i++)
  {
    // from db_utils
    FormatInsertForSuspActivity(query, (char*)K_TBL_SUSP_ACTIVITY,
      activity_db.at(i).car_id,
      activity_db.at(i).time_of_detect,
      activity_db.at(i).length_of_activity
    );
//    printf("%s\n", query);
    if(InsertEntry(conn, query))
      cout << "error inserting" << endl;
  }
  cout << "made insertions (activity)" << endl;
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
void TakeNewImage()
{
  char cmd[160] = {0};
  //sprintf(fn, "img_%04u.jpg", num);
  //sprintf(fn, "img.jpg", num);

  // Day Time
//  sprintf(cmd, "fswebcam -r 1920x1080 -s brightness=auto -s contrast=auto -s gamma=auto img_`date +%Y%m%d%H%M%S`.jpg -S 30");
  sprintf(cmd, "fswebcam -r 1920x1080 -s brightness=60%% -s contrast=80%% -s gamma=10%% img.jpg -S 50");
//  sprintf(cmd, "fswebcam -r 1920x1080 img.jpg -S 50");

  // Night Time
  //sprintf(cmd, "fswebcam -r 1920x1080 -s brightness=100%% -s contrast=100%% -s gamma=70%% img.jpg -S 10");

  cout << cmd << endl;
  system(cmd);
}

/*
void GetLatestImage(char * fn)
{
    FILE *fp = popen("ls | grep img_ | tail -2 | head -1", "r");
    fscanf(fp, "%s", fn);
    pclose(fp);
}
*/
