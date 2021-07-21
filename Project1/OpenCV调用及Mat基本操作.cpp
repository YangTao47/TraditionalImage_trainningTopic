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
				//��λ����row,col�����ص����ɫͨ������������ɫ�任
				image.at<Vec3b>(a, b)[0] = 255;
				//��λ����row,col�����ص����ɫͨ������������ɫ�任
				image.at<Vec3b>(a, b)[1] = 255;
				//��λ����row,col�����ص�ĺ�ɫͨ������������ɫ�任
				image.at<Vec3b>(a, b)[2] = 255;
				
			}

		}

	}

}

void drawPixel(Mat &image,int x, int y)
{

	
	//��λ����row,col�����ص����ɫͨ������������ɫ�任
	image.at<Vec3b>(x, y)[0] = 255;
	//��λ����row,col�����ص����ɫͨ������������ɫ�任
	image.at<Vec3b>(x, y)[1] = 255;
	//��λ����row,col�����ص�ĺ�ɫͨ������������ɫ�任
	image.at<Vec3b>(x, y)[2] = 255;
	

}






void drawPoint(Mat &image, int x0, int y0, int x, int y)
{	//�ĸ��ԳƵ��
	drawPixel(image, x0 + x, y0 + y);//��һ�����ϰ벿��
	drawPixel(image, x0 + y, y0 + x);//��һ�����°벿��
	drawPixel(image, x0 - x, y0 + y);
	drawPixel(image, x0 - y, y0 + x);
	drawPixel(image, x0 + x, y0 - y);
	drawPixel(image, x0 + y, y0 - x);
	drawPixel(image, x0 - x, y0 - y);
	drawPixel(image, x0 - y, y0 - x);

}

//���Ļ�Բ��
void MiddlePiontCircle(Mat &image,int x0, int y0, int r) 
{
	int x = 0;
	int y = r;
	int d = 1 - r;

	if (x0 > 600||y0 > 800)
	{

		cout << "Բ�ĳ�����Χ����������";
		
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
		// ��ֵ�ָ� ��һ��ȡ������
		//imshow("image", image);
		threshold(image, result, 220, 255, THRESH_BINARY_INV);
		//���
		floodFill(result, Point(0, 0), Scalar(0, 0, 0));
		//imwrite("D:\\result.jpg", result);
		imshow("output", result);
		waitKey(0);
	}
	else
	{
		cout << "�Ƿ�������Բ�ѿ�磬���ڷ�Χ�ڻ�Բ";
	}
	system("pause");
	return 0;
}