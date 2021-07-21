#include <iostream>
#include<opencv2\opencv.hpp>
#include<opencv2\core\core.hpp>

#define WHITE   255
#define BLACK   0
using namespace std;
using namespace cv;
void NaiveRemoveNoise(int pNum, Mat* img);
void Flood_filled_ContoursRemoveNoise(double pArea, Mat * img);
Mat cirlce_detect(Mat &src);
Mat test();

int main()
{

	Mat image=test();
	cout << image.channels() << endl;
	imshow("result", image);
	waitKey(27);
	system("pause");
	return 0;

}

Mat test()
{
	Mat imageSource = imread("C:\\Users\\hzxuxin\\Pictures\\Saved Pictures\\test.jpg");
	//imshow("ԭʼͼ��", imageSource);

	Mat image;
	//GaussianBlur(imageSource, image, Size(3, 3), 10);//��˹�˲�
	//Canny(imageSource, image, 80, 150);
	//medianBlur(image, image, 1);
	GaussianBlur(imageSource, image, Size(3, 3), 50);
	Canny(imageSource, image, 80, 200);
	medianBlur(image, image, 1);


	Mat structElement1 = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1));
	Mat structElement2 = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1));
	dilate(image, image, structElement1);
	erode(image, image, structElement2);
	NaiveRemoveNoise(1, &image);
	Flood_filled_ContoursRemoveNoise(3.1, &image);
	
	////vector<Vec3f> pcircles;
	//////���룬��������������ͣ�-HOUGH_GRADIENT��dp(dp=1ʱ��ʾ����ռ�������ͼ��ռ�Ĵ�Сһ�£�dp=2ʱ����ռ�������ͼ��ռ��һ�룬�Դ�����)����̾���-���Էֱ�������Բ�� ����Ϊ��ͬ��Բ ,��Ե���ʱʹ��Canny���ӵĸ���ֵ�����ĵ��ۼ�����ֵ����ѡԲ�ģ�����ռ����ۼӺʹ��ڸ���ֵ�ĵ�Ͷ�Ӧ��Բ�ģ�����⵽Բ����С�뾶����⵽Բ�ĵ����뾶
	////HoughCircles(image, pcircles, CV_HOUGH_GRADIENT, 1, 10, 100, 30, 5, 20);

	//////��ԭͼ����Բ�ĺ�Բ
	////for (size_t i = 0; i < pcircles.size(); i++)//size_t �޷������� unsigned int 
	////{
	////	//��ȡԲ������
	////	Point center(cvRound(pcircles[i][0]), cvRound(pcircles[i][1]));//cvRound ���غͲ�����ӽ�������ֵ
	////																   //��ȡ�뾶
	////	int radius = cvRound(pcircles[i][2]);
	////	//Բ��
	////	circle(imageSource, center, 3, Scalar(0, 255, 0), -1, 8, 0);
	////	//Բ
	////	circle(imageSource, center, radius, Scalar(0, 255, 0), 3, 8, 0);
	////}


	//imshow("111", image);
	waitKey(27);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point());
	Mat imageContours = Mat::zeros(image.size(), CV_8UC1);
	Mat Contours = Mat::zeros(image.size(), CV_8UC1); //����

	for (int i = 0; i < contours.size(); i++)
	{
		//contours[i]������ǵ�i��������contours[i].size()������ǵ�i�����������е����ص���
		for (int j = 0; j < contours[i].size(); j++)
		{
			//���Ƴ�contours�������е����ص�
			Point P = Point(contours[i][j].x, contours[i][j].y);
			Contours.at<uchar>(P) = 255;
		}

		//��������
		drawContours(imageContours, contours, i, Scalar(255), 1, 8, hierarchy);
	}
	//imshow("Contours Image CV_TRET_TREE", imageContours); //����
	//imshow("Point of Contours CV_CHAIN_APPROX_NONE", Contours); //����contours�ڱ�������������㼯
	//waitKey(0);
	return imageContours;
}





void NaiveRemoveNoise(int pNum, Mat* img)
{
	//naive remove noise
	int i, j, m, n, nValue, nCount;
	int nWidth = img->cols;
	int nHeight = img->rows;
	//set boundry to be white
	for (i = 0; i < nWidth; ++i)
	{
		img->at<uchar>(0, i) = WHITE;               //���к���
		img->at<uchar>(nHeight - 1, i) = WHITE;       //���к���
	}

	for (i = 0; i < nHeight; ++i)
	{
		img->at<uchar>(i, 0) = WHITE;
		img->at<uchar>(i, nWidth - 1) = WHITE;
	}
	//if the neighbor of a point is white but it is black, delete it
	for (j = 1; j < nHeight - 1; ++j)//����
		for (i = 1; i < nWidth - 1; ++i)//����
		{
			nValue = img->at<uchar>(j, i);
			if (!nValue)//����Ǻ�ɫ���
			{
				nCount = 0;
				for (m = i - 1; m <= i + 1; ++m)//i������
					for (n = j - 1; n <= j + 1; ++n)//j������
					{
						if (!(img->at<uchar>(n, m)))
							nCount++;
					}
				if (nCount <= pNum)//�ж�3X3��Χ��һ���м�����ɫ��㣬�����9����ɫ���,�����Ŀ����pNum, ������Ϊ��ɫ
					img->at<uchar>(j, i) = WHITE;
			}
			else
			{
				nCount = 0;
				for (m = i - 1; m <= i + 1; ++m)
					for (n = j - 1; n <= j + 1; ++n)
					{
						if (!(img->at<uchar>(n, m)))
							nCount++;
					}
				if (nCount >= 7)
					img->at<uchar>(j, i) = BLACK;
			}
		}
}

/*  ��������        Flood_filled_ContoursRemoveNoise
*  ���������
*  img��         ͼ��ָ��
*  pArea:        ����ɫ�������ص�Ķ��٣�������������Ŀ���Ͱ����Ǳ�Ϊ��ɫ�ı���
*  return:       void
*/
void Flood_filled_ContoursRemoveNoise(double pArea, Mat * img)
{
	int i, j;
	int color = 1;
	int nHeight = img->rows;
	int nWidth = img->cols;

	for (i = 0; i < nWidth; ++i)//����
		for (j = 0; j < nHeight; ++j) //����
		{
			if (!img->at<uchar>(j, i))
			{
				//FloodFill each point in connect area using different color
				floodFill(*img, cvPoint(i, j), cvScalar(color));
				color++;
			}
		}

	int ColorCount[255] = { 0 };
	for (i = 0; i < nWidth; ++i)  //����
	{
		for (j = 0; j < nHeight; ++j)  //����
		{
			//caculate the area of each area
			if (img->at<uchar>(j, i) != 255)
			{
				ColorCount[img->at<uchar>(j, i)]++;
			}
		}
	}
	//get rid of noise point
	for (i = 0; i < nWidth; ++i)  //����
	{
		for (j = 0; j < nHeight; ++j)  //����
		{
			if (ColorCount[img->at<uchar>(j, i)] <= pArea)
			{
				img->at<uchar>(j, i) = WHITE;
			}
		}
	}
	for (i = 0; i < nWidth; ++i)//����
	{
		for (j = 0; j < nHeight; ++j) //����
		{
			if (img->at<uchar>(j, i) < WHITE)
			{
				img->at<uchar>(j, i) = BLACK;
			}
		}
	}
}

Mat cirlce_detect(Mat &src)
{
	
	Mat medianImg;
	Mat grayImg;
	Mat houghCirclesImg;
	if (src.empty())
	{
		cout << "fail to load Img" << endl;
	}

	//����Բ���
	vector<Vec3f> pcircles;
	//���룬��������������ͣ�-HOUGH_GRADIENT��dp(dp=1ʱ��ʾ����ռ�������ͼ��ռ�Ĵ�Сһ�£�dp=2ʱ����ռ�������ͼ��ռ��һ�룬�Դ�����)����̾���-���Էֱ�������Բ�� ����Ϊ��ͬ��Բ ,��Ե���ʱʹ��Canny���ӵĸ���ֵ�����ĵ��ۼ�����ֵ����ѡԲ�ģ�����ռ����ۼӺʹ��ڸ���ֵ�ĵ�Ͷ�Ӧ��Բ�ģ�����⵽Բ����С�뾶����⵽Բ�ĵ����뾶
	HoughCircles(grayImg, pcircles, CV_HOUGH_GRADIENT, 1, 10, 100, 30, 5, 50);

	//��ԭͼ����Բ�ĺ�Բ
	for (size_t i = 0; i < pcircles.size(); i++)//size_t �޷������� unsigned int 
	{
		//��ȡԲ������
		Point center(cvRound(pcircles[i][0]), cvRound(pcircles[i][1]));//cvRound ���غͲ�����ӽ�������ֵ
																	   //��ȡ�뾶
		int radius = cvRound(pcircles[i][2]);
		//Բ��
		circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//Բ
		circle(src, center, radius, Scalar(0, 255, 0), 3, 8, 0);
	}

	return src;

}

