#include<iostream>
#include<opencv2\opencv.hpp>
using namespace cv;
using namespace std;

class DrawCircle
{
public:
	void drawPixel(Mat &image, int x, int y)
	{
		image.at<Vec3b>(x, y) = { 255,255,255 };
	}

	void drawPoint(Mat &image, int x0, int y0, int x, int y)
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

	void middlePiontCircle(Mat &image, int x0, int y0, int r,
						   const int &RectangleX, const int &RectangleY)
	{
		int x = 0;
		int y = r;
		int d = 1 - r;//设置初始值
		if (x0 > RectangleY || y0 > RectangleX)
		{
			cout << "圆心超出范围请重新输入";
		}
		else if (y0 - r > 0 && y0 + r < RectangleY
			     && x0 - r > 0 && x0 + r < RectangleX)
		{
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
			}/* end of while (x <= y) */
			/* 阈值分割 做一个取反操作 */
			Mat result;
			threshold(image, result, 220, 255, THRESH_BINARY_INV);
			floodFill(result, Point(0, 0), Scalar(0, 0, 0));
			imwrite("result.jpg", result);
			imshow("output", result);
			waitKey(0);
		}/* end of else if (y0 - r > 0 && y0 + r < rectangle_y 
							&& x0 - r > 0 && x0 + r < rectangle_x) */
		else
		{
			cout << "非法操作：圆已跨界，请在范围内画圆";
		}
	}
};

int main()
{
	const int kRectangleX = 800, kRectangleY = 600;
	Mat image(kRectangleY, kRectangleX, CV_8UC3, Scalar(0, 0, 0));
	int x = 0, y = 0, r = 0;
	cin >> x >> y >> r;
	DrawCircle drawcicle;
	drawcicle.middlePiontCircle(image, x, y, r, kRectangleX, kRectangleY);
	system("pause");
	return 0;
}
