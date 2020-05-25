#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
using namespace cv;
using namespace std;
using namespace boost::filesystem;

Mat& calculateStep(Mat& I, Mat& I1)
{
  // accept only char type matrices
  CV_Assert(I.depth() == CV_8U);
  int nRows = I.rows;
  int nCols = I.cols;
  int i,h,counter=0;
  uchar* p;
  uchar* p1 = I1.ptr<uchar>(0);
  for( h = 0; h < nCols*7/8; h += (int)nCols/8)
  {
    for( i = 0; i < nRows*5/6; i+=(int)nRows/6)
    {
      Mat temp = I(Rect(h, i, (int)nCols/8, (int)nRows/6));
      p1[counter] = ((int)(mean(temp)[0]))*127/255;
      counter++;
    }
  }
  return I1;
}

void findCentroid(Mat& I, vector<Point>& centroids)
{
  if (countNonZero(I) == 0)
  {
    centroids.push_back(Point(0, 0));
    return;
  }
  Moments m = moments(I,true);
  Point p(m.m10/m.m00, m.m01/m.m00);
  centroids.push_back(p);
}

Mat lowPass(Mat& I, Mat& I1)
{
  // accept only char type matrices
  CV_Assert(I.depth() == CV_8S);
  Mat I2(6,8, CV_8S);
  I2 = I+(I1-I)*0.25;
  return I2;
}

void filterSteps(vector<Point>& centroids, vector<Mat>& steps)
{
  double angle, originAngle;
  int originIndx = 0;
  for (int i = 2; i < centroids.size(); i++)
  {
    if (centroids[i-1].y == 0 && centroids[i-1].x == 0)
    {
      steps.erase(steps.begin() + i-1);
      centroids.erase(centroids.begin() + i-1);
      i++;
    }
    else
    {
      Point p1 = centroids[i-1];
      Point p2 = centroids[i];
      Point origin = centroids[originIndx];
      angle = atan2(p1.y - p2.y, p1.x - p2.x);
      originAngle = atan2(p1.y - origin.y, p1.x - origin.x);
      if (angle  < 0) {angle += 2*CV_PI;}
      if (originAngle  < 0) {originAngle += 2*CV_PI;}
      angle = angle * (180.0/CV_PI);
      originAngle = originAngle * (180.0/CV_PI);
      if (abs(angle-originAngle) > 0.1*originAngle)
      {
        int j = originIndx+1;
        while (j < i-1)
        {
          steps.erase(steps.begin() + originIndx);
          centroids.erase(centroids.begin() + originIndx);
        }
        originIndx = originIndx+1;
      }
    }
  }
}

int main(int argc, char** argv )
{
  if ( argc != 2 )
    {
        printf("enter one input of directory path to images\n");
        return -1;
    }
  path p (argv[1]);
  vector<path> ret;
  if (is_directory(p)){
    recursive_directory_iterator it(p);
    recursive_directory_iterator endit;
    while(it != endit)
    {
        if(is_regular_file(*it) && it->path().extension() == ".png")
        {
          path canonicalPath = canonical(it->path(), path("./"));
          ret.push_back(canonicalPath);
        }
        ++it;
    }
  }
  else
  {
      printf("enter valid path\n");
      return -1;
  }
  Mat image[ret.size()];
  vector<Mat> steps;
  vector<Point> centroids;
  steps.push_back(Mat::zeros(6,8, CV_8S));

  for ( int i = 0; i < ret.size(); i++ ){
    image[i] = imread(ret[i].string(), 1);
    cvtColor(image[i], image[i], COLOR_BGR2GRAY);
  }

  centroids.push_back(Point(0, 0));

  for ( int i = 1; i < ret.size(); i++ ){
    threshold(abs(image[i-1] - image[i]), image[i-1], 0, 255, 0);
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image[i-1]);
    waitKey(0);
    steps.push_back(Mat(6,8, CV_8S));
    steps[i] = calculateStep(image[i-1], steps[i]);
    cout << steps[i] << endl;
    steps[i] = lowPass(steps[i-1], steps[i]);
    findCentroid(image[i-1], centroids);
    cout << centroids[i] <<endl;
    cout << steps[i] << endl;
  }
  filterSteps(centroids, steps);
  cout <<centroids<<endl;
}
