#pragma once
#include<iostream>
#include<opencv2\opencv.hpp>
#include<chrono>
#include"CellBoardCoordinates.h"
using namespace std;
using namespace cv;
using namespace std::chrono;

class DigitalRecognition
{
public:
	
	vector<CellBoardCoordinates> extractCellCoordinates(const Mat& Image);


private:
	vector<vector<Point2f>> findCellBox1(const Mat& g_srcImage);
	vector<vector<Point2f>> sortCorners(vector<Point2f> corners, Mat& showimage);
	void showAllCorners(vector<Point2f> corners, Mat& showimage);
	Point getBoardCoordinates(const Mat& src, vector<vector<Point2f>> P, int i);
	Point composeNumbers(int num1, int num2, int num3, int num4);
	int getRowSum(Mat src, int row);
	int getColSum(Mat src, int col);
	int matchTemplates(const Mat& src, int nums);
	int cutLeft(Mat& src, Mat& leftImg, Mat& rightImg);
	void cutTop(Mat& src, Mat& dstImg);
	Mat removeWhiteNoise(Mat& src);
	Mat removeBlackNoise(Mat& src);
	Mat reverseImage(Mat& src);
};
