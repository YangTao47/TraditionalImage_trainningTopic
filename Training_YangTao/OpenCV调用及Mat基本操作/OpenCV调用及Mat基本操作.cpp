#include<iostream>
using namespace std;
#include<opencv2\opencv.hpp>
using namespace cv;

//void drawCircle(Mat img, int x, int y, int r, const Scalar& color)
// 
void drawPoint(Mat &image,int x,int y,int r)
{

	for (int a = 0; a < 1000; a++) {
		for (int b = 0; b < 1000; b++) {
			if (pow((a - x), 2) + pow((b - y), 2) == pow(r, 2))
			{
				cout << a << b << endl;
				//cout<<image.at<uchar>(a, b)<<",";
				//定位到（row,col）像素点的蓝色通道，并进行颜色变换
				image.at<Vec3b>(a, b)[0] = 255;
				//定位到（row,col）像素点的绿色通道，并进行颜色变换
				image.at<Vec3b>(a, b)[1] = 255;
				//定位到（row,col）像素点的红色通道，并进行颜色变换
				image.at<Vec3b>(a, b)[2] = 255;
				
			}

		}

	}

}

void drawPixel(Mat &image,int x, int y)
{

	
	//定位到（row,col）像素点的蓝色通道，并进行颜色变换
	image.at<Vec3b>(x, y)[0] = 255;
	//定位到（row,col）像素点的绿色通道，并进行颜色变换
	image.at<Vec3b>(x, y)[1] = 255;
	//定位到（row,col）像素点的红色通道，并进行颜色变换
	image.at<Vec3b>(x, y)[2] = 255;
	

}






void drawPoint(Mat &image, int x0, int y0, int x, int y)
{	//四个对称点对
	drawPixel(image, x0 + x, y0 + y);//第一象限上半部分
	drawPixel(image, x0 + y, y0 + x);//第一象限下半部分
	drawPixel(image, x0 - x, y0 + y);
	drawPixel(image, x0 - y, y0 + x);
	drawPixel(image, x0 + x, y0 - y);
	drawPixel(image, x0 + y, y0 - x);
	drawPixel(image, x0 - x, y0 - y);
	drawPixel(image, x0 - y, y0 - x);

}

//中心画圆法
void MiddlePiontCircle(Mat &image,int x0, int y0, int r) 
{
	int x = 0;
	int y = r;
	int d = 1 - r;

	if (x0 > 600||y0 > 800)
	{

		cout << "圆心超出范围请重新输入";
		
	}
	else {
		drawPoint(image, x0, y0, x, y);
		while (x <= y)
		{
		    if (d < 0)
				d += 2 * x+3 ;
			else
			{
				d += 2 * (x - y) + 5;
				y--;
			}
			
			x++;
			drawPoint(image, x0, y0, x, y);


		}
	}




}



int main()
{
	Mat image(600, 800, CV_8UC3, Scalar(0, 0, 0));
	int x, y, r;
	cin >> x;
	cin >> y;
	cin >> r;
	//imshow("create", image);
	//waitKey(0);
	//circle(image, Point(800, 800), 100, Scalar(255, 255, 255));
	//imshow("output", image);
	//waitKey(0);
	if (y-r>0&&y+r<600&&x-r>0&&x+r<800)
	{
		Mat result;
		MiddlePiontCircle(image, x, y, r);
		//GaussianBlur(image,pro1,Size(5, 5), 10,20);
		//Canny(image, pro1, 3, 3, 3);
		// 阈值分割 做一个取反操作
		//imshow("image", image);
		threshold(image, result, 220, 255, THRESH_BINARY_INV);
		//填充
		floodFill(result, Point(0, 0), Scalar(0, 0, 0));
		//imwrite("D:\\result.jpg", result);
		imshow("output", result);
		waitKey(0);
	}
	else
	{
		cout << "非法操作：圆已跨界，请在范围内画圆";
	}
	system("pause");
	return 0;
}