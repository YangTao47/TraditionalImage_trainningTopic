#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

//旋转函数
Mat RotateImg(Mat image, double angle);



int main()
{
	int alpha;

	cin >> alpha;


	Mat image = imread("D:\\Project1\\lena.jpg");
	Mat imgout = RotateImg(image, alpha);
	imshow("test", imgout);
	waitKey(0);
	getchar();
	
	return 0;
}


//旋转函数 旋转方向为逆时针为正
Mat RotateImg(Mat image, double angle)
{
	/*
	对旋转的进行改进，由于图形是一个矩形，旋转后的新图像的形状是一个原图像的外接矩形
	因此需要重新计算出旋转后的图形的宽和高
	*/
	int width = image.cols;
	int height = image.rows;

	double radian = angle * CV_PI / 180.;//角度转换为弧度 sin/cos 都是用弧度计算
	double width_rotate = fabs(width*cos(radian)) + fabs(height*sin(radian));
	double height_rotate = fabs(width*sin(radian)) + fabs(height*cos(radian));

	//旋转中心 原图像中心点
	cv::Point2f center((float)width / 2.0, (float)height / 2.0);
	//旋转矩阵  只是进行了旋转并没有移动
	Mat m1 = cv::getRotationMatrix2D(center, angle, 1.0);
	//cout << m1.size << endl;  2x3
	//m1为2行3列通道数为1的矩阵
	//变换矩阵的中心点相当于平移一样 原图像的中心点与新图像的中心点的相对位置（原图像中心点需要平移的量） 中心点（x1，y1）（x，y）  x1-x ，，y1-y
	m1.at<double>(0, 2) += (width_rotate - width) / 2.;
	m1.at<double>(1, 2) += (height_rotate - height) / 2.;
	Mat imgOut;
	if (image.channels() == 1)
	{
		cv::warpAffine(image, imgOut, m1, cv::Size(width_rotate, height_rotate), cv::INTER_LINEAR, 0, Scalar(0));
	}
	else if (image.channels() == 3)
	{
		cv::warpAffine(image, imgOut, m1, cv::Size(width_rotate, height_rotate), cv::INTER_LINEAR, 0, Scalar(0, 0, 0));
	}
	return imgOut;
}
