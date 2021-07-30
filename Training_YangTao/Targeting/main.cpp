#include<iostream>
#include<opencv2\opencv.hpp>
using namespace cv;
using namespace std;

class ObjectTarget
{
public:
	Mat targeting()
	{
		Mat dst, mask;
		Mat image = imread("lcd.bmp");
		Point2f P[4];
		image.copyTo(dst);
		cvtColor(image, image, CV_BGR2GRAY);
		threshold(image, image, 21, 255, THRESH_BINARY + THRESH_OTSU);
		vector<vector<Point>> contours;
		findContours(image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		//矩形操作
		rectangularOperation(contours, dst);
		return dst;
	}

	Point2f rectangularOperation(const vector<vector<Point>>contours, Mat& dst)
	{
		Mat box;
		Point2f P[4];
		double hminVal = 0, hmaxVal = 0;
		double wminVal = 0, wmaxVal = 0;
		/*获得轮廓点的最小旋转矩形*/
		for (int i = 0; i < contours.size(); i++)
		{
			RotatedRect r = minAreaRect(Mat(contours[i]));
			boxPoints(r, box);
			minMaxIdx(box.col(0), &hminVal, &hmaxVal);
			minMaxIdx(box.col(1), &wminVal, &wmaxVal);
			double height = hmaxVal - hminVal;
			double width = wmaxVal - wminVal;
			//根据矩形宽高比和面积筛选矩形
			if ((float)width * height >= 600000 && (float)width * height <= 10000000)
			{
				r.points(P);
				for (int j = 0; j <= 3; j++)
				{
					line(dst, P[j], P[(j + 1) % 4], Scalar(0, 0, 255), 1);
				}
			}
		}
		return P[4];
	}
};

int main()
{
	Mat dst;
	Point2f P[4];
	ObjectTarget test;
	dst = test.targeting();
	imwrite("result.jpg", dst);
	return 0;
}

