#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

//��ת����
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


//��ת���� ��ת����Ϊ��ʱ��Ϊ��
Mat RotateImg(Mat image, double angle)
{
	/*
	����ת�Ľ��иĽ�������ͼ����һ�����Σ���ת�����ͼ�����״��һ��ԭͼ�����Ӿ���
	�����Ҫ���¼������ת���ͼ�εĿ�͸�
	*/
	int width = image.cols;
	int height = image.rows;

	double radian = angle * CV_PI / 180.;//�Ƕ�ת��Ϊ���� sin/cos �����û��ȼ���
	double width_rotate = fabs(width*cos(radian)) + fabs(height*sin(radian));
	double height_rotate = fabs(width*sin(radian)) + fabs(height*cos(radian));

	//��ת���� ԭͼ�����ĵ�
	cv::Point2f center((float)width / 2.0, (float)height / 2.0);
	//��ת����  ֻ�ǽ�������ת��û���ƶ�
	Mat m1 = cv::getRotationMatrix2D(center, angle, 1.0);
	//cout << m1.size << endl;  2x3
	//m1Ϊ2��3��ͨ����Ϊ1�ľ���
	//�任��������ĵ��൱��ƽ��һ�� ԭͼ������ĵ�����ͼ������ĵ�����λ�ã�ԭͼ�����ĵ���Ҫƽ�Ƶ����� ���ĵ㣨x1��y1����x��y��  x1-x ����y1-y
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
