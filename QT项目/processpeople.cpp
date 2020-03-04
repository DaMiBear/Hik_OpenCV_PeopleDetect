#include "processpeople.h"
#define ImgX 64
#define ImgY 128
extern LinkQueue  MYQUEUE;		//创建队列
//获得矩形中心
Point getCenterPoint(Rect rect)
{
    Point cpt;
    cpt.x = rect.x + cvRound(rect.width / 2.0);
    cpt.y = rect.y + cvRound(rect.height / 2.0);
    return cpt;
}

//HOG检测器，用来计算HOG描述子的
//检测窗口(48,48),块尺寸(16,16),块步长(8,8),cell尺寸(8,8),直方图bin个数9
cv::HOGDescriptor hog(cv::Size(ImgX, ImgY), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);
MySignal *a = new MySignal;
Mat ProcessPeople()
{
static bool once=true;
if(once)
{
        //HOG描述子的维数，由图片大小、检测窗口大小、块大小、细胞单元中直方图bin个数决定
        int DescriptorDim;

        //从XML文件读取训练好的SVM模型
        cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::load("D:\\OpenCV\\opencv\\sources\\data\\hogcascades\\SVM_HOG_2400PosINRIA_12000Neg\\SVM_HOG_2400PosINRIA_12000Neg.xml");
        //cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::load("D:\\OpenCV\\opencv\\sources\\data\\hogcascades\\SVM_HOG_2400PosINRIA_12000Neg\\SVM_HOG_2400PosINRIA_12000Neg_HardExample.xml");

        if (svm->empty())
        {
            std::cout << "load svm detector failed!!!" << std::endl;
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

    }
    once = false;
        int width = 1280;
        int height = 720;
        Point CenterPoint;	//创建一个Point类用来存放矩形的中心点
        IplImage *img = NULL;

            //	先判断队列长度是否为0
            while (QueueLength(&MYQUEUE) <= 0)
            {
                Sleep(8);
            }
            //capture >> frame;

            DeQueue(&MYQUEUE, &img);	//出队获取图像
            Mat frame = cvarrToMat(img, 1);	//格式转换
            cv::resize(frame, frame, cv::Size(width / 2, height / 2));
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
             a->sendSignal(0,0,found_filtered.size());//当坐标都为0时，只发送人数
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
//                cout << "矩形x:" << CenterPoint.x;
//                cout << "    矩形y:" << CenterPoint.y << endl;
                circle(frame, Point(CenterPoint.x, CenterPoint.y), 3, Scalar(0, 0, 255), -1);

                a->sendSignal(CenterPoint.x,CenterPoint.y,found_filtered.size());
            }

            for (int i = 0; i<found_filtered.size(); i++)
            {
                cv::Rect r = found_filtered[i];

                cv::rectangle(frame, r.tl(), r.br(), cv::Scalar(0, 0, 255), 2);

            }

            cvReleaseImage(&img);

//        out.release();
        return frame;
}
