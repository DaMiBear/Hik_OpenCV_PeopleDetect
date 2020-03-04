#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include "Windows.h"
#include "HCNetSDK.h"
#include "plaympeg4.h"

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <opencv2\opencv.hpp>
#include "cv.h"

int Init_Camera();
int Close_Camera();

#endif // CAMERA_H
