#include<iostream>
using namespace std;
#include<opencv2\opencv.hpp>
using namespace cv;

Mat calcGrayHist(const Mat &image)
{	//存储256个灰度级的像素个数
	Mat histogram = Mat::zeros(Size(256, 1), CV_32SC1);
	//图像的高和宽
	int rows = image.rows;
	int cols = image.cols;
	//计算每个灰度级的个数
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int index = int(image.at<uchar>(r, c));
			histogram.at<int>(0, index) += 1;
		}
	}
	return histogram;
}


int threshTwoPeaks(const Mat &image, Mat &thresh_out)
{
	Mat histogram = calcGrayHist(image);
	//cout << histogram.size() << endl;
	Point firstPeakLoc;
	minMaxLoc(histogram, NULL, NULL, NULL, &firstPeakLoc);
	int firstPeak = firstPeakLoc.x;
	Mat measureDists = Mat::zeros(Size(256, 1), CV_32FC1);
	for (int k = 0; k < 256; k++)
	{
		int hist_k = histogram.at<int>(0, k);
		measureDists.at<float>(0, k) = pow(float(k - firstPeak), 2)*hist_k;
	}
	Point secondPeakLoc;
	minMaxLoc(measureDists, NULL, NULL, NULL, &secondPeakLoc);
	int secondPeak = secondPeakLoc.x;
	Point threshLoc;
	int thresh = 0;
	if (firstPeak < secondPeak)
	{
		minMaxLoc(histogram.colRange(firstPeak, secondPeak), NULL, NULL, &threshLoc);
		thresh = firstPeak + threshLoc.x + 1;
	}
	else
	{
		minMaxLoc(histogram.colRange(secondPeak, firstPeak), NULL, NULL, &threshLoc);
		thresh = secondPeak + threshLoc.x + 1;
	}
	threshold(image, thresh_out, thresh, 255, THRESH_BINARY);
	return thresh;
}
int main()
{	
	Mat t1;
	Mat image = imread("C:\\Users\\hzxuxin\\Pictures\\Saved Pictures\\test.jpg");
	threshTwoPeaks(image, t1);
	imshow("out", t1);
	waitKey(27);
	system("pause");
	return 0;
}