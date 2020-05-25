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
      p1[counter] = (mean(temp)[0]);
      counter++;
    }
  }
  return I1;
}

Mat lowPass(Mat& I, Mat& I1)
{
  // accept only char type matrices
  CV_Assert(I.depth() == CV_8U);
  Mat I2(6,8, CV_8U);
  I2 = I+(I1-I)*0.25;
  return I2;
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
  Mat steps[ret.size()];
  steps[0] = Mat::zeros(6,8, CV_8U);

  for ( int i = 0; i < ret.size(); i++ ){
    image[i] = imread(ret[i].string(), 1);
    cvtColor(image[i], image[i], COLOR_BGR2GRAY);
  }

  for ( int i = 1; i < ret.size(); i++ ){
    threshold(abs(image[i-1] - image[i]), image[i-1], 0, 255, 0);
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image[i-1]);
    waitKey(0);
    steps[i] = Mat(6,8, CV_8U);
    steps[i] = calculateStep(image[i-1], steps[i]);
    steps[i] = lowPass(steps[i-1], steps[i]);
    cout << steps[i] << endl;
  }
}
