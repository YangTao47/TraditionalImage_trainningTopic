#include<iostream>
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;
Mat SegmentImage(const Mat &srcImage);

int main()
{
	Mat image;
	image = imread("test.jpg");
	Mat result;
	result = SegmentImage(image);
	imwrite("result.jpg", result);
	return 0;
}

Mat SegmentImage(const Mat &srcImage)
{
	Mat mask;
	double low_H = 90, low_S = 43, low_V = 46, high_H = 124, high_S = 255, high_V = 255;
	cvtColor(srcImage, srcImage, CV_BGR2HSV);
	/* 使用inrange函数，将在这个蓝青色的颜色提取出来，得到mask矩阵 */
	inRange(srcImage, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask);
	cvtColor(mask, mask, CV_GRAY2BGR);
	cvtColor(srcImage, srcImage, CV_HSV2BGR);
	addWeighted(srcImage, 1, mask, 1, 1, srcImage);
	medianBlur(srcImage, srcImage, 5);
	return srcImage;
}
