#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;

Mat& ScanImageAndReduceC(Mat& I, Mat& I1)
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
      p1[counter] = (int)mean(temp)[0];
      counter++;
    }
  }
  return I1;
}

int main(int argc, char** argv )
{
    if ( argc != 4 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
    Mat image, image1, image2;
    Mat image3(6,8, CV_8U);
    image = imread( argv[1], 1 );
    image1 = imread( argv[2], 1 );
    image2 = imread( argv[3], 1 );
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
    if ( !image2.data )
    {
        printf("No image data \n");
        return -1;
    }
    cvtColor(image, image, COLOR_BGR2GRAY);
    cvtColor(image1, image1, COLOR_BGR2GRAY);
    cvtColor(image2, image2, COLOR_BGR2GRAY);
    threshold(abs(image - image1), image, 0, 255, 0);
    image3 = ScanImageAndReduceC(image, image3);
    std::cout << image3 << std::endl;
    threshold(abs(image2 - image1), image2, 0, 255, 0);
    image3 = ScanImageAndReduceC(image2, image3);
    std::cout << image3 << std::endl;
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image2);
    waitKey(0);
    return 0;
}
