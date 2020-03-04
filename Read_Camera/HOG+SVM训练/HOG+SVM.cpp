#include <iostream>  
#include <fstream>  
#include <opencv2/opencv.hpp>
#include <string>
 
#include "Windows.h" 
#include <opencv2\opencv.hpp>  
#include "cv.h"  
#include "highgui.h" 


#define PosSamNO 3000  //正样本个数  
#define NegSamNO 6000 //负样本个数  
#define HardExampleNO 0 //难例个数

#define WinX 16	//检测窗口尺寸
#define WinY 16
#define WinStep	2	//检测窗口移动步长
#define BlockX 4	//块尺寸
#define BlockY 4
#define BlockStep	2	//块步长
#define CellSize	2	//cell尺寸
using namespace std;
using namespace cv;


void train_svm_hog()
{
	//HOG检测器，用来计算HOG描述子的
	//检测窗口(48,48),块尺寸(16,16),块步长(8,8),cell尺寸(8,8),直方图bin个数9 
	cv::HOGDescriptor hog(cv::Size(WinX, WinY), cv::Size(BlockX, BlockY), cv::Size(BlockStep, BlockStep), cv::Size(CellSize, CellSize), 9);
	int DescriptorDim;//HOG描述子的维数，由图片大小、检测窗口大小、块大小、细胞单元中直方图bin个数决定  

					  //设置SVM参数
	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
	svm->setType(cv::ml::SVM::Types::C_SVC);
	svm->setKernel(cv::ml::SVM::KernelTypes::LINEAR);
	svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));

	std::string ImgName;

	//正样本图片的文件列表
	std::ifstream finPos("E:\\大学\\中英3\\李旭\\C++处理后的人头图片resize\\poslist.txt");
	//负样本图片的文件列表
	std::ifstream finNeg("E:\\大学\\中英3\\李旭\\negFile\\headNeg.txt");

	//所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数 
	cv::Mat sampleFeatureMat;
	//训练样本的类别向量，行数等于所有样本的个数，列数等于1；1表示有目标，-1表示无目标 
	cv::Mat sampleLabelMat;


	//依次读取正样本图片，生成HOG描述子  
	for (int num = 0; num < PosSamNO && getline(finPos, ImgName); num++)
	{
		std::cout << "Processing：" << ImgName << std::endl;
		cv::Mat image = cv::imread(ImgName);
		//image = image(cv::Rect(16, 16, WinX, WinY));//将96*160的INRIA正样本图片剪裁为WinX*WinY，即剪去上下左右各16个像素 
		cv::resize(image, image, cv::Size(WinX, WinY));
		//HOG描述子向量 
		std::vector<float> descriptors;
		//计算HOG描述子，检测窗口移动步长(8,8)
		hog.compute(image, descriptors, cv::Size(WinStep, WinStep));

		//处理第一个样本时初始化特征向量矩阵和类别矩阵，因为只有知道了特征向量的维数才能初始化特征向量矩阵  
		if (0 == num)
		{
			//HOG描述子的维数 
			DescriptorDim = descriptors.size();
			//初始化所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数sampleFeatureMat  
			sampleFeatureMat = cv::Mat::zeros(PosSamNO + NegSamNO + HardExampleNO, DescriptorDim, CV_32FC1);
			//初始化训练样本的类别向量，行数等于所有样本的个数，列数等于1 
			sampleLabelMat = cv::Mat::zeros(PosSamNO + NegSamNO + HardExampleNO, 1, CV_32SC1);
		}

		//将计算好的HOG描述子复制到样本特征矩阵sampleFeatureMat  
		for (int i = 0; i < DescriptorDim; i++)
		{
			//第num个样本的特征向量中的第i个元素 
			sampleFeatureMat.at<float>(num, i) = descriptors[i];
		}
		//正样本类别为1，有目标	 
		sampleLabelMat.at<float>(num, 0) = 1;
	}

	//依次读取负样本图片，生成HOG描述子  
	for (int num = 0; num < NegSamNO && getline(finNeg, ImgName); num++)
	{
		std::cout << "Processing：" << ImgName << std::endl;
		cv::Mat src = cv::imread(ImgName);
		cv::resize(src, src, cv::Size(WinX, WinY));
		//HOG描述子向量
		std::vector<float> descriptors;
		//计算HOG描述子，检测窗口移动步长(8,8) 
		hog.compute(src, descriptors, cv::Size(WinStep, WinStep));
		std::cout << "descriptor dimention：" << descriptors.size() << std::endl;
		//将计算好的HOG描述子复制到样本特征矩阵sampleFeatureMat  
		for (int i = 0; i < DescriptorDim; i++)
		{
			//第PosSamNO+num个样本的特征向量中的第i个元素
			sampleFeatureMat.at<float>(num + PosSamNO, i) = descriptors[i];
		}

		//负样本类别为-1，无目标
		sampleLabelMat.at<float>(num + PosSamNO, 0) = -1;
	}

	//处理HardExample负样本  
	if (HardExampleNO > 0)
	{
		//HardExample负样本的文件列表 
		std::ifstream finHardExample("D:\\iVMS-4200 Station\\Catch\\Labhard\\Labhard.txt");
		//依次读取HardExample负样本图片，生成HOG描述子  
		for (int num = 0; num < HardExampleNO && getline(finHardExample, ImgName); num++)
		{
			std::cout << "Processing：" << ImgName << std::endl;

			cv::Mat src = cv::imread(ImgName, cv::IMREAD_GRAYSCALE);
			cv::resize(src, src, cv::Size(WinX, WinY));
			//HOG描述子向量  
			std::vector<float> descriptors;
			//计算HOG描述子，检测窗口移动步长(8,8) 
			hog.compute(src, descriptors, cv::Size(WinStep, WinStep));

			//将计算好的HOG描述子复制到样本特征矩阵sampleFeatureMat  
			for (int i = 0; i < DescriptorDim; i++)
			{
				//第PosSamNO+num个样本的特征向量中的第i个元素
				sampleFeatureMat.at<float>(num + PosSamNO + NegSamNO, i) = descriptors[i];
			}
			//负样本类别为-1，无目标 
			sampleLabelMat.at<float>(num + PosSamNO + NegSamNO, 0) = -1;
		}
	}

	//训练SVM分类器  
	//迭代终止条件，当迭代满1000次或误差小于FLT_EPSILON时停止迭代  
	std::cout << "开始训练SVM分类器" << std::endl;
	cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(sampleFeatureMat, cv::ml::SampleTypes::ROW_SAMPLE, sampleLabelMat);
	//训练分类器  
	svm->train(td);
	std::cout << "训练完成" << std::endl;
	//将训练好的SVM模型保存为xml文件
	svm->save("E:\\大学\\中英3\\李旭\\SVM_HOG.xml");

	return;
}
int main()
{
	train_svm_hog();
	return 0;
}