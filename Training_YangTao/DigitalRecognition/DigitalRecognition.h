#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
#include"CellBoardCoordinates.h"
using namespace cv;
using namespace std;

class DigitalRecognition
{
public:

	vector<CellBoardCoordinates> extractCellCoordinates(const Mat& Image);

private:

	const int connectedComponentsArea1 = 50000;
	const int connectedComponentsArea2 = 50000;
	const int expandPixels = 200;
	const float smallScale = 0.8f;
	const float bigScale = 1.f / smallScale;
	const int roiScale = 160;
	const int warpPerspectiveScale = 160;
	const int replacePixelsLowValue = 95;
	const int replacePixelsHighValue = 134;


	Point getBoardCoordinates(const Mat& src, vector<vector<Point>>& P, int i);
	Point composeNumbers(int num1, int num2, int num3, int num4);
	int matchTemplates(const Mat& src, int nums);
	void findCellBox(const Mat& src, vector<vector<Point>>& grid_corner);
	void reverseImage(const Mat& src, Mat& dst);
	void findNoiseMask(const Mat& src, int area, Mat& mask);
	void denoiseImage(const Mat& reverse_image, const Mat& binary_image, Mat& denoise_image);
	Mat computeScoreMap(Mat& gray_image, Size& ksize1, Size& ksize2);
	void replacePixels(Mat& image, bool use_cache_mat1 = true);
	void computeCrossPoint(const Mat& src, vector<Point>& cross_points);
	void sortCorners(vector<Point>& corners, vector<vector<Point>>& grid_corner, Mat& gray_image);
};
