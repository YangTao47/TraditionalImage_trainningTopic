#include<iostream>
#include<opencv2\opencv.hpp>
#include<chrono>
using namespace cv;
using namespace std;

typedef struct RoiInfo
{
	Mat RoiRect;
	Point RoiLeftUpPoint;
}Roi;

class ObjectTarget
{
public:
	Mat targetImage(const Mat& src)
	{
		Mat roi, dst;
		vector<Point> cross_points(4);
		findRoi(src, roiInfo);
		getCornerPoints(roiInfo.RoiRect, src, cross_points);
		dst = drawLines(src, cross_points);
		return dst;
	}
private:
	Roi roiInfo;
	const int error = 20;
	const float smallScale = 0.1f;
	const int bigScale = 10;
	const int thresholdValue = 254;
	const int area = 50;

	void getCornerPoints(const Mat& roi, const Mat& src, vector<Point>& cross_points)
	{
		vector<vector<Point>> two_point_of_lines(4);
		vector<vector<Point>> contour_points(4);
		vector<Vec4d> lines(4);
		Mat image;
		src.copyTo(image);
		findContoursPoints(roi, contour_points);
		fitLines(contour_points, lines);
		getTwoPointsOfLines(src, two_point_of_lines, lines);
		calculateCrossPoints(cross_points, two_point_of_lines);
	}

	void findRoi(const Mat& src, Roi& roiInfo)
	{
		Mat binary_image, labels, stats, centroids, dst, rvs_Image, small_img;
		vector<int> roi;
		resize(src, small_img, Size(int(src.cols * smallScale), int(src.rows * smallScale)));
		threshold(small_img, binary_image, 1, 255, THRESH_BINARY + THRESH_OTSU);
		rvs_Image = reverseImage(binary_image);
		findBigestConnectedComponentsArea(rvs_Image, roi);

		int x = bigScale * roi[0] - error;
		int y = bigScale * roi[1] - error;
		int width = bigScale * roi[2] + (2 * error);
		int height = bigScale * roi[3] + (2 * error);

		Rect roi_rect(x, y, width, height);
		roiInfo.RoiLeftUpPoint.x = x;
		roiInfo.RoiLeftUpPoint.y = y;
		roiInfo.RoiRect = src(roi_rect).clone();
	}

	void findContoursPoints(const Mat& roi, vector<vector<Point>>& contour_points)
	{
		Mat binary_image, denoise_image;
		vector<vector<Point>> P;
		vector<Point> up_line_points, right_line_points, down_line_points, left_line_point;
		threshold(roi, binary_image, 1, 255, THRESH_BINARY + THRESH_OTSU);
		denoise_image = removeNoise(binary_image);

		findJumpPoint(denoise_image, up_line_points, "up");
		findJumpPoint(denoise_image, right_line_points, "right");
		findJumpPoint(denoise_image, down_line_points, "down");
		findJumpPoint(denoise_image, left_line_point, "left");

		contour_points[0] = up_line_points;
		contour_points[1] = right_line_points;
		contour_points[2] = down_line_points;
		contour_points[3] = left_line_point;
	}

	void fitLines(const vector<vector<Point>>& contour_points, vector<Vec4d>& lines)
	{
		Vec4d up_line, right_line, down_line, left_line;
		fitLine(contour_points[0], up_line, CV_DIST_WELSCH, 0, 0.01, 0.01);
		fitLine(contour_points[1], right_line, CV_DIST_WELSCH, 0, 0.01, 0.01);
		fitLine(contour_points[2], down_line, CV_DIST_WELSCH, 0, 0.01, 0.01);
		fitLine(contour_points[3], left_line, CV_DIST_WELSCH, 0, 0.01, 0.01);
		
		lines[0] = up_line;
		lines[1] = right_line;
		lines[2] = down_line;
		lines[3] = left_line;
	}

	void  getTwoPointsOfLines(const Mat& src, vector<vector<Point>>& two_point_of_lines, const vector<Vec4d> lines)
	{
		int w = src.cols;
		int h = src.rows;
		/*x=0 , x =src.cols*/
		Mat image;
		src.copyTo(image);
		cvtColor(image, image, CV_GRAY2BGR);
		for (int i = 0; i < lines.size(); i++)
		{
			vector<Point> temp_point_vector(2);
			double x0 = lines[i][2], y0 = lines[i][3];
			double k = lines[i][1] / lines[i][0];
			if (i == 0 || i == 2)
			{
				int y1 = int(-x0 * k + y0);
				int y2 = int((w - x0) * k + y0);

				Point temp_point1(0, y1);
				Point temp_point2((w - 1), y2);
				temp_point_vector[0] = temp_point1;
				temp_point_vector[1] = temp_point2;
			}
			else
			{
				int x1 = int((x0 * k - y0 ) / k);
				int x2 = int((x0 * k - y0 + h) / k);
				Point temp_point1(x1, 0);
				Point temp_point2(x2, h);
				temp_point_vector[0] = temp_point1;
				temp_point_vector[1] = temp_point2;
			}
			two_point_of_lines[i] = temp_point_vector;
		}

		for (int i = 0; i < 4; i++)
		{
			circle(image, two_point_of_lines[i][0], 20, Scalar(0, 0, 255), -1);
			circle(image, two_point_of_lines[i][1], 20, Scalar(0, 0, 255), -1);
		}
	}


	void calculateCrossPoints(vector<Point>& cross_points, const vector<vector<Point>>& two_point_of_lines)
	{
		for (int i = 0; i < two_point_of_lines.size(); i++)
		{

			int x00 = two_point_of_lines[i][1].x;
			int y00 = two_point_of_lines[i][1].y;
			int x01 = two_point_of_lines[i][0].x;
			int y01 = two_point_of_lines[i][0].y;

			float a1 = float(y01 - y00);
			float b1 = float(x00 - x01);
			float c1 = float(a1 * x00 + b1 * y00);

			int x10 = two_point_of_lines[(i + 1) % 4][1].x;
			int y10 = two_point_of_lines[(i + 1) % 4][1].y;
			int x11 = two_point_of_lines[(i + 1) % 4][0].x;
			int y11 = two_point_of_lines[(i + 1) % 4][0].y;


			float a2 = float(y11 - y10);
			float b2 = float(x10 - x11);
			float c2 = float(a2 * x10 + b2 * y10);

			float denominator = a1 * b2 - a2 * b1;
			float x = float(b2 * c1 - b1 * c2) / (float)denominator;
			float y = float(a1 * c2 - a2 * c1) / (float)denominator;

			Point temp_point;
			temp_point.x = (int)x;
			temp_point.y = (int)y;
			cross_points[i] = temp_point;
		}
	}

	/*返回x y w h*  去噪*/
	void findNoiseConnectedComponentsArea(const Mat& src, const int area, vector<vector<int>>& roi)
	{
		Mat labels, stats, centroids;
		int num = connectedComponentsWithStats(src, labels, stats, centroids, 4);
		for (int i = 1; i < num; i++)
		{
			vector<int> candidateArea;
			if (stats.at<int>(i, CC_STAT_AREA) < area)
			{
				for (int cols = 0; cols < 4; cols++)
				{
					candidateArea.push_back(stats.at<int>(i, cols));
				}
				roi.push_back(candidateArea);
			}
		}
	}

	void findBigestConnectedComponentsArea(const Mat& small_image, vector<int>& roi)
	{
		Mat labels, stats, centroids;
		int num = connectedComponentsWithStats(small_image, labels, stats, centroids, 4);
		int temp = 0;
		int id = 0;
		for (int i = 1; i < num; i++)
		{
			if (stats.at<int>(i, CC_STAT_AREA) > temp)
			{
				temp = stats.at<int>(i, CC_STAT_AREA);
				id = i;
			}
		}
		for (int cols = 0; cols < 4; cols++)
		{
			roi.push_back(stats.at<int>(id, cols));
		}
	}

	Mat removeNoise(Mat& roi_binary)
	{
		vector<vector<int>> roi;
		findNoiseConnectedComponentsArea(roi_binary, area, roi);
		for (int i = 0; i < roi.size(); i++)
		{
			int x = roi[i][0];
			int y = roi[i][1];
			int width = roi[i][2];
			int height = roi[i][3];
			for (int c = x; c < x + width; c++)
			{
				for (int r = y; r < y + height; r++)
				{
					roi_binary.at<uchar>(r, c) = 0;
				}
			}
		}
		return roi_binary;
	}

	Mat reverseImage(const Mat& src)
	{
		return 255 - src;
	}

	Mat drawLines(const Mat& src, const vector<Point>& p)
	{
		Mat dst;
		src.copyTo(dst);
		if (dst.channels() == 1) {
			cvtColor(dst, dst, CV_GRAY2BGR);
		}
		for (int i = 0; i < p.size(); i++)
		{
			line(dst, p[i], p[(i + 1) % 4], Scalar(0, 0, 255), 1);
		}
		return dst;
	}

	void findJumpPoint(const Mat& src, vector<Point>& jump_point, string mode = "up")
	{
		int center_x = src.cols / 2;
		int center_y = src.rows / 2;
		int src_rows = src.rows;
		int src_cols = src.cols;
		int pixelNums = (int)min(src.rows / 3, src.cols / 3);
		//cout << pixelNums << endl;
		CV_Assert(src.at<uchar>(center_y, center_x) == 0);
		CV_Assert(center_x + pixelNums < src.cols);
		CV_Assert(center_x - pixelNums > 0);
		CV_Assert(center_y + pixelNums < src.rows);
		CV_Assert(center_y - pixelNums > 0);
		if (src.at<uchar>(center_y, center_x) == 0)
		{
			if (mode == "up")
			{
				//cout << "up" << endl;
				for (int num = 0; num < pixelNums; num++)
				{
					Point temp_point;
					for (int i = 0; i < src_rows / 2; i++)
					{
						int p1 = src.at<uchar>(center_y - i, center_x);
						int p2 = src.at<uchar>(center_y - i - 1, center_x);
						if (p2 - p1 > thresholdValue)
						{
							offsetPoint(temp_point, jump_point, i, center_x, center_y - i - 1);
						}
					}
					center_x++;
				}
				for (int num = 0; num < pixelNums; num++)
				{
					Point temp_point;
					for (int i = 0; i < src_rows / 2; i++)
					{
						int p1 = src.at<uchar>(center_y - i, center_x);
						int p2 = src.at<uchar>(center_y - i - 1, center_x);
						if (p2 - p1 > thresholdValue)
						{
							offsetPoint(temp_point, jump_point, i, center_x, center_y - i - 1);
						}
					}
					center_x--;
				}
			}

			if (mode == "down")
			{
				//cout << "down" << endl;
				for (int num = 0; num < pixelNums; num++)
				{
					Point temp_point;
					for (int i = 0; i < src_rows / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y + i, center_x);
						int p2 = src.at<uchar>(center_y + i + 1, center_x);
						if (p2 - p1 > thresholdValue)
						{
							offsetPoint(temp_point, jump_point, i, center_x, center_y + i + 1);
						}
					}
					center_x++;
				}

				for (int num = 0; num < pixelNums; num++)
				{
					Point temp_point;
					for (int i = 0; i < src_rows / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y + i, center_x);
						int p2 = src.at<uchar>(center_y + i + 1, center_x);
						if (p2 - p1 > thresholdValue)
						{
							offsetPoint(temp_point, jump_point, i, center_x, center_y + i + 1);
						}
					}
					center_x--;
				}
			}

			if (mode == "left")
			{
				//cout << "left" << endl;
				for (int num = 0; num < pixelNums; num++)
				{
					Point temp_point;
					for (int i = 0; i < src_cols / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y, center_x - i);
						int p2 = src.at<uchar>(center_y, center_x - i - 1);
						if (p2 - p1 > thresholdValue)
						{
							offsetPoint(temp_point, jump_point, i, center_x - i - 1, center_y);
						}
					}
					center_y++;
				}
				for (int num = 0; num < pixelNums; num++)
				{
					Point temp_point;
					for (int i = 0; i < src_cols / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y, center_x - i);
						int p2 = src.at<uchar>(center_y, center_x - i - 1);
						if (p2 - p1 > thresholdValue)
						{
							offsetPoint(temp_point, jump_point, i, center_x - i - 1, center_y);
						}
					}
					center_y--;
				}
			}

			if (mode == "right")
			{
				//cout << "right" << endl;
				for (int num = 0; num < pixelNums; num++)
				{
					Point temp_point;
					for (int i = 0; i < src_cols / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y, center_x + i);
						int p2 = src.at<uchar>(center_y, center_x + i + 1);
						if (p2 - p1 > thresholdValue)
						{
							offsetPoint(temp_point, jump_point, i, center_x + i + 1, center_y);
						}
					}
					center_y++;
				}
				for (int num = 0; num < pixelNums; num++)
				{
					Point temp_point;
					for (int i = 0; i < src_cols / 2 - 1; i++)
					{
						int p1 = src.at<uchar>(center_y, center_x + i);
						int p2 = src.at<uchar>(center_y, center_x + i + 1);
						if (p2 - p1 > thresholdValue)
						{
							offsetPoint(temp_point, jump_point, i, center_x + i + 1, center_y);
						}
					}
					center_y--;
				}
			}
		}
	}

	void offsetPoint(Point& temp_point, vector<Point>& jump_point, int i, int x, int y)
	{
		temp_point.x = x + roiInfo.RoiLeftUpPoint.x;
		temp_point.y = y + roiInfo.RoiLeftUpPoint.y;
		jump_point.push_back(temp_point);
	}
};

int main()
{
	Mat image = imread("lcd.bmp", CV_8UC1);
	Mat dst;
	ObjectTarget test;
	auto start_time = chrono::high_resolution_clock::now();

	dst = test.targetImage(image);

	chrono::time_point<chrono::high_resolution_clock> end_time = chrono::high_resolution_clock::now();
	chrono::milliseconds processing_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
	cout << processing_time.count() << endl;
	imwrite("result.jpg", dst);
	return 0;
}
