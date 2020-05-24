#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;

Mat& ScanImageAndReduceC(Mat& I)
{
  // accept only char type matrices
  CV_Assert(I.depth() == CV_8U);
  int channels = I.channels();
  int nRows = I.rows;
  int nCols = I.cols * channels;
  if (I.isContinuous())
  {
      nCols *= nRows;
      nRows = 1;
  }
  int i,j;
  uchar* p;
  for( i = 0; i < nRows; ++i)
  {
      p = I.ptr<uchar>(i);
      for ( j = 0; j < nCols; ++j)
      {
        int si = (int)p[j];
        std::string s = std::to_string(si);
        std::cout << s << std::endl;
      }
  }
  return I;
}

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
    Mat image;
    image = imread( argv[1], 1 );
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    cvtColor(image, image, cv::COLOR_BGR2GRAY);
    ScanImageAndReduceC(image);
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);
    waitKey(0);
    return 0;
}
