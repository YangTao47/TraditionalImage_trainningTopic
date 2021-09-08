#include<opencv2/opencv.hpp>
#include<iostream>
#include<chrono>
using namespace std;
using namespace cv;

class RotateImage
{
public:

	void rotatePixels(const Mat& src, Mat& dst, float angle)
	{
		Mat rotation_matrix;
		rotation_matrix = calculateRotationMatrix(src, angle);
		int dst_cols = dst.cols;
		int dst_rows = dst.rows;
		for (int c = 0; c < dst_cols; c++)
		{
			for (int r = 0; r < dst_rows; r++)
			{
				calculatePixelsWeightingValue(dst, src, r, c, rotation_matrix);
			}
		}
	}

private:

	Mat calculateRotationMatrix(const Mat& src, float angle)
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

	void calculatePixelsWeightingValue(Mat& dst, const Mat& src, int dst_r, int dst_c, const Mat& rotation_matrix)
	{
		//Mat dst_matrix = (Mat_<float>(3, 1) << dst_c, dst_r, 1);

		Mat dst_matrix(3, 1, CV_32FC1);
		Point point_low, point_up, point_xup_ylow, point_yup_xlow;

		dst_matrix.at<float>(0, 0) = (float)dst_c;
		dst_matrix.at<float>(1, 0) = (float)dst_r;
		dst_matrix.at<float>(2, 0) = (float)1;

		Mat src_matrix;
		src_matrix = rotation_matrix * dst_matrix;

		float src_x = src_matrix.at<float>(0, 0);
		float src_y = src_matrix.at<float>(1, 0);

		point_low.x  = int(floor(src_x));
		point_low.y = int(floor(src_y));

		point_up.x = int(ceil(src_x));
		point_up.y = int(ceil(src_y));

		point_xup_ylow.x = point_up.x;
		point_xup_ylow.y = point_low.y;

		point_yup_xlow.x = point_low.x;
		point_yup_xlow.y = point_up.y;

		float diff_x = abs(src_x - point_low.x);
		float diff_y = abs(src_y - point_low.y);

		int src_rows = src.rows;
		int src_cols = src.cols;

		Rect roi(0, 0, src.cols, src.rows);
		/* 限制旋转点不超出原图范围  双线性插值得到像素值 */
		if (roi.contains(point_low) && roi.contains(point_up) && roi.contains(point_xup_ylow) && roi.contains(point_yup_xlow))
		{	
			Vec3b p1 = src.at<Vec3b>(point_up.y, point_up.x);
			Vec3b p2 = src.at<Vec3b>(point_up.y, point_low.x);
			Vec3b p3 = src.at<Vec3b>(point_low.y, point_up.x);
			Vec3b p4 = src.at<Vec3b>(point_low.y, point_low.x);

			Vec3b part1 = diff_y * (diff_x * p1 + (1 - diff_x) * p2);
			Vec3b part2 = (1 - diff_y) * (diff_x * p3 + (1 - diff_x) * p4);

			dst.at<Vec3b>(dst_r, dst_c) = part1 + part2;
		}
	}
};

void rotate_arbitrarily_angle(Mat &src, Mat &dst, float angle)
{
	float radian = (float)(angle / 180.0 * CV_PI);

	//填充图像
	int maxBorder = (int)(max(src.cols, src.rows)* 1.414); //即为sqrt(2)*max
	int dx = (maxBorder - src.cols) / 2;
	int dy = (maxBorder - src.rows) / 2;
	copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);

	//旋转
	Point2f center((float)(dst.cols / 2), (float)(dst.rows / 2));
	Mat affine_matrix = getRotationMatrix2D(center, angle, 1.0);//求得旋转矩阵
	warpAffine(dst, dst, affine_matrix, dst.size());

	//计算图像旋转之后包含图像的最大的矩形
	float sinVal = abs(sin(radian));
	float cosVal = abs(cos(radian));
	Size targetSize((int)(src.cols * cosVal + src.rows * sinVal),
		(int)(src.cols * sinVal + src.rows * cosVal));

	//剪掉多余边框
	int x = (dst.cols - targetSize.width) / 2;
	int y = (dst.rows - targetSize.height) / 2;
	Rect rect(x, y, targetSize.width, targetSize.height);
	dst = Mat(dst, rect);
}


int main()
{
	RotateImage rotation;
	Mat src = imread("R-C.jpg");
	float angle;
	cin >> angle;
	
	Mat dst(src.rows, src.cols, src.type(), Scalar(0));

	auto start_time = chrono::high_resolution_clock::now();
	
	rotation.rotatePixels(src, dst, angle);
	//rotate_arbitrarily_angle(src, dst, angle);
	chrono::time_point<chrono::high_resolution_clock> end_time = chrono::high_resolution_clock::now();
	chrono::milliseconds processing_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
	cout << processing_time.count() << endl;

	imwrite("result.jpg", dst);
	system("pause");
	return 0;
}
