#include<iostream>
#include<opencv2\opencv.hpp>
using namespace cv;
using namespace std;

class ObjectTarget
{
public:
	Mat targetObject(Mat& image)
	{
		Mat dst, linesImage, gray_image;
		vector<vector<Point>> contours;
		vector<Point> contour;
		vector<Vec4f> line;
		image.copyTo(dst);
		cvtColor(image, gray_image, CV_BGR2GRAY);
		threshold(gray_image, gray_image, 21, 255, THRESH_BINARY + THRESH_OTSU);
		findContours(gray_image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		contour = operateRectangular(contours, dst);
		line = fitLines(contour);
		linesImage = drawLines(dst, line);
		drawRectangle(dst,linesImage);
		return dst;
	}

	/* 拟合轮廓直线 */
	vector<Vec4f> fitLines(vector<Point> contour)
	{
		vector<Point> contour1, contour2, contour3, contour4;
		Vec4f line1, line2, line3, line4;
		vector<Vec4f> P;
		for (int i = 0; i < contour.size(); i++)
		{
			if (contour[i].y < 1050 && contour[i].x>1600 && contour[i].x < 5000)
			{
				contour1.push_back(contour[i]);//上
			}
			if (contour[i].y > 3000 && contour[i].x>1600 && contour[i].x < 5000)
			{
				contour2.push_back(contour[i]);//下
			}
			if (contour[i].x < 1500 && contour[i].y>1100 && contour[i].y < 3000)
			{
				contour3.push_back(contour[i]);//左
			}
			if (contour[i].x > 5000 && contour[i].y>1100 && contour[i].y < 3000)
			{
				contour4.push_back(contour[i]);//右
			}

		}
	
		fitLine(contour1, line1, CV_DIST_WELSCH, 0, 0.01, 0.01);
		fitLine(contour2, line2, CV_DIST_WELSCH, 0, 0.01, 0.01);
		fitLine(contour3, line3, CV_DIST_WELSCH, 0, 0.01, 0.01);
		fitLine(contour4, line4, CV_DIST_WELSCH, 0, 0.01, 0.01);

		P.push_back(line1);
		P.push_back(line2);
		P.push_back(line3);
		P.push_back(line4);

		return P;
	}

	/* 根据fitLines返回的点数组画线图 */
	Mat drawLines(Mat& image, vector<Vec4f> paramete)
	{
		Mat dst;
		image.copyTo(dst);
		int w = dst.cols;
		for (int i = 0; i < 4; i++)
		{
			float k = paramete[i][1] / paramete[i][0];
			float x = paramete[i][2];
			float y = paramete[i][3];
			int y1 = int((-x * k) + y);
			int y2 = int((w - x) * k + y);
			line(dst, Point((w - 1), y2), Point(0, y1), Scalar(255, 0, 0), 1);
		}	
		return dst;
	}

	/*绘制四边形*/
	Mat drawRectangle(Mat& dst, Mat& image)
	{
		Mat hsvImage, mask;
		vector<Point2f> corners;
		vector<Point2f> rectVertex;
		int maxcorners = 4;
		double qualityLevel = 0.5;  //角点检测可接受的最小特征值
		double minDistance = 1000;	//角点之间最小距离
		int blockSize = 3;//计算导数自相关矩阵时指定的领域范围
		double  k = 0.04; //权重系数
		double low_H = 100, low_S = 43, low_V = 46, high_H = 124, high_S = 255, high_V = 255;
		cvtColor(image, hsvImage, CV_BGR2HSV);
		inRange(hsvImage, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask);
		goodFeaturesToTrack(mask, corners, maxcorners, qualityLevel, minDistance, Mat(), blockSize, false, k);
		rectVertex = classifyCorner(corners);
		for (int i = 0; i < rectVertex.size(); i++)
		{
			Point p1 = rectVertex[i];
			Point p2 = rectVertex[(i + 1) % 4];
			line(dst, p1, p2, Scalar(0, 0, 255), 1);
		}
		return dst;
	}

	/*角点排序*/
	vector<Point2f> classifyCorner(vector<Point2f> corners)
	{
		vector<Point2f>rectVertex;
		/* 左上 右下*/
		for (int i = 0; i < corners.size(); i++)
		{
			for (int j = 0; j < corners.size(); j++)
			{
				if (corners[i].x - corners[j].x > 2000 && corners[i].y - corners[j].y > 1000)
				{
					rectVertex.push_back(corners[i]);
					rectVertex.push_back(corners[j]);
				}
			}
		}
		/* 右上 */
		for (int j = 0; j < corners.size(); j++)
		{
			if (rectVertex[0].x - corners[j].x > 2000 && rectVertex[0].y - corners[j].y <100)
			{
				rectVertex.push_back(corners[j]);
			}
		}
		/* 左下 */
		for (int j = 0; j < corners.size(); j++)
		{
			if (rectVertex[0].x - corners[j].x <100 && rectVertex[0].y - corners[j].y >1000)
			{
				rectVertex.push_back(corners[j]);
			}
		}

		Point2f temp = rectVertex[1];
		rectVertex[1] = rectVertex[2];
		rectVertex[2] = temp;
		return rectVertex;
	}

	/*寻找符合面积范围的轮廓*/
	vector<Point> operateRectangular(const vector<vector<Point>> contours, Mat& dst)
	{
		Mat box;
		Point2f P[4];
		vector<Point> P1;
		double hminVal = 0, hmaxVal = 0;
		double wminVal = 0, wmaxVal = 0;
		/*获得轮廓点的最小旋转矩形*/
		for (int i = 0; i < contours.size(); i++)
		{
			RotatedRect r = minAreaRect(Mat(contours[i]));
			boxPoints(r, box);
			minMaxIdx(box.col(0), &hminVal, &hmaxVal);
			minMaxIdx(box.col(1), &wminVal, &wmaxVal);
			double height = hmaxVal - hminVal;
			double width = wmaxVal - wminVal;
			/*根据矩形宽高比和面积筛选矩形*/
			if ((float)width * height >= 600000 && (float )width * height <= 10000000)
			{
				P1 = contours[i];
			}
		}
		return P1;
	}
};

int main()
{
	Mat image = imread("lcd.bmp");
	Mat dst;
	ObjectTarget test;
	dst = test.targetObject(image);
	imwrite("result.jpg", dst);
	return 0;
}

