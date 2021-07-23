#include<iostream>
#include<opencv2\opencv.hpp>
#include<ctime>
#include<chrono>
using namespace std::chrono;
using namespace cv;
using namespace std;

class Tranversing
{
public:
	/* 利用成员函数at */
	void atMethod1(Mat &image)
	{
		int nl = image.rows; 
		int nc = image.cols; 
		for (int i = 0; i < nl; i++)
		{
			for (int j = 0; j < nc; j++)
			{
				image.at<uchar>(i , j) = 255;
			}
		}
	}

	/* 利用成员变量data和[] */
	void dataMethod2(Mat &image)
	{
		int nl = image.rows;
		int nc = image.cols;
		for (int i = 0; i < nl; i++)
		{
			for (int j = 0; j < nc; j++)
			{
				/* 图像数组是逐行逐列顺序排列的，也就是第一行，全列，第二行全列的走 */
				int indexs = i * image.cols + j;
				image.data[indexs] = 255;
			}
		}
	}

	/* 利用成员函数ptr和[] */
	void ptrMethod3(Mat &image)
	{
		int nl = image.rows;
		int nc = image.cols;
		for (int i = 0; i < nl; i++)
		{
			for (int j = 0; j < nc; j++)
			{
				uchar* image_rows_ptr = image.ptr<uchar>(i);
				image_rows_ptr[j] = 255;
			}
		}
	}

	/* 利用迭代器iterator */
	void iteratorMethod4(Mat& image)
	{
		MatIterator_<uchar> itbegin = image.begin<uchar>();
		MatIterator_<uchar> itend = image.end<uchar>();
		for (; itbegin != itend; itbegin++)
		{
			(*itbegin) = 255;
		}
	}

	/* 利用LUT函数 */
	void lutMethod5(Mat &image)
	{
		uchar lutData[256];
		for (int i = 0; i < 256; i++)
		{
			if (i == 0)
			{
				lutData[i] = 255;
			}
		}
		Mat lut(1, 256, CV_8UC1, lutData);
		LUT(image, lut, image);
	}

	/* 利用ptr和*++ */
	void ptrMethod6(Mat &image)
	{
		int nl = image.rows;
		int nc = image.cols * image.channels();
		for (int i = 0; i < nl; i++)
		{
			uchar* data = image.ptr<uchar>(i);
			for (int j = 0; j < nc; j++)
			{
				*data++ = 255;
			}
		}
	}

	/* 利用data和step */
	void dataMethod7(Mat &image, int div = 64) {

		int nl = image.rows; 
		int nc = image.cols * image.channels(); 
		int step = image.step; /* image.step = image.step[1]*cols */ 	   
		uchar *data = image.data;
		for (int i = 0; i < nl; i++)
		{
			for (int j = 0; j < nc; j++)
			{
				*(data + j) = 255;
			} 
			data += step;  
		}
	}
};

int main()
{	
	const int kRectangleX = 6576, kRectangleY = 4383;
	Mat image(kRectangleY, kRectangleX, CV_8UC1, Scalar(0));
	time_point<high_resolution_clock> start_time = high_resolution_clock::now();
	Tranversing test;
	for (int i = 0; i <= 10; i++)
	{
		test.dataMethod7(image);
	}
	time_point<high_resolution_clock> end_time = high_resolution_clock::now();
	milliseconds processing_time = duration_cast<milliseconds>(end_time - start_time)/10;
	cout << processing_time.count() << endl;
	system("pause");
	return 0;
}
