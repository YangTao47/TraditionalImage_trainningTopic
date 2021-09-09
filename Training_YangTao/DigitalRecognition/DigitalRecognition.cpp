#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
#include"DigitalRecognition.h"
using namespace cv;
using namespace std;

vector<CellBoardCoordinates> DigitalRecognition::extractCellCoordinates(const Mat& Image)
{
	CellBoardCoordinates cell_board_point;
	vector<CellBoardCoordinates> cell_board_point_vector;
	vector<vector<Point>> grid_corner;
	Point cell_point;
	Point board_point;
	findCellBox(Image, grid_corner);
	for (int i = 0; i < grid_corner.size(); i++)
	{
		cell_point = getBoardCoordinates(Image, grid_corner, i);
		board_point = grid_corner[i][0];
		cell_board_point.cellCoordinates = cell_point;
		cell_board_point.boardCoordinates = board_point;
		cell_board_point_vector.push_back(cell_board_point);
	}
	return cell_board_point_vector;
}

void DigitalRecognition::findCellBox(const Mat& src, vector<vector<Point>>& grid_corner)
{
	Mat gray_image, binary_image, reverse_image, denoise_image, scored_map, small_image;
	vector<Point> cross_point;
	CV_Assert(src.channels() == 1);
	medianBlur(src, gray_image, 13);
	adaptiveThreshold(gray_image, binary_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 1333, 1);
	reverseImage(binary_image, reverse_image);
	copyMakeBorder(reverse_image, reverse_image, expandPixels, expandPixels, expandPixels, expandPixels, BORDER_REPLICATE);
	copyMakeBorder(binary_image, binary_image, expandPixels, expandPixels, expandPixels, expandPixels, BORDER_REPLICATE);

	denoiseImage(reverse_image, binary_image, denoise_image);
	resize(denoise_image, small_image, Size(int(smallScale * denoise_image.cols), int(smallScale * denoise_image.rows)));
	threshold(small_image, small_image, 1, 255, THRESH_BINARY + THRESH_OTSU);

	scored_map = computeScoreMap(small_image, Size(roiScale, roiScale / 2), Size(roiScale / 2, roiScale));
	resize(scored_map, scored_map, Size(int(bigScale * scored_map.cols), int(bigScale * scored_map.rows)));
	computeCrossPoint(scored_map, cross_point);
	sortCorners(cross_point, grid_corner, gray_image);

	/*for (int i = 0; i < grid_corner.size(); i++) {
		circle(gray_image, grid_corner[i][0], 10, Scalar(255), 5);
	}*/
}

/*黑底白噪图像，默认黑色为背景色 */
void DigitalRecognition::findNoiseMask(const Mat& src, int area, Mat& mask)
{
	Mat labels, stats, centroids;
	CV_Assert(src.channels() == 1);
	int num = connectedComponentsWithStats(src, labels, stats, centroids, 4);
	vector<int> p(num);
	for (int m = 0; m < num; m++) {
		if (stats.at<int>(m, CC_STAT_AREA) < area) {
			p[m] = 255;
		}
		else{
			p[m] = 0;
		}
	}
	for (int y = 0; y < mask.rows; y++)
	{
		for (int x = 0; x < mask.cols; x++)
		{
			int label = labels.at<int>(y, x);//注意labels是int型，不是uchar
			mask.at<uchar>(y, x) = p[label];
		}
	}
}

void DigitalRecognition::reverseImage(const Mat& src, Mat& dst)
{
	dst = 255 - src;
}

void DigitalRecognition::denoiseImage(const Mat& reverse_image, const Mat& binary_image, Mat& denoise_image)
{
	Mat mask(reverse_image.rows, reverse_image.cols, CV_8UC1);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	findNoiseMask(reverse_image, connectedComponentsArea1, mask);
	Mat mv_black_noise = binary_image + mask;
	morphologyEx(mv_black_noise, mv_black_noise, MORPH_CLOSE, element, Point(-1, -1), 10);

	findNoiseMask(mv_black_noise, connectedComponentsArea2, mask);
	Mat mv_white_noise = mv_black_noise - mask;
	morphologyEx(mv_white_noise, denoise_image, MORPH_CLOSE, element, Point(-1, -1), 5);
}

void DigitalRecognition::computeCrossPoint(const Mat& src, vector<Point>& cross_points)
{
	Mat canny_image;
	int rows = src.rows;
	int cols = src.cols;
	vector<vector<Point>> contours;
	threshold(src, src, 200, 255, THRESH_BINARY);
	/****************************输入Canny图寻找轮廓，找到轮廓最小外接圆中心（粗定位不返回原图做）*******************************************/
	Canny(src, canny_image, 1, 3);
	findContours(canny_image, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++) {
		Point2f temp_center_point;
		float temp_offset = (float)expandPixels;
		float temp_radiu;
		minEnclosingCircle(contours[i], temp_center_point, temp_radiu);
		// 将原点移回原图左上角
		cross_points.push_back(temp_center_point - Point2f(temp_offset, temp_offset));
	}
	/***********************方法二点集分类，boundingRect寻找左上角，在原图中找到ROI区域（精定位） ***************************************/
	/*for (auto& i : contours) {
		Rect temp_rect = boundingRect(i);
		Point temp_point(temp_rect.x, temp_rect.y);
	}*/
}

Mat DigitalRecognition::computeScoreMap(Mat& gray_image, Size& ksize1, Size& ksize2)
{
	Mat cache_mat1, cache_mat2, dst;
	gray_image.convertTo(gray_image, CV_32F);
	blur(gray_image, cache_mat1, ksize1);
	blur(gray_image, cache_mat2, ksize2);

	replacePixels(cache_mat1);
	replacePixels(cache_mat2, false);
	dst = cache_mat1 & cache_mat2;
	//Mat test = cache_mat1 + cache_mat2;
	return dst;
}

/* 图片不能过度旋转 */
void DigitalRecognition::replacePixels(Mat& image, bool use_cache_mat1)
{
	int rows = image.rows;
	int cols = image.cols;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (image.at<float>(i, j) < replacePixelsLowValue || image.at<float>(i, j) > replacePixelsHighValue) {
				image.at<float>(i, j) = 0;
			}
		}
	}

	threshold(image, image, replacePixelsLowValue, 255, THRESH_BINARY);
	image.convertTo(image, CV_8UC1);

	if (use_cache_mat1) {
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				if (i + roiScale >= rows) {
					bool condition1 = (image.at<uchar>(i, j) - image.at<uchar>(rows - 1, j) != 0);
					bool condition2 = (image.at<uchar>(i, j) - image.at<uchar>(i - roiScale, j) == 255);
						if (condition1 || condition2) {
							image.at<uchar>(i, j) = 0;
							//cout << (image.at<uchar>(1811, 1478) - image.at<uchar>(1811 - roiScale, 1478)) << endl;
							//cout << i << endl;
						}
				}
				else if (image.at<uchar>(i, j) - image.at<uchar>(i + roiScale, j) != 0) {
					image.at<uchar>(i, j) = 0;
				}
			}
		}
	}

	else {
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				if (j + roiScale >= cols) {
					bool condition1 = (image.at<uchar>(i, j) - image.at<uchar>(i, cols - 1) != 0);
					bool condition2 = (image.at<uchar>(i, j) - image.at<uchar>(i, j - roiScale) == 255);
					if (condition1 || condition2) {
						image.at<uchar>(i, j) = 0;
					}
				}
				else if (image.at<uchar>(i, j) - image.at<uchar>(i, j + roiScale) != 0) {
					image.at<uchar>(i, j) = 0;
				}
			}
		}
	}
}

bool cmp_grid_corner(vector<Point> a, vector<Point> b)
{
	if (abs(a[0].y - b[0].y) > 100) {
		return a[0].y < b[0].y;
	}
	else {
		return a[0].x < b[0].x;
	}
}

void DigitalRecognition::sortCorners(vector<Point>& corners, vector<vector<Point>>& grid_corner, Mat& gray_image)
{
	vector<vector<Point>>cell_box_temp;
	//寻找左上右上
	for (int i = 0; i < corners.size(); i++)
	{
		vector<Point>temp_point;
		for (int j = 0; j < corners.size(); j++)
		{
			if (corners[i].x - corners[j].x> -720 && corners[i].x - corners[j].x < -640 && corners[i].y - corners[j].y < 25 && corners[i].y - corners[j].y > -25)
			{
				temp_point.push_back(corners[i]);
				temp_point.push_back(corners[j]);
				cell_box_temp.push_back(temp_point);
			}
		}
	}
	//右下
	for (int i = 0; i < cell_box_temp.size(); i++)
	{
		Point temp_point;
		for (int j = 0; j < corners.size(); j++)
		{
			if (cell_box_temp[i][0].x - corners[j].x> -720 && cell_box_temp[i][0].x - corners[j].x < -640 && cell_box_temp[i][0].y - corners[j].y < -640 && cell_box_temp[i][0].y - corners[j].y > -720)
			{
				cell_box_temp[i].push_back(corners[j]);
			}
		}
	}
	//左下
	for (int i = 0; i < cell_box_temp.size(); i++)
	{
		Point temp_point;
		for (int j = 0; j < corners.size(); j++)
		{
			if (cell_box_temp[i][0].x - corners[j].x> -25 && cell_box_temp[i][0].x - corners[j].x < 25 && cell_box_temp[i][0].y - corners[j].y < -640 && cell_box_temp[i][0].y - corners[j].y > -720)
			{
				cell_box_temp[i].push_back(corners[j]);
			}
		}
	}
	/* 防止有容器中出现不齐4个角点的情况引起程序崩溃 */
	for (int i = 0; i < cell_box_temp.size(); i++)
	{
		if (cell_box_temp[i].size() == 4 )
		{
			grid_corner.push_back(cell_box_temp[i]);
		}
	}
	//按从左到右，从上到下给grid排序
	sort(grid_corner.begin(), grid_corner.end(), cmp_grid_corner);
}