#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;

Mat& ScanImageAndReduceC(Mat& I, Mat& I1)
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
  uchar* p1;
  for( i = 0; i < nRows; ++i)
  {
      p = I.ptr<uchar>(i);
      p1 = I1.ptr<uchar>(i);
      for ( j = 0; j < nCols; ++j)
      {
        if ( (int)p[j] != (int)p1[j] ){
          p[j] = (uchar)255;
        }
        else{
          p[j] = (uchar)0;
        }
      }
  }
  return I;
}

int main(int argc, char** argv )
{
    if ( argc != 3 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
    Mat image, image1;
    image = imread( argv[1], 1 );
    image1 = imread( argv[2], 1 );
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    if ( !image1.data )
    {
        printf("No image data \n");
        return -1;
    }
    cvtColor(image, image, COLOR_BGR2GRAY);
    cvtColor(image1, image1, COLOR_BGR2GRAY);
    // image = ScanImageAndReduceC(image, image1);
    threshold(abs(image - image1), image, 0, 255, 0);
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);
    waitKey(0);
    return 0;
}
