#include <opencv2/opencv.hpp>
#include <iostream>
#include<algorithm>
#include<chrono>
#include"DigitalRecognition.h"
#include"CellBoardCoordinates.h"
using namespace cv;
using namespace std;
using namespace std::chrono;

int main()
{
	DigitalRecognition test;
	vector<CellBoardCoordinates> cell_board_point_vector;
	Mat Image = imread("标定1\\正\\2.bmp");
	auto start_time = high_resolution_clock::now();

	cell_board_point_vector = test.extractCellCoordinates(Image);
	for (int i = 0; i < cell_board_point_vector.size(); i++)
	{
		cout << cell_board_point_vector[i].boardCoordinates << endl;
		cout << cell_board_point_vector[i].cellCoordinates << endl;
	}

	time_point<high_resolution_clock> end_time = high_resolution_clock::now();
	milliseconds processing_time = duration_cast<milliseconds>(end_time - start_time);
	cout << "用时" << processing_time.count() << "毫秒" << endl;
	system("pause");
	return 0;
}
