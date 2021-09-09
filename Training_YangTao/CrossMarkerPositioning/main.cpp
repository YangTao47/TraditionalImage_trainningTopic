#include<iostream>
#include<opencv2\opencv.hpp>
#include<chrono>
#include<linemod\gradientMatch.h>
using namespace std;
using namespace cv;

class LocateCrossMark
{
public:
	/*******方法一*********/
	Point locateCrossMark(const Mat& Image)
	{
		Point2f p;
		vector<vector<Point>>contours;
		Mat gray_image;
		cvtColor(Image, gray_image, CV_BGR2GRAY);
		threshold(gray_image, gray_image, 1, 255, THRESH_OTSU + THRESH_BINARY);
		findContours(gray_image, contours, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
		p = operateRectangular(Image, contours);
		return p;
	}
	/**********************************方法二训练+测试********************************************************/
	Point lineModMatchResult(const Mat& Image)
	{
		lineModMatch test;
		Point left_up_point;
		test.MIPP_test();
		test.image_path("D:/", "D:/train.jpg", "D:/1.bmp");
		test.angle_test("test", left_up_point);
		return left_up_point;
	}

private:
	const int templSize = 205;
	/***********************************方法一旋转矩操作（20ms）************************************/
	Point2f operateRectangular(const Mat& Image, const vector<vector<Point>>contours)
	{
		Mat box;
		Point2f p1;
		double h_minVal = 0, h_maxVal = 0;
		double wminVal = 0, wmaxVal = 0;
		float circle_radius = 0;
		int contours_size = (int)contours.size();
		for (int i = 0; i < contours_size; i++)
		{
			RotatedRect r = minAreaRect(Mat(contours[i]));
			boxPoints(r, box);
			minMaxIdx(box.col(0), &h_minVal, &h_maxVal);
			minMaxIdx(box.col(1), &wminVal, &wmaxVal);
			double height = h_maxVal - h_minVal;
			double width = wmaxVal - wminVal;
			if ((float)width / height >= 0.9 && (float)width / height <= 1.2 && (float)width * height >= 40000 && (float)width * height <= 50000)
			{
				minEnclosingCircle(box, p1, circle_radius);
				circle(Image, p1, (int)circle_radius, Scalar(0, 0, 255));
			}
		}
		return p1;
	}

	/*********************************方法二模板匹配（旋转模板匹配未改尺寸60ms）**************************************/
};

int main()
{
	LocateCrossMark locate;
	Mat image = imread("产品\\正\\2.bmp");
	Point2f p1;
	auto start_time = chrono::high_resolution_clock::now();
	p1 = locate.lineModMatchResult(image);
	chrono::time_point<chrono::high_resolution_clock> end_time = chrono::high_resolution_clock::now();
	chrono::milliseconds processing_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time) / 10;
	cout << processing_time.count() << endl;
	system("pause");
	return 0;
}
