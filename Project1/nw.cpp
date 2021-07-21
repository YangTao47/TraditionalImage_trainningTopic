#include <iostream>
#include<opencv2\opencv.hpp>
#include<opencv2\core\core.hpp>

#define WHITE   255
#define BLACK   0
using namespace std;
using namespace cv;
void NaiveRemoveNoise(int pNum, Mat* img);
void Flood_filled_ContoursRemoveNoise(double pArea, Mat * img);
Mat cirlce_detect(Mat &src);
Mat test();

int main()
{

	Mat image=test();
	cout << image.channels() << endl;
	imshow("result", image);
	waitKey(27);
	system("pause");
	return 0;

}

Mat test()
{
	Mat imageSource = imread("C:\\Users\\hzxuxin\\Pictures\\Saved Pictures\\test.jpg");
	//imshow("原始图像", imageSource);

	Mat image;
	//GaussianBlur(imageSource, image, Size(3, 3), 10);//高斯滤波
	//Canny(imageSource, image, 80, 150);
	//medianBlur(image, image, 1);
	GaussianBlur(imageSource, image, Size(3, 3), 50);
	Canny(imageSource, image, 80, 200);
	medianBlur(image, image, 1);


	Mat structElement1 = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1));
	Mat structElement2 = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1));
	dilate(image, image, structElement1);
	erode(image, image, structElement2);
	NaiveRemoveNoise(1, &image);
	Flood_filled_ContoursRemoveNoise(3.1, &image);
	
	////vector<Vec3f> pcircles;
	//////输入，输出，方法（类型）-HOUGH_GRADIENT，dp(dp=1时表示霍夫空间与输入图像空间的大小一致，dp=2时霍夫空间是输入图像空间的一半，以此类推)，最短距离-可以分辨是两个圆否 则认为是同心圆 ,边缘检测时使用Canny算子的高阈值，中心点累加器阈值—候选圆心（霍夫空间内累加和大于该阈值的点就对应于圆心），检测到圆的最小半径，检测到圆的的最大半径
	////HoughCircles(image, pcircles, CV_HOUGH_GRADIENT, 1, 10, 100, 30, 5, 20);

	//////在原图画出圆心和圆
	////for (size_t i = 0; i < pcircles.size(); i++)//size_t 无符号整数 unsigned int 
	////{
	////	//提取圆心坐标
	////	Point center(cvRound(pcircles[i][0]), cvRound(pcircles[i][1]));//cvRound 返回和参数最接近的整数值
	////																   //提取半径
	////	int radius = cvRound(pcircles[i][2]);
	////	//圆心
	////	circle(imageSource, center, 3, Scalar(0, 255, 0), -1, 8, 0);
	////	//圆
	////	circle(imageSource, center, radius, Scalar(0, 255, 0), 3, 8, 0);
	////}


	//imshow("111", image);
	waitKey(27);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point());
	Mat imageContours = Mat::zeros(image.size(), CV_8UC1);
	Mat Contours = Mat::zeros(image.size(), CV_8UC1); //绘制

	for (int i = 0; i < contours.size(); i++)
	{
		//contours[i]代表的是第i个轮廓，contours[i].size()代表的是第i个轮廓上所有的像素点数
		for (int j = 0; j < contours[i].size(); j++)
		{
			//绘制出contours向量所有的像素点
			Point P = Point(contours[i][j].x, contours[i][j].y);
			Contours.at<uchar>(P) = 255;
		}

		//绘制轮廓
		drawContours(imageContours, contours, i, Scalar(255), 1, 8, hierarchy);
	}
	//imshow("Contours Image CV_TRET_TREE", imageContours); //轮廓
	//imshow("Point of Contours CV_CHAIN_APPROX_NONE", Contours); //向量contours内保存的所有轮廓点集
	//waitKey(0);
	return imageContours;
}





void NaiveRemoveNoise(int pNum, Mat* img)
{
	//naive remove noise
	int i, j, m, n, nValue, nCount;
	int nWidth = img->cols;
	int nHeight = img->rows;
	//set boundry to be white
	for (i = 0; i < nWidth; ++i)
	{
		img->at<uchar>(0, i) = WHITE;               //先行后列
		img->at<uchar>(nHeight - 1, i) = WHITE;       //先行后列
	}

	for (i = 0; i < nHeight; ++i)
	{
		img->at<uchar>(i, 0) = WHITE;
		img->at<uchar>(i, nWidth - 1) = WHITE;
	}
	//if the neighbor of a point is white but it is black, delete it
	for (j = 1; j < nHeight - 1; ++j)//行数
		for (i = 1; i < nWidth - 1; ++i)//列数
		{
			nValue = img->at<uchar>(j, i);
			if (!nValue)//如果是黑色噪点
			{
				nCount = 0;
				for (m = i - 1; m <= i + 1; ++m)//i是列数
					for (n = j - 1; n <= j + 1; ++n)//j是行数
					{
						if (!(img->at<uchar>(n, m)))
							nCount++;
					}
				if (nCount <= pNum)//判断3X3范围内一共有几个黑色噪点，最多有9个黑色噪点,噪点数目低于pNum, 该噪点变为白色
					img->at<uchar>(j, i) = WHITE;
			}
			else
			{
				nCount = 0;
				for (m = i - 1; m <= i + 1; ++m)
					for (n = j - 1; n <= j + 1; ++n)
					{
						if (!(img->at<uchar>(n, m)))
							nCount++;
					}
				if (nCount >= 7)
					img->at<uchar>(j, i) = BLACK;
			}
		}
}

/*  函数名：        Flood_filled_ContoursRemoveNoise
*  传入变量：
*  img：         图像指针
*  pArea:        大块黑色噪点的像素点的多少，如果少于这个数目，就把它们变为白色的背景
*  return:       void
*/
void Flood_filled_ContoursRemoveNoise(double pArea, Mat * img)
{
	int i, j;
	int color = 1;
	int nHeight = img->rows;
	int nWidth = img->cols;

	for (i = 0; i < nWidth; ++i)//列数
		for (j = 0; j < nHeight; ++j) //行数
		{
			if (!img->at<uchar>(j, i))
			{
				//FloodFill each point in connect area using different color
				floodFill(*img, cvPoint(i, j), cvScalar(color));
				color++;
			}
		}

	int ColorCount[255] = { 0 };
	for (i = 0; i < nWidth; ++i)  //列数
	{
		for (j = 0; j < nHeight; ++j)  //行数
		{
			//caculate the area of each area
			if (img->at<uchar>(j, i) != 255)
			{
				ColorCount[img->at<uchar>(j, i)]++;
			}
		}
	}
	//get rid of noise point
	for (i = 0; i < nWidth; ++i)  //列数
	{
		for (j = 0; j < nHeight; ++j)  //行数
		{
			if (ColorCount[img->at<uchar>(j, i)] <= pArea)
			{
				img->at<uchar>(j, i) = WHITE;
			}
		}
	}
	for (i = 0; i < nWidth; ++i)//列数
	{
		for (j = 0; j < nHeight; ++j) //行数
		{
			if (img->at<uchar>(j, i) < WHITE)
			{
				img->at<uchar>(j, i) = BLACK;
			}
		}
	}
}

Mat cirlce_detect(Mat &src)
{
	
	Mat medianImg;
	Mat grayImg;
	Mat houghCirclesImg;
	if (src.empty())
	{
		cout << "fail to load Img" << endl;
	}

	//霍夫圆检测
	vector<Vec3f> pcircles;
	//输入，输出，方法（类型）-HOUGH_GRADIENT，dp(dp=1时表示霍夫空间与输入图像空间的大小一致，dp=2时霍夫空间是输入图像空间的一半，以此类推)，最短距离-可以分辨是两个圆否 则认为是同心圆 ,边缘检测时使用Canny算子的高阈值，中心点累加器阈值—候选圆心（霍夫空间内累加和大于该阈值的点就对应于圆心），检测到圆的最小半径，检测到圆的的最大半径
	HoughCircles(grayImg, pcircles, CV_HOUGH_GRADIENT, 1, 10, 100, 30, 5, 50);

	//在原图画出圆心和圆
	for (size_t i = 0; i < pcircles.size(); i++)//size_t 无符号整数 unsigned int 
	{
		//提取圆心坐标
		Point center(cvRound(pcircles[i][0]), cvRound(pcircles[i][1]));//cvRound 返回和参数最接近的整数值
																	   //提取半径
		int radius = cvRound(pcircles[i][2]);
		//圆心
		circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//圆
		circle(src, center, radius, Scalar(0, 255, 0), 3, 8, 0);
	}

	return src;

}

