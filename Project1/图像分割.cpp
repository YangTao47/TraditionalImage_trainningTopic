#include<iostream>
using namespace std;
#include<opencv2\opencv.hpp>
using namespace cv;

int main()
{
	Mat t1, t2, t3, t4,yellow;
	Mat image = imread("C:\\Users\\hzxuxin\\Pictures\\Saved Pictures\\test.jpg");
	threshold(image, image, 50, 255, THRESH_BINARY);
	//cvtColor(image, t3, CV_RGB2GRAY);
	//Canny(t3, t1, 100,20);
	GaussianBlur(image,t1,Size(3,3),10);
	Canny(t1, t2, 80, 150);
	medianBlur(t2, t3, 1);
	cvtColor(t3, t3, CV_GRAY2BGR);






	//addWeighted(image, 0.3, t3, 0.7, 0, t4);
	//threshold(t3, t3, 200, 255, THRESH_BINARY);
	//cout << t3.channels() << endl;
	//cvtColor(t3, t3, CV_GRAY2BGR);
	//floodFill(t3, t3, Point(0, 0), Scalar(0,0,0));
	//cvtColor(t3, t3, CV_GRAY2BGR);
	//t4 = image + t3;
	//floodFill(image,t2, Point(0, 0), Scalar(0, 0, 0));

	imshow("test", image);
	waitKey(27);

	system("pause");
	return 0;
}