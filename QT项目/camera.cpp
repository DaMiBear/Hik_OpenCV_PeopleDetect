#include <camera.h>
#include "queue.h"
LONG nPort = -1;
using namespace cv;
using namespace std;
#define USECOLOR 1
HWND hWnd = NULL;
LONG lUserID2;
LONG lRealPlayHandle2;

LinkQueue  MYQUEUE;		//创建队列
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
//        cvNamedWindow("IPCamera", CV_WINDOW_NORMAL);
//        cvShowImage("IPCamera", pImg);
        if (!EnQueue(&MYQUEUE, pImg))
        {
            cout << "EnQueue Fail!" << endl;
            return ;
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
    }

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

int Init_Camera()
{
    InitQueue(&MYQUEUE);//队列初始化
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
        lUserID2 = lUserID;
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
        lRealPlayHandle2=lRealPlayHandle;

        if (lRealPlayHandle<0)
        {
            cout << "NET_DVR_RealPlay_V40 failed! Error number:  " << NET_DVR_GetLastError() << endl;
            return 0;
        }

        cout << "The program is successful !!" << endl;
        return 0;

}
int Close_Camera()
{
    //---------------------------------------
    //关闭预览
    if (!NET_DVR_StopRealPlay(lRealPlayHandle2))
    {
        cout << "NET_DVR_StopRealPlay error! Error number: " << NET_DVR_GetLastError() << endl;
        NET_DVR_Logout(lUserID2);
        NET_DVR_Cleanup();
        return 0;
    }
    //释放播放库资源
    PlayM4_Stop(nPort);
    PlayM4_CloseStream(nPort);
    PlayM4_FreePort(nPort);

    //注销用户
    NET_DVR_Logout(lUserID2);
    NET_DVR_Cleanup();

    return 0;
}
