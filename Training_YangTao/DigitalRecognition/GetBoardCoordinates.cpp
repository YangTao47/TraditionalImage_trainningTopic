#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
#include"DigitalRecognition.h"
using namespace cv;
using namespace std;

Point DigitalRecognition::getBoardCoordinates(const Mat& src, vector<vector<Point>>& P, int i)
{
	Point board_coordinate;
	Point2f srcTri[4];
	Point2f dstTri[4];
	Mat warpPerspective_mat(3, 3, CV_32FC1);
	Mat warpPerspective_dst;
	warpPerspective_dst = Mat::zeros(warpPerspectiveScale, warpPerspectiveScale, src.type());

	srcTri[0] = P[i][0];
	srcTri[1] = P[i][1];
	srcTri[2] = P[i][3];
	srcTri[3] = P[i][2];

	dstTri[0] = Point2f(0, 0);
	dstTri[1] = Point2f((float)(warpPerspectiveScale - 1), 0);
	dstTri[2] = Point2f(0, (float)(warpPerspectiveScale - 1));
	dstTri[3] = Point2f((float)(warpPerspectiveScale - 1), (float)(warpPerspectiveScale - 1));

	warpPerspective_mat = getPerspectiveTransform(srcTri, dstTri);
	warpPerspective(src, warpPerspective_dst, warpPerspective_mat, warpPerspective_dst.size());
	CV_Assert(warpPerspective_dst.channels() == 1);

	Rect rect1(36, 26, 28, 40);
	Rect rect2(96, 26, 28, 40);
	Rect rect3(36, 87, 28, 40);
	Rect rect4(96, 87, 28, 40);

	Mat rect01 = warpPerspective_dst(rect1).clone();
	Mat rect02 = warpPerspective_dst(rect2).clone();
	Mat rect03 = warpPerspective_dst(rect3).clone();
	Mat rect04 = warpPerspective_dst(rect4).clone();

	int num1 = matchTemplates(rect01, 20);
	int num2 = matchTemplates(rect02, 20);
	int num3 = matchTemplates(rect03, 20);
	int num4 = matchTemplates(rect04, 20);

	board_coordinate = composeNumbers(num1, num2, num3, num4);
	//cout << p << endl;
	return board_coordinate;
}

Point DigitalRecognition::composeNumbers(int num1, int num2, int num3, int num4)
{
	Point board_coordinate;
	if (num1 == 0){
		board_coordinate.x = num2;
	}
	else{
		board_coordinate.x = num1 * 10 + num2;
	}
	if (num3 == 0){
		board_coordinate.y = num4;
	}
	else{
		board_coordinate.y = num3 * 10 + num4;
	}
	return board_coordinate;
}

/* 模板匹配函数 */
int DigitalRecognition::matchTemplates(const Mat& src, int nums)
{
	double min_val = 0, max_val = 0;
	double temp = 0;
	int num = 0;
	Mat img_result;
	for (int i = 0; i < nums; i++)
	{
		char name[200];
		sprintf_s(name, "templates\\%d.bmp", i);
		Mat Template = imread(name, CV_LOAD_IMAGE_GRAYSCALE);
		resize(Template, Template, Size(18, 30));
		matchTemplate(src, Template, img_result, TM_CCOEFF_NORMED);
		minMaxIdx(img_result, &min_val, &max_val);
		if (temp < max_val)
		{
			temp = max_val;
			num = i;
		}
	}
	/*模板从0到19  0-9为黑底白字 10-19为白底黑字*/
	if (num >= 10){
		num -= 10;
	}
	return num;
}
