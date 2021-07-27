#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
Mat ImgRotate(Mat matSrc, float angle);
int main()
{
	float angle;
	cin >> angle;
	Mat image = imread("lena.jpg");
	Mat imageout = ImgRotate(image, angle);
	imwrite("result.jpg", imageout);
	namedWindow("test", CV_WINDOW_NORMAL);
	imshow("test", imageout);
	waitKey(0);
	getchar();
	return 0;
}

Mat ImgRotate(Mat matSrc, float angle)
{
	double input_theta = angle * CV_PI / 180.0;
	int src_rows = matSrc.rows;//225
	int src_cols = matSrc.cols;//400
	/* 创建中点坐标 */
	int center_x = src_cols / 2;  //200
	int center_y = src_rows / 2;  //112
	int output_x1 = 0, output_y1 = 0;
	double cos_theta = cos(input_theta);
	double sin_theta = sin(input_theta);
	Mat matRet(src_rows, src_cols, matSrc.type(), Scalar(0));
	for (int c = 0; c < src_cols; c++)//列
	{
		for (int r = 0; r < src_rows; r++)//行
		{
			double output_x0 = cos_theta*c + sin_theta*r + (1 - cos_theta)*center_x - sin_theta*center_y;//x为列坐标
			double output_y0 = -sin_theta*c + cos_theta*r + (1 - cos_theta)*center_y + sin_theta*center_x;//y为行坐标
			output_x1 = (int)output_x0, output_y1 = (int)output_y0;
			if (output_x1 >= 0 && output_x1 < src_cols && output_y1 >= 0 && output_y1 < src_rows)
			{
				matRet.at<Vec3b>(output_y1, output_x1) = matSrc.at<Vec3b>(r, c);
			}
		}
	}
	return matRet;
}
