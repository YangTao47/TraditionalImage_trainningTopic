#include<iostream>
using namespace std;
#include<opencv2\opencv.hpp>
#include<ctime>
using namespace cv;

// at
void at_method1(Mat &image)
{
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (image.channels() == 1)
			{	
				image.at<uchar>(i, j) = 255;
			}
			/*else if (image.channels() == 3)
			{
				image.at<Vec3b>(i, j)[0] = 255;
				image.at<Vec3b>(i, j)[1] = 255;
				image.at<Vec3b>(i, j)[2] = 255;
			}*/
		}
	}

}
//data
void data_method2(Mat &image)
{
	for (int i = 0; i < image.rows; i++)//行
	{
		for (int j = 0; j < image.cols; j++)//列
		{
			if (image.channels() == 1)
			{
				//图像数组是逐行逐列顺序排列的，也就是第一行，全列，第二行全列的走
				int indexs = i * image.cols + j;
				image.data[indexs] = 255;
			}
			//else if (image.channels() == 3)
			//{
			//	int indexs = i * image.cols + j;
			//	//在彩色图像中，每个indexs又是三个通过的数组
			//	image.data[3 * indexs + 0] = 255;
			//	image.data[3 * indexs + 1] = 255;
			//	image.data[3 * indexs + 2] = 255;

			//}
		}
	}

}

//ptr
void ptr_method3(Mat &image)
{
	for (int i = 0; i < image.rows; i++)
	{
		//获取第i行首地址,ptr与at结果差不多
		
		for (int j = 0; j < image.cols; j++)
		{
			if (image.channels() == 1)
			{
				uchar* image_rows_ptr = image.ptr<uchar>(i);
				image_rows_ptr[j] = 255;
			}
			//else
			//{
			//	Vec3b* image_rows_ptr = image.ptr<Vec3b>(i);
			//	//每行指针的地址，又代表一行数组名称，因此下面就是遍历一行中的列数组数据

			//	image_rows_ptr[j][0] = 255;
			//	image_rows_ptr[j][1] = 255;
			//	image_rows_ptr[j][2] = 255;
			//}
		}
		
	}

}
//iterator
void iterator_method4(Mat& image)
{
	Mat_<uchar>::iterator itbegin = image.begin<uchar>();
	Mat_<uchar>::iterator itend = image.end<uchar>();
	for (; itbegin != itend; itbegin++) {
		(*itbegin) = 255;

	}
}




int main()
{
	clock_t start, stop;
	Mat image(4384, 6576,CV_8UC1 ,Scalar(0));


	start = clock();
	iterator_method4(image);
	stop = clock();
	cout << (float)(stop - start)/1000 << "秒" << endl;
	cout << image.size() << endl;
	imshow("create_picutre", image);
	waitKey(27);
	system("pause");
	return 0;
}