#include <iostream>
#include "Windows.h" 
#include "HCNetSDK.h"
#include "plaympeg4.h"
#include <opencv2\opencv.hpp>  
#include "cv.h"  
#include "highgui.h" 
#include <process.h>
#include "Queue.h"
using namespace std;
using namespace cv;
#define USECOLOR 1
#define ImgX 64
#define ImgY 128

LONG nPort = -1;
HWND hWnd = NULL;

LinkQueue  MYQUEUE;		//创建队列
void svm_hog_detect();



void yv12toYUV(char *outYuv, char *inYv12, int width, int height, int widthStep)
{
	int col, row;
	unsigned int Y, U, V;
	int tmp;
	int idx;

	//printf("widthStep=%d.\n",widthStep);

	for (row = 0; row<height; row++)
	{
		idx = row * widthStep;
		int rowptr = row * width;

		for (col = 0; col<width; col++)
		{
			//int colhalf=col>>1;
			tmp = (row / 2)*(width / 2) + (col / 2);
			//         if((row==1)&&( col>=1400 &&col<=1600))
			//         { 
			//          printf("col=%d,row=%d,width=%d,tmp=%d.\n",col,row,width,tmp);
			//          printf("row*width+col=%d,width*height+width*height/4+tmp=%d,width*height+tmp=%d.\n",row*width+col,width*height+width*height/4+tmp,width*height+tmp);
			//         } 
			Y = (unsigned int)inYv12[row*width + col];
			U = (unsigned int)inYv12[width*height + width * height / 4 + tmp];
			V = (unsigned int)inYv12[width*height + tmp];
			//         if ((col==200))
			//         { 
			//         printf("col=%d,row=%d,width=%d,tmp=%d.\n",col,row,width,tmp);
			//         printf("width*height+width*height/4+tmp=%d.\n",width*height+width*height/4+tmp);
			//         return ;
			//         }
			if ((idx + col * 3 + 2)> (1200 * widthStep))
			{
				//printf("row * widthStep=%d,idx+col*3+2=%d.\n",1200 * widthStep,idx+col*3+2);
			}
			outYuv[idx + col * 3] = Y;
			outYuv[idx + col * 3 + 1] = U;
			outYuv[idx + col * 3 + 2] = V;
		}
	}
	//printf("col=%d,row=%d.\n",col,row);
}
//解码回调 视频为YUV数据(YV12)，音频为PCM数据
void CALLBACK DecCBFun(long nPort, char * pBuf, long nSize, FRAME_INFO * pFrameInfo, long nReserved1, long nReserved2)
{
	long lFrameType = pFrameInfo->nType;

	if (lFrameType == T_YV12)
	{
#if USECOLOR
		//int start = clock();
		IplImage* pImgYCrCb = cvCreateImage(cvSize(pFrameInfo->nWidth, pFrameInfo->nHeight), 8, 3);//得到图像的Y分量  
		yv12toYUV(pImgYCrCb->imageData, pBuf, pFrameInfo->nWidth, pFrameInfo->nHeight, pImgYCrCb->widthStep);//得到全部RGB图像
		IplImage* pImg = cvCreateImage(cvSize(pFrameInfo->nWidth, pFrameInfo->nHeight), 8, 3);
		cvCvtColor(pImgYCrCb, pImg, CV_YCrCb2RGB);
		//int end = clock();
#else
		IplImage* pImg = cvCreateImage(cvSize(pFrameInfo->nWidth, pFrameInfo->nHeight), 8, 1);
		memcpy(pImg->imageData, pBuf, pFrameInfo->nWidth*pFrameInfo->nHeight);
#endif
		//printf("%d\n",end-start);
		//cvNamedWindow("IPCamera", CV_WINDOW_NORMAL);
		//cvShowImage("IPCamera", pImg);
		if (!EnQueue(&MYQUEUE, pImg))
		{
			cout << "入队失败" << endl;
			return;
		}
		if (QueueLength(&MYQUEUE) > 15)
		{
			IplImage *data = NULL;
			DeQueue(&MYQUEUE, &data);
			cvReleaseImage(&data);
		}
		cvWaitKey(1);
#if USECOLOR
		cvReleaseImage(&pImgYCrCb);
		//cvReleaseImage(&pImg);
#else
		cvReleaseImage(&pImg);
#endif


		//此时是YV12格式的视频数据，保存在pBuf中，可以fwrite(pBuf,nSize,1,Videofile);
		//fwrite(pBuf,nSize,1,fp);
	}
	/***************
	else if (lFrameType ==T_AUDIO16)
	{
	//此时是音频数据，数据保存在pBuf中，可以fwrite(pBuf,nSize,1,Audiofile);

	}
	else
	{

	}
	*******************/

}

///实时流回调  
void CALLBACK fRealDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void *pUser)
{
	DWORD dRet = 0;
	BOOL inData = FALSE;
	switch (dwDataType)
	{
	case NET_DVR_SYSHEAD:    //系统头  
		if (nPort >= 0)
		{
			break; //同一路码流不需要多次调用开流接口
		}
		if (!PlayM4_GetPort(&nPort)) //获取播放库未使用的通道号  
		{
			break;
		}
		if (dwBufSize > 0)
		{
			if (!PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME))  //设置实时流播放模式
			{
				cout << "PlayM4_SetStreamOpenMode failed " << endl;
				break;
			}
			if (!PlayM4_OpenStream(nPort, pBuffer, dwBufSize, 1024 * 1024))   //打开流接口
			{
				cout << "PlayM4_OpenStream failed " << endl;
				dRet = PlayM4_GetLastError(nPort);
				break;
			}
			//设置解码回调函数 只解码不显示  
			if (!PlayM4_SetDecCallBack(nPort, DecCBFun))                    //设置回调函数，替换播放器中的显示部分，由用户自己控制显示
			{
				dRet = PlayM4_GetLastError(nPort);
				break;
			}

			//设置解码回调函数 解码且显示  
			//if (!PlayM4_SetDecCallBackEx(nPort,DecCBFun,NULL,NULL))  
			//{  
			//  dRet=PlayM4_GetLastError(nPort);  
			//  break;  
			//}  

			//打开视频解码  
			if (!PlayM4_Play(nPort, hWnd))
			{
				dRet = PlayM4_GetLastError(nPort);
				break;
			}

			//打开音频解码, 需要码流是复合流  
			/*if (!PlayM4_PlaySound(nPort))
			{
			dRet = PlayM4_GetLastError(nPort);
			break;
			}*/
		}
		break;

	case NET_DVR_STREAMDATA:   //码流数据  
		inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
		while (!inData)
		{
			Sleep(2);
			inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
			cout << "PlayM4_InputData failed 11111" << endl;
			break;
		}
		break;
	default:
		inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
		while (!inData)
		{
			Sleep(2);
			inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
			cout << "PlayM4_InputData failed 22222" << endl;
			break;
		}
		break;
	}
}

void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
	char tempbuf[256] = { 0 };
	switch (dwType)
	{
	case EXCEPTION_RECONNECT:    //预览时重连  
		cout << "----------reconnect--------" << endl;
		break;
	default:
		break;
	}
}
//线程函数，读取摄像头
unsigned __stdcall readCamera(void *param)
{
	//---------------------------------------  
	// 初始化  SDK
	NET_DVR_Init();
	//设置连接时间与重连时间  
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);


	//---------------------------------------  
	// 注册设备  
	LONG lUserID;
	NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };//登录参数，包括设备地址、登录用户、密码等
	NET_DVR_DEVICEINFO_V40 struDeviceInfo = { 0 };//设备信息，输出参数

	strcpy((char *)struLoginInfo.sDeviceAddress, "169.254.27.88"); //设备 IP 地址
	strcpy((char *)struLoginInfo.sUserName, "admin"); //设备登录用户名
	strcpy((char *)struLoginInfo.sPassword, "landian123"); //设备登录密码
	struLoginInfo.wPort = 8000;
	struLoginInfo.bUseAsynLogin = 0; //同步登录，登录接口返回成功即登录成功

	lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfo);
	if (lUserID < 0)
	{
		cout << "NET_DVR_Login_V40 failed, error code: " << NET_DVR_GetLastError() << endl;
		NET_DVR_Cleanup();
		return 0;
	}


	int iRet;
	//获取通道 1 的压缩参数
	DWORD dwReturnLen;
	NET_DVR_COMPRESSIONCFG_V30 struParams = { 0 };
	iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_COMPRESSCFG_V30, 1, &struParams, \
		sizeof(NET_DVR_COMPRESSIONCFG_V30), &dwReturnLen);
	if (!iRet)
	{
		printf("NET_DVR_GetDVRConfig NET_DVR_GET_COMPRESSCFG_V30 error.\n");
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup();
		return 0;
	}

	//设置通道 1 的压缩参数
	struParams.struNormHighRecordPara.dwVideoBitrate = 0.5;

	iRet = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_COMPRESSCFG_V30, 1, \
		&struParams, sizeof(NET_DVR_COMPRESSIONCFG_V30));
	if (!iRet)
	{
		printf("NET_DVR_GetDVRConfig NET_DVR_SET_COMPRESSCFG_V30 error.\n");
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup();
		return 0;
	}
	//获取通道 1 的压缩参数
	iRet = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_COMPRESSCFG_V30, 1, \
		&struParams, sizeof(NET_DVR_COMPRESSIONCFG_V30), &dwReturnLen);
	if (!iRet)
	{
		printf("NET_DVR_GetDVRConfig NET_DVR_GET_COMPRESSCFG_V30 error.\n");
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup();
		return 0;
	}
	printf("Video Bitrate is %d\n", struParams.struNormHighRecordPara.dwVideoBitrate);



	//---------------------------------------  
	//设置异常消息回调函数  
	NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);

	//---------------------------------------
	//启动预览并设置回调数据流
	NET_DVR_PREVIEWINFO StruPlayInfo = { 0 };
	StruPlayInfo.hPlayWnd = NULL;  //窗口为空，设备SDK不解码只取流  
	StruPlayInfo.lChannel = 1;     //预览通道号
	StruPlayInfo.dwStreamType = 0; //0-主流码，1-子流码，2-流码3，3-流码4，以此类推
	StruPlayInfo.dwLinkMode = 0;   //0-TCP方式，1-UDP方式，2-多播方式，3-RTP方式，4-RTP/RTSP，5-RSTP/HTTP
	StruPlayInfo.bBlocked = 1;     //0-非堵塞取流，1-堵塞取流

	LONG lRealPlayHandle;
	lRealPlayHandle = NET_DVR_RealPlay_V40(lUserID, &StruPlayInfo, fRealDataCallBack, NULL);


	if (lRealPlayHandle<0)
	{
		cout << "NET_DVR_RealPlay_V40 failed! Error number:  " << NET_DVR_GetLastError() << endl;
		return 0;
	}

	cout << "The program is successful !!" << endl;
	Sleep(-1);

	//---------------------------------------  
	//关闭预览  
	if (!NET_DVR_StopRealPlay(lRealPlayHandle))
	{
		cout << "NET_DVR_StopRealPlay error! Error number: " << NET_DVR_GetLastError() << endl;
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup();
		return 0;
	}
	//释放播放库资源
	PlayM4_Stop(nPort);
	PlayM4_CloseStream(nPort);
	PlayM4_FreePort(nPort);

	//注销用户  
	NET_DVR_Logout(lUserID);
	NET_DVR_Cleanup();

	return 0;

}
//获得矩形中心
Point getCenterPoint(Rect rect)		
{
	Point cpt;
	cpt.x = rect.x + cvRound(rect.width / 2.0);
	cpt.y = rect.y + cvRound(rect.height / 2.0);
	return cpt;
}



unsigned __stdcall process_people(void *param)
{

	svm_hog_detect();
	
	return -1;
}


void main()
{

	InitQueue(&MYQUEUE);
	HANDLE hGetFrame, hProcess_people; //创建句柄
	unsigned tidGetFrame, tidProcess_people; //记录线程ID地址
											 //创建2个线程
	hGetFrame = (HANDLE)_beginthreadex(NULL, 0, &readCamera, NULL, 0, &tidGetFrame);		//读取摄像头线程
	hProcess_people = (HANDLE)_beginthreadex(NULL, 0, &process_people, NULL, 0, &tidProcess_people);	//图像处理线程

	ExitThread(tidGetFrame);	//结束线程
	ExitThread(tidProcess_people);

}



void svm_hog_detect()
{
	IplImage *img = NULL;
	//HOG检测器，用来计算HOG描述子的  
	//检测窗口(48,48),块尺寸(16,16),块步长(8,8),cell尺寸(8,8),直方图bin个数9  
	cv::HOGDescriptor hog(cv::Size(ImgX, ImgY), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);

	//HOG描述子的维数，由图片大小、检测窗口大小、块大小、细胞单元中直方图bin个数决定 
	int DescriptorDim;

	//从XML文件读取训练好的SVM模型
	//cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::load("D:\\OpenCV\\opencv\\sources\\data\\hogcascades\\SVM_HOG_2400PosINRIA_12000Neg\\SVM_HOG_2400PosINRIA_12000Neg.xml");
	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::load("E:\\大学\\中英3\\李旭\\SVM_HOG.xml");
	if (svm->empty())
	{
		std::cout << "load svm detector failed!!!" << std::endl;
		return;
	}

	//特征向量的维数，即HOG描述子的维数  
	DescriptorDim = svm->getVarCount();

	//获取svecsmat，元素类型为float
	cv::Mat svecsmat = svm->getSupportVectors();
	//特征向量维数
	int svdim = svm->getVarCount();
	int numofsv = svecsmat.rows;

	//alphamat和svindex必须初始化，否则getDecisionFunction()函数会报错
	cv::Mat alphamat = cv::Mat::zeros(numofsv, svdim, CV_32F);
	cv::Mat svindex = cv::Mat::zeros(1, numofsv, CV_64F);

	cv::Mat Result;
	double rho = svm->getDecisionFunction(0, alphamat, svindex);
	//将alphamat元素的数据类型重新转成CV_32F
	alphamat.convertTo(alphamat, CV_32F);
	Result = -1 * alphamat * svecsmat;

	std::vector<float> vec;
	for (int i = 0; i < svdim; ++i)
	{
		vec.push_back(Result.at<float>(0, i));
	}
	vec.push_back(rho);

	//saving HOGDetectorForOpenCV.txt
	std::ofstream fout("HOGDetectorForOpenCV.txt");
	for (int i = 0; i < vec.size(); ++i)
	{
		fout << vec[i] << std::endl;
	}

	hog.setSVMDetector(vec);



	int width = 1280;
	int height = 720;
	Point CenterPoint;	//创建一个Point类用来存放矩形的中心点
	//cv::VideoWriter out;
	//char saveName[256];//剪裁出来的hard example图片的文件名
	//int hardExampleCount = 0; //hard example计数 
	//用于保存检测结果
	//out.open("test result.avi", CV_FOURCC('D', 'I', 'V', 'X'), 25.0, cv::Size(width / 2, height / 2), true);
	while (1)	
	{
		//	先判断队列长度是否为0
		while (QueueLength(&MYQUEUE) <= 0)
		{
			Sleep(8);
		}
		//capture >> frame;

		DeQueue(&MYQUEUE, &img);	//出队获取图像
		Mat frame = cvarrToMat(img, 1);	//格式转换
		//Mat frame = imread("E:\\Pictures\\Camera Roll\\447FA072CBDCC89F45ED5F805B8BA0D2.jpg",1);
		width = frame.cols;
		height = frame.rows;
		Mat src=frame;	//储存灰度前的图像
		cv::resize(src, src, cv::Size(width / 2, height / 2));
		cv::resize(frame, frame, cv::Size(width / 2, height / 2));
		cv::cvtColor(frame, frame, CV_RGB2GRAY);	//灰度转换
		//目标矩形框数组 
		std::vector<cv::Rect> found, found_1, found_filtered;
		//对图片进行多尺度检测
		hog.detectMultiScale(frame, found, 0, cv::Size(8, 8), cv::Size(16, 16), 1.2, 2);

		for (int i = 0; i<found.size(); i++)
		{
			if (found[i].x > 0 && found[i].y > 0 && (found[i].x + found[i].width)< frame.cols
				&& (found[i].y + found[i].height)< frame.rows)
				found_1.push_back(found[i]);
		}

		//找出所有没有嵌套的矩形框r,并放入found_filtered中,如果有嵌套的话,
		//则取外面最大的那个矩形框放入found_filtered中  
		for (int i = 0; i < found_1.size(); i++)
		{
			cv::Rect r = found_1[i];
			int j = 0;
			for (; j < found_1.size(); j++)
				if (j != i && (r & found_1[j]) == found_1[j])
					break;
			if (j == found_1.size())
				found_filtered.push_back(r);
		}

		//画矩形框，因为hog检测出的矩形框比实际目标框要稍微大些,所以这里需要做一些调整，可根据实际情况调整  
		for (int i = 0; i<found_filtered.size(); i++)
		{
			cv::Rect r = found_filtered[i];
			//将检测矩形框缩小后绘制，根据实际情况调整
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.1);
			r.height = cvRound(r.height*0.8);

			CenterPoint = getCenterPoint(r);	//得到矩阵中心坐标
			/*cout << "矩形x:" << CenterPoint.x;
			cout << "    矩形y:" << CenterPoint.y << endl;*/
			cout << "人数:" << found_filtered.size() << endl;
			circle(src, Point(CenterPoint.x, CenterPoint.y), 3, Scalar(0, 0, 255), -1);
			//将矩形框保存为图片，就是Hard Example  
		/*	Mat hardExampleImg = frame(r);//从原图上截取矩形框大小的图片  
			resize(hardExampleImg, hardExampleImg, Size(ImgX, ImgY));//将剪裁出来的图片缩放为ImgX*ImgY大小  
			sprintf(saveName, "hardexample%09d.jpg", hardExampleCount++);//生成hard example图片的文件名  
			imwrite(saveName, hardExampleImg);//保存文件	*/

		}

		for (int i = 0; i<found_filtered.size(); i++)
		{
			cv::Rect r = found_filtered[i];

			cv::rectangle(src, r.tl(), r.br(), cv::Scalar(0, 0, 255), 2);
		}
		//cvNamedWindow("detect result", CV_WINDOW_NORMAL);
		cv::imshow("detect result", src);
		//保存检测结果
		//out << frame;
		if (cv::waitKey(30) == 'q')
		{
			break;
		}
		cvReleaseImage(&img);
	}
	//out.release();
	return;
}