#include<iostream>
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;

class SplitImage
{
public:
	Mat segmentImage(const Mat& src)
	{
		Mat mask, hsvImage, filteredImage, dst;
		double low_H = 89, low_S = 43, low_V = 46, high_H = 124, high_S = 255, high_V = 255;
		bilateralFilter(src, filteredImage, 17, 77, 11);
		medianBlur(filteredImage, filteredImage, 3);
		cvtColor(filteredImage, hsvImage, CV_BGR2HSV);
		/* 使用inrange函数，将在这个蓝青色的颜色提取出来，得到mask矩阵 */
		inRange(hsvImage, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask);
		cvtColor(mask, mask, CV_GRAY2BGR);
		addWeighted(src, 1, mask, 1, 1, dst);
		return dst;
	}
};

int main()
{
	SplitImage split;
	Mat src, dst;
	src = imread("test.jpg");
	dst = split.segmentImage(src);
	imwrite("result.jpg", dst);
	return 0;
}
