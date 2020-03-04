#include <iostream>  
#include <fstream>  
#include <opencv2/opencv.hpp>
#include <string>
#include <stdio.h>
#include "Windows.h" 
#include <opencv2\opencv.hpp>  
#include "cv.h"  
#include "highgui.h" 
using namespace std;
using namespace cv;


char saveName[256];//裁剪出来的样本图片文件名
int CropImageCount = 0; //裁剪出来的样本图片个数
int main()
{
	Mat src;
	ifstream myfile;		
	string fileAdr;
	int headNum,img_sign ,img_x,img_y,img_w,img_h;	//人头个数，标志位（1），x,y,w,h（位置信息）
	myfile.open("E:\\大学\\中英3\\李旭\\yuncong\\yuncong_data\\Mall_train.txt",ios::in);	//只读
	/*myfile.open("C:\\Users\\lenovo\\Desktop\\新建文本文档.txt", ios::in); */
	string temp;
	int v;
	if (!myfile.is_open())
	{
		cout << "File loaf error!" << endl;
		return 0;
	}
	stringstream str_stream;		//创建一个字符串流用于下面的转换
	while (myfile.peek() != EOF)		//判断是否到文件末尾
	{
		getline(myfile, temp);	//读取一行的字符串放在temp中
		str_stream << temp;		//把数据传入流中
		str_stream >> fileAdr;		//获取文件路径
		fileAdr = "E:/大学/中英3/李旭/yuncong/yuncong_data/" + fileAdr;
		cout << "file:" << fileAdr << endl;
		src = imread(fileAdr, 1);		//读取图片
		if (src.empty())
		{
			cout << "读取图片失败" << endl;
			return 0;
		}
		//此时的字符串流信息为：人头数、标志位（1）、x、y、w、h、```标志位（1）、x、y、w、h
		str_stream >> headNum;		//获取人头数
		//cout <<"headNum:" << headNum << endl;
		cout << "ing......" << endl;
		for (int i = 0; i < headNum; i++)
		{
			str_stream >> img_sign;
			/*获取字符串流中的人头信息*/
			str_stream >> img_x;
			str_stream >> img_y;
			str_stream >> img_w;
			str_stream >> img_h;
			//cout << img_x << ' ' << img_y << ' ' << img_w << ' ' << img_h << ' ' << endl;
			if (img_w == img_h)		//防止TXT中信息出错
			{
				Mat imgROI = src(Rect(img_x, img_y, img_w, img_h));
				sprintf_s(saveName, "E:\\大学\\中英3\\李旭\\C++处理后的人头图片resize\\HeadPos%08d.jpg", ++CropImageCount);//生成裁剪出的负样本图片的文件名
				resize(imgROI, imgROI, cv::Size(48, 48));
				imwrite(saveName, imgROI);//保存文件
			}
			
		}
		str_stream.str("");		//清空数据
		if (str_stream.eof())
		{
			str_stream.clear();
		}


	}
	myfile.close();   //关闭文件
	
	return 0;
}
