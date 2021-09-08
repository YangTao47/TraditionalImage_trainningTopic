#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
#include"DigitalRecognition.h"
#include"CellBoardCoordinates.h"
using namespace cv;
using namespace std;

int main()
{
	DigitalRecognition test;
	vector<CellBoardCoordinates> cell_board_point_vector;
	Mat image = imread("标定1/正/1.bmp", CV_8UC1);
	auto start_time = chrono::high_resolution_clock::now();

	cell_board_point_vector = test.extractCellCoordinates(image);
	for (int i = 0; i < cell_board_point_vector.size(); i++)
	{
		cout << cell_board_point_vector[i].boardCoordinates << endl;
		cout << cell_board_point_vector[i].cellCoordinates << endl;
	}

	chrono::time_point<chrono::high_resolution_clock> end_time = chrono::high_resolution_clock::now();
	chrono::milliseconds processing_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
	cout << "用时" << processing_time.count() << "毫秒" << endl;
	getchar();
	return 0;
}