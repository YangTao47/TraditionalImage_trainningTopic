#include<iostream>
#include<opencv2\opencv.hpp>
using namespace cv;
using namespace std;

class DrawCircle
{
public:
	void drawPixel(Mat& image, int x, int y)
	{
		image.at<Vec3b>(x, y) = { 255,255,255 };
	}

	void drawPoint(Mat& image, int x0, int y0, int x, int y)
	{
		/* 四个对称点对 */
		drawPixel(image, x0 + x, y0 + y);
		drawPixel(image, x0 + y, y0 + x);
		drawPixel(image, x0 - x, y0 + y);
		drawPixel(image, x0 - y, y0 + x);
		drawPixel(image, x0 + x, y0 - y);
		drawPixel(image, x0 + y, y0 - x);
		drawPixel(image, x0 - x, y0 - y);
		drawPixel(image, x0 - y, y0 - x);
	}
	void drawCircle(Mat& image, int x0, int y0, int r)
	{
		int x = 0, y = r, d = 1 - r;
		drawPoint(image, x0, y0, x, y);
		while (x <= y)
		{
			if (d < 0)
			{
				d += 2 * x + 3;
			}
			else
			{
				d += 2 * (x - y) + 5;
				y--;
			}
			x++;
			drawPoint(image, x0, y0, x, y);
		}
	}

	Mat middlePointCircle(Mat& image, int x0, int y0, int r)
	{
		Mat result;
		if (x0 > image.cols || y0 > image.rows)
		{
			cout << "圆心超出范围请重新输入 " << endl;
		}
		else if (y0 - r > 0 && y0 + r < image.rows
			     && x0 - r > 0 && x0 + r <image.cols)
		{
			drawCircle(image, x0, y0, r);
			threshold(image, result, 220, 255, THRESH_BINARY_INV);
			floodFill(result, Point(0, 0), Scalar(0, 0, 0));
		}
		else
		{
			cout << "非法操作：圆已跨界，请在范围内画圆 " << endl;
		}
		return result;
	}
};

int main()
{
	const int kRectangleX = 800, kRectangleY = 600;
	int radius = 0;
	DrawCircle drawcicle;
	Mat image(kRectangleY, kRectangleX, CV_8UC3, Scalar(0, 0, 0));
	Mat result;
	Point center_point;
	cin >> center_point.x >> center_point.y >> radius;
	result = drawcicle.middlePointCircle(image, center_point.x, center_point.y, radius);
	imwrite("result.jpg", result);
	return 0;
}
