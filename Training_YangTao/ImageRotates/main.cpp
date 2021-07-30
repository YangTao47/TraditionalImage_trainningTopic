#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
Mat ImgRotate(Mat original, float angle);
vector<Point> rotatePixels(const int r, const int c, const double input_theta, const Point center_point);
void pixelAssignment(Mat& matRet, Mat& original, vector<Point>p, const int r, const int c);

int main()
{
	float angle;
	cin >> angle;
	Mat image;
	image = imread("lena.jpg");
	Mat imageout;
	imageout = ImgRotate(image, angle);
	imwrite("result.jpg", imageout);
	return 0;
}

Mat ImgRotate(Mat original, float angle)
{
	vector<Point>p;
	Point center_point;
	double input_theta = angle * CV_PI / 180.0;
	int src_rows = original.rows;//225
	int src_cols = original.cols;//400
	center_point.x = src_cols / 2;
	center_point.y = src_rows / 2;
	Mat matRet(src_rows, src_cols, original.type(), Scalar(0));	
	/* 旋转后的图到原图的顺时针投影 */
	for (int c = 0; c < src_cols; c++)
	{
		for (int r = 0; r < src_rows; r++)
		{
			p = rotatePixels(r, c, input_theta, center_point);
			if (p[1].x >= 0 && p[1].x < 400 && p[0].x >= 0 && p[0].x < 400 
				&& p[1].y >= 0 && p[1].y < 225 && p[0].y >= 0 && p[0].y < 225)
			{
				pixelAssignment(matRet, original, p, r, c);
			}
		}
	}
	return matRet;
}

vector<Point> rotatePixels(const int r, const int c, const double input_theta,const Point center_point)
{
	vector<Point>p;
	Point2i point_up, point_low;
	Point2d a;
	double cos_theta = cos(input_theta);
	double sin_theta = sin(input_theta);
	double output_x0 = cos_theta*c + -sin_theta*r + (1 - cos_theta)*center_point.x + sin_theta*center_point.y;
	double output_y0 = sin_theta*c + cos_theta*r + (1 - cos_theta)*center_point.y - sin_theta*center_point.x;
	point_low.x = int(floor(output_x0));
	point_up.x = int(ceil(output_x0));
	point_low.y = int(floor(output_y0));
	point_up.y = int(ceil(output_y0));
	a.x = output_x0 - point_low.x;
	a.y = output_y0 - point_low.y;
	p.push_back(point_up);
	p.push_back(point_low);
	p.push_back(a);
	return p;

}

void pixelAssignment(Mat& matRet, Mat& original, vector<Point>p, const int r, const int c) 
{
	matRet.at<Vec3b>(r, c) = p[2].y*(p[2].x*original.at<Vec3b>(p[0].y, p[0].x) + (1 - p[2].x)*original.at<Vec3b>(p[0].y, p[1].x))
		+ (1 - p[2].y)*(p[2].x*original.at<Vec3b>(p[1].y, p[0].x) + (1 - p[2].x)*original.at<Vec3b>(p[1].y, p[1].x));
}