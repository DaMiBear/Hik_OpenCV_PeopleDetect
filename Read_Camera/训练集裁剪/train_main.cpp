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


char saveName[256];//�ü�����������ͼƬ�ļ���
int CropImageCount = 0; //�ü�����������ͼƬ����
int main()
{
	Mat src;
	ifstream myfile;		
	string fileAdr;
	int headNum,img_sign ,img_x,img_y,img_w,img_h;	//��ͷ��������־λ��1����x,y,w,h��λ����Ϣ��
	myfile.open("E:\\��ѧ\\��Ӣ3\\����\\yuncong\\yuncong_data\\Mall_train.txt",ios::in);	//ֻ��
	/*myfile.open("C:\\Users\\lenovo\\Desktop\\�½��ı��ĵ�.txt", ios::in); */
	string temp;
	int v;
	if (!myfile.is_open())
	{
		cout << "File loaf error!" << endl;
		return 0;
	}
	stringstream str_stream;		//����һ���ַ��������������ת��
	while (myfile.peek() != EOF)		//�ж��Ƿ��ļ�ĩβ
	{
		getline(myfile, temp);	//��ȡһ�е��ַ�������temp��
		str_stream << temp;		//�����ݴ�������
		str_stream >> fileAdr;		//��ȡ�ļ�·��
		fileAdr = "E:/��ѧ/��Ӣ3/����/yuncong/yuncong_data/" + fileAdr;
		cout << "file:" << fileAdr << endl;
		src = imread(fileAdr, 1);		//��ȡͼƬ
		if (src.empty())
		{
			cout << "��ȡͼƬʧ��" << endl;
			return 0;
		}
		//��ʱ���ַ�������ϢΪ����ͷ������־λ��1����x��y��w��h��```��־λ��1����x��y��w��h
		str_stream >> headNum;		//��ȡ��ͷ��
		//cout <<"headNum:" << headNum << endl;
		cout << "ing......" << endl;
		for (int i = 0; i < headNum; i++)
		{
			str_stream >> img_sign;
			/*��ȡ�ַ������е���ͷ��Ϣ*/
			str_stream >> img_x;
			str_stream >> img_y;
			str_stream >> img_w;
			str_stream >> img_h;
			//cout << img_x << ' ' << img_y << ' ' << img_w << ' ' << img_h << ' ' << endl;
			if (img_w == img_h)		//��ֹTXT����Ϣ����
			{
				Mat imgROI = src(Rect(img_x, img_y, img_w, img_h));
				sprintf_s(saveName, "E:\\��ѧ\\��Ӣ3\\����\\C++��������ͷͼƬresize\\HeadPos%08d.jpg", ++CropImageCount);//���ɲü����ĸ�����ͼƬ���ļ���
				resize(imgROI, imgROI, cv::Size(48, 48));
				imwrite(saveName, imgROI);//�����ļ�
			}
			
		}
		str_stream.str("");		//�������
		if (str_stream.eof())
		{
			str_stream.clear();
		}


	}
	myfile.close();   //�ر��ļ�
	
	return 0;
}
