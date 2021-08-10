#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
#include"DigitalRecognition.h"
#include"CellBoardCoordinates.h"
using namespace cv;
using namespace std;
using namespace std::chrono;

vector<CellBoardCoordinates> DigitalRecognition::extractCellCoordinates(const Mat& Image)
{
	CellBoardCoordinates cell_board_point;
	vector<CellBoardCoordinates> cell_board_point_vector;
	vector<vector<Point2f>> P;
	Point cell_point;
	Point board_point;
	P = findCellBox1(Image);
	for (int i = 0; i < P.size(); i++)
	{
		cell_point = getBoardCoordinates(Image, P, i);
		board_point = P[i][0];
		cell_board_point.cellCoordinates = cell_point;
		cell_board_point.boardCoordinates = board_point;
		cell_board_point_vector.push_back(cell_board_point);
	}
	return cell_board_point_vector;
}

vector<vector<Point2f>> DigitalRecognition::findCellBox1(const Mat& src)
{
	Mat src_copy, gray_image, CannyImg, src_copy2;
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	vector<vector<Point2f>>P;
	vector<Point2f> corners;//提供初始角点的坐标位置和精确的坐标的位置
	int maxcorners = 200;
	double qualityLevel = 0.3;  //角点检测可接受的最小特征值
	double minDistance = 50;	//角点之间最小距离
	int blockSize = 3;//计算导数自相关矩阵时指定的领域范围
	double  k = 0.04; //权重系数
	src_copy = src.clone();
	src_copy2 = src.clone();
	cvtColor(src_copy, gray_image, COLOR_BGR2GRAY);
	adaptiveThreshold(gray_image, gray_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 2111, 1);
	gray_image = removeWhiteNoise(gray_image);
	gray_image = removeBlackNoise(gray_image);
	morphologyEx(gray_image, gray_image, MORPH_CLOSE, element, Point(-1, -1), 5);
	Canny(gray_image, CannyImg, 140, 250, 3);
	/* 角点检测 */
	goodFeaturesToTrack(CannyImg, corners, maxcorners, qualityLevel, minDistance, Mat(), blockSize, false, k);
	//showAllCorners(corners, src_copy2);
	P = sortCorners(corners, src_copy);
	return P;
}

void DigitalRecognition::showAllCorners(vector<Point2f> corners, Mat& showimage)
{
	for (int i = 0; i < corners.size(); i++)
	{
		circle(showimage, corners[i], 10, Scalar(0, 0, 255), -1, 8, 0);
	}
}

/*把角点按照左上、右上、右下、左上的顺序放入容器中*/
vector<vector<Point2f>> DigitalRecognition::sortCorners(vector<Point2f> corners, Mat& showimage)
{
	vector<vector<Point2f>>P;
	vector<vector<Point2f>>P1;
	for (int i = 0; i < corners.size(); i++)
	{
		vector<Point2f>p1;
		for (int j = 0; j < corners.size(); j++)
		{
			if (corners[i].x - corners[j].x> -720 && corners[i].x - corners[j].x < -640 && corners[i].y - corners[j].y < 25 && corners[i].y - corners[j].y > -25)
			{
				p1.push_back(corners[i]);
				p1.push_back(corners[j]);
				P.push_back(p1);
			}
		}
	}

	for (int i = 0; i < P.size(); i++)
	{
		Point2f p1;
		for (int j = 0; j < corners.size(); j++)
		{	/* 已修改参数 -700  -650*/
			if (P[i][0].x - corners[j].x> -710 && P[i][0].x - corners[j].x < -640 && P[i][0].y - corners[j].y < -640 && P[i][0].y - corners[j].y > -700)
			{
				P[i].push_back(corners[j]);
			}
		}
	}

	for (int i = 0; i < P.size(); i++)
	{
		Point2f p1;
		for (int j = 0; j < corners.size(); j++)
		{	/* 已修改参数 -700  -650*/
			if (P[i][0].x - corners[j].x> -20 && P[i][0].x - corners[j].x < 20 && P[i][0].y - corners[j].y < -650 && P[i][0].y - corners[j].y > -700)
			{
				P[i].push_back(corners[j]);
			}
		}
	}

	/* 防止有容器中出现只有三个角点的情况引起程序崩溃 */
	for (int i = 0; i < P.size(); i++) 
	{
		if (P[i].size() == 4) 
		{
			P1.push_back(P[i]);
		}
	}

	/******************在传入的图中画角点****************************/
	/* 
	for (int i = 0; i < P1.size(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			circle(showimage, P1[i][j], 10, Scalar(0, 0, 255), -1, 8, 0);
		}
	}*/

	return P1;
}

Point DigitalRecognition::getBoardCoordinates(const Mat& src, vector<vector<Point2f>> P, int i)
{
	Point p;
	Point2f srcTri[4];
	Point2f dstTri[4];
	Mat warpPerspective_mat(3, 3, CV_32FC1);
	Mat warpPerspective_dst;
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	int row_col = 700;
	warpPerspective_dst = Mat::zeros(row_col, row_col, src.type());

	srcTri[0] = P[i][0];
	srcTri[1] = P[i][1];
	srcTri[2] = P[i][3];
	srcTri[3] = P[i][2];

	dstTri[0] = Point2f(0, 0);
	dstTri[1] = Point2f((float)(row_col - 1), 0);
	dstTri[2] = Point2f(0, (float)(row_col - 1));
	dstTri[3] = Point2f((float)(row_col - 1), (float)(row_col - 1));

	warpPerspective_mat = getPerspectiveTransform(srcTri, dstTri);
	warpPerspective(src, warpPerspective_dst, warpPerspective_mat, warpPerspective_dst.size());

	cvtColor(warpPerspective_dst, warpPerspective_dst, CV_BGR2GRAY);
	adaptiveThreshold(warpPerspective_dst, warpPerspective_dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 2111, 1);
	morphologyEx(warpPerspective_dst, warpPerspective_dst, MORPH_CLOSE, element, Point(-1, -1), 4);
	morphologyEx(warpPerspective_dst, warpPerspective_dst, MORPH_OPEN, element, Point(-1, -1), 3);
	medianBlur(warpPerspective_dst, warpPerspective_dst, 5);

	Rect rect1(145, 100, 150, 190);
	Rect rect2(395, 100, 150, 190);
	Rect rect3(145, 360, 150, 190);
	Rect rect4(395, 360, 150, 190);

	Mat rect01 = warpPerspective_dst(rect1).clone();
	Mat rect02 = warpPerspective_dst(rect2).clone();
	Mat rect03 = warpPerspective_dst(rect3).clone();
	Mat rect04 = warpPerspective_dst(rect4).clone();

	Mat leftImg01, rightImg01;
	Mat leftImg02, rightImg02;
	Mat leftImg03, rightImg03;
	Mat leftImg04, rightImg04;

	int res01 = cutLeft(rect01, leftImg01, rightImg01);
	int res02 = cutLeft(rect02, leftImg02, rightImg02);
	int res03 = cutLeft(rect03, leftImg03, rightImg03);
	int res04 = cutLeft(rect04, leftImg04, rightImg04);

	int num1 = matchTemplates(leftImg01, 20);
	int num2 = matchTemplates(leftImg02, 20);
	int num3 = matchTemplates(leftImg03, 20);
	int num4 = matchTemplates(leftImg04, 20);

	p = composeNumbers(num1, num2, num3, num4);
	return p;
}

Point DigitalRecognition::composeNumbers(int num1, int num2, int num3, int num4)
{
	Point p;
	if (num1 == 0)
	{
		p.x = num2;
	}
	else
	{
		p.x = num1 * 10 + num2;
	}
	if (num3 == 0)
	{
		p.y = num4;
	}
	else
	{
		p.y = num3 * 10 + num4;
	}
	//cout << p << endl;
	return p;
}

/* 模板匹配函数 */
int DigitalRecognition::matchTemplates(const Mat& src, int nums)
{
	double minVal = 0, maxVal = 0;
	double temp = 0;
	int serieNum = 0;
	Mat img_result;
	for (int i = 0; i < nums; i++)
	{
		char name[200];
		sprintf_s(name, "templates\\%d.bmp", i);
		Mat Template = imread(name, CV_LOAD_IMAGE_GRAYSCALE);
		resize(Template, Template, Size(src.cols, src.rows));
		matchTemplate(src, Template, img_result, TM_CCOEFF_NORMED);
		minMaxIdx(img_result, &minVal, &maxVal);
		if (temp < maxVal)
		{
			temp = maxVal;
			serieNum = i;
		}
	}
	/*模板从0到19  0-9为黑底白字 10-19为白底黑字*/
	if (serieNum >= 10)
	{
		serieNum -= 10;
	}
	return serieNum;
}

int DigitalRecognition::getColSum(Mat src, int col)//统计所有列像素的总和
{
	int sum = 0;
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < height; i++)
	{
		sum = sum + src.at <uchar>(i, col);
	}
	return sum;
}

int DigitalRecognition::getRowSum(Mat src, int row)//统计所有行像素的总和
{
	int sum = 0;
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < width; i++)
	{
		sum += src.at <uchar>(row, i);
	}
	return sum;
}

void DigitalRecognition::cutTop(Mat& src, Mat& dstImg)//上下切割
{
	int top, bottom;
	top = 0;
	bottom = src.rows;

	/*黑底白色数字ROI扫描*/
	int i;
	for (i = 0; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);//统计所有行像素的总和
										 //cout <<i<<" th "<< colValue << endl;
		if (colValue>0)//扫描直到行像素的总和大于0时，记下当前位置top
		{
			top = i;
			break;
		}
	}
	for (; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);//统计所有行像素的总和
										 //cout << i << " th " << colValue << endl;
		if (colValue == 0)//继续扫描直到行像素的总和等于0时，记下当前位置bottom
		{
			bottom = i;
			break;
		}
	}

	/* 白底黑色数字ROI扫描 */
	if (top == 0)
	{

		int j;
		for (j = 0; j < src.rows; j++)
		{
			int colValue = getRowSum(src, j);//统计所有行像素的总和
											 //cout <<i<<" th "<< colValue << endl;
			if (colValue<src.cols * 255)//扫描直到行像素的总和小于列数*255时，记下当前位置top
			{
				top = j;
				break;
			}
		}
		for (; j < src.rows; j++)
		{
			int colValue = getRowSum(src, j);//统计所有行像素的总和
											 //cout << i << " th " << colValue << endl;
			if (colValue == src.cols * 255)//继续扫描直到行像素的总和等于列数*255时，记下当前位置bottom
			{
				bottom = j;
				break;
			}
		}
	}

	int height = bottom - top;
	if (height < 125)
	{
		height = 125;
	}

	Rect rect(0, top, src.cols, height);
	dstImg = src(rect).clone();
}


int DigitalRecognition::cutLeft(Mat& src, Mat& leftImg, Mat& rightImg)//左右切割
{
	int left, right;
	left = 0;
	right = src.cols;
	
	/*黑底白色数字ROI扫描*/
	int i;
	for (i = 0; i < src.cols; i++)
	{
		int colValue = getColSum(src, i);//统计所有列像素的总和
										 //cout <<i<<" th "<< colValue << endl;
		if (colValue>0)//扫描直到列像素的总和大于0时，记下当前位置left
		{
			left = i;
			break;
		}
	}

	//继续扫描
	for (; i < src.cols; i++)
	{
		int colValue = getColSum(src, i);//统计所有列像素的总和
										 //cout << i << " th " << colValue << endl;
		if (colValue == 0)//继续扫描直到列像素的总和等于0时，记下当前位置right
		{
			right = i;
			break;
		}
	}

	/* 白底黑色数字ROI扫描 */
	if (left == 0)
	{
		int j;
		for (j = 0; j < src.cols; j++)
		{
			int colValue = getColSum(src, j);//统计所有列像素的总和
											 //cout <<i<<" th "<< colValue << endl;
			if (colValue<src.rows * 255)//扫描直到列像素的总和小于列数*255时，记下当前位置left
			{
				left = j;
				break;
			}
		}

		//继续扫描
		for (; j < src.cols; j++)
		{
			int colValue = getColSum(src, j);//统计所有列像素的总和
											 //cout << i << " th " << colValue << endl;
			if (colValue == src.rows * 255)//继续扫描直到列像素的总和等于列数*255时，记下当前位置right
			{
				right = j;
				break;
			}
		}
	}

	/*扩大类似 1 这样的数字的ROI范围 提高可靠性 */
	int width = right - left;//分割图片的宽度则为right - left
	if (width < 48)
	{
		width = 48;
	}

	Rect rect(left, 0, width, src.rows);//构造一个矩形，参数分别为矩形左边顶部的X坐标、Y坐标，右边底部的X坐标、Y坐标（左上角坐标为0，0）
	leftImg = src(rect).clone();
	Rect rectRight(right, 0, src.cols - right, src.rows);//分割后剩下的原图
	rightImg = src(rectRight).clone();
	cutTop(leftImg, leftImg);//上下切割
	return 0;
}



Mat DigitalRecognition::removeWhiteNoise(Mat& src)
{
	Mat src_color, labels, stats, centroids, reversal, dst;
	if (src.channels() == 3)
	{
		cvtColor(src, src, CV_BGR2GRAY);
	}
	reversal = reverseImage(src);
	cvtColor(src, src, CV_GRAY2BGR);
	int num = connectedComponentsWithStats(reversal, labels, stats, centroids, 4);
	vector<Vec3b> color(num + 1);
	color[0] = Vec3b(0, 0, 0);
	for (int m = 0; m < num; m++) {
		color[m] = Vec3b(0, 0, 0);
		if (stats.at<int>(m, CC_STAT_AREA) < 15000)
		{
			color[m] = Vec3b(255, 255, 255);
		}
	}
	src_color = Mat::zeros(src.size(), CV_8UC3);
	for (int y = 0; y < src_color.rows; y++)
	{
		for (int x = 0; x < src_color.cols; x++)
		{

			int label = labels.at<int>(y, x);//注意labels是int型，不是uchar.
			CV_Assert(0 <= label && label <= num);
			src_color.at<Vec3b>(y, x) = color[label];
		}
	}
	addWeighted(src, 1, src_color, 1, 1, dst);
	cvtColor(dst, dst, CV_BGR2GRAY);
	return dst;
}

Mat DigitalRecognition::removeBlackNoise(Mat& src)
{
	Mat src_color, labels, stats, centroids, reversal, dst;
	if (src.channels() == 3)
	{
		cvtColor(src, src, CV_BGR2GRAY);
	}
	adaptiveThreshold(src, src, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 1333, 1);
	reversal = reverseImage(src);
	int num = connectedComponentsWithStats(src, labels, stats, centroids, 4);
	vector<Vec3b> color(num + 1);
	color[0] = Vec3b(0, 0, 0);
	for (int m = 0; m < num; m++) {
		color[m] = Vec3b(0, 0, 0);
		if (stats.at<int>(m, CC_STAT_AREA) < 8000)
		{
			color[m] = Vec3b(255, 255, 255);
		}
	}
	src_color = Mat::zeros(src.size(), CV_8UC3);
	for (int y = 0; y < src_color.rows; y++)
	{
		for (int x = 0; x < src_color.cols; x++)
		{

			int label = labels.at<int>(y, x);//注意labels是int型，不是uchar.
			CV_Assert(0 <= label && label <= num);
			src_color.at<Vec3b>(y, x) = color[label];
		}
	}
	cvtColor(reversal, reversal, CV_GRAY2BGR);
	addWeighted(reversal, 1, src_color, 1, 1, dst);
	dst = reverseImage(dst);
	cvtColor(dst, dst, CV_BGR2GRAY);
	return dst;
}

Mat DigitalRecognition::reverseImage(Mat& src)
{

	Mat img_result;
	uchar lutData[256];
	for (int i = 0; i < 256; i++)
	{
		lutData[i] = 255 - i;
	}
	Mat lut(1, 256, CV_8UC1, lutData);
	LUT(src, lut, img_result);

	return img_result;
}
