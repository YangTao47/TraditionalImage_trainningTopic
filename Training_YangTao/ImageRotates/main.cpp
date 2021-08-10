#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

class RotateImage
{
public:

	Mat rotateImage(Mat& src, Mat& dst, float angle)
	{
		Mat result;
		result = rotatePixels(src, dst, angle);
		return result;
	}
	
private:

	Mat rotatePixels(Mat& src, Mat& dst, float angle)
	{
		Mat rotation_matrix;
		rotation_matrix = calculateRotationMatrix(src, angle);
		for (int c = 0; c < dst.cols; c++)
		{
			for (int r = 0; r < dst.rows; r++)
			{
				swapPixelValues(dst, src, r, c, rotation_matrix);
			}
		}
		return dst;
	}

	Mat calculateRotationMatrix(Mat& src, float angle)
	{
		double input_theta = angle * CV_PI / 180.0;
		float cos_theta = (float)cos(input_theta);
		float sin_theta = (float)sin(input_theta);
		int center_point_x = src.cols / 2;
		int center_point_y = src.rows / 2;
		Mat rotation_matrix = (Mat_<float>(3, 3) << cos_theta, -sin_theta, (1 - cos_theta) * center_point_x + sin_theta * center_point_y,
												    sin_theta, cos_theta, (1 - cos_theta) * center_point_y - sin_theta * center_point_x,
													0,        0,          1);
		return rotation_matrix;
	}

	void swapPixelValues(Mat& dst, Mat& src, int dst_r, int dst_c, Mat& rotation_matrix)
	{
		float output_x0 = 0, output_y0 = 0;
		Mat dst_matrix = (Mat_<float>(3, 1) << dst_c, dst_r, 1);
		Mat src_matrix = (Mat_<float>(3, 1) << output_x0, output_y0, 1);
		src_matrix = rotation_matrix * dst_matrix;

		float src_x = src_matrix.at<float>(0, 0);
		float src_y = src_matrix.at<float>(1, 0);

		int point_low_x = int(floor(src_x));
		int point_up_x= int(ceil(src_x));
		int point_low_y = int(floor(src_y));
		int point_up_y= int(ceil(src_y));

		float a = abs(src_x - point_low_x);
		float b = abs(src_y - point_low_y);

		int src_rows = src.rows;
		int src_cols = src.cols;

		/* 限制旋转点不超出原图范围  双线性插值得到像素值 */
		if (point_low_x > 0 && point_low_x < src_cols && point_up_x > 0 && point_up_x < src_cols
			&& point_low_y > 0 && point_low_y < src_rows && point_up_y >= 0 && point_up_y < src_rows)
		{	
			dst.at<Vec3b>(dst_r, dst_c) = b * (a * src.at<Vec3b>(point_up_y, point_up_x) + (1 - a) * src.at<Vec3b>(point_up_y, point_low_x))
				+ (1 - b) * (a * src.at<Vec3b>(point_low_y, point_up_x) + (1 - a) * src.at<Vec3b>(point_low_y, point_low_x));
		}
	}
};

int main()
{
	RotateImage rotation;
	Mat src;
	float angle;
	cin >> angle;
	src = imread("R-C.jpg");
	Mat dst(src.rows, src.cols, src.type(), Scalar(0));
	dst = rotation.rotateImage(src, dst, angle);
	imwrite("result.jpg", dst);
	return 0;
}
