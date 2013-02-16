#define _USE_MATH_DEFINES


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <Windows.h>
#include <vector>

#include "opencv2\opencv.hpp"
#ifdef _DEBUG 
//Debugモードの場合
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_core240d.lib")    
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_imgproc240d.lib")  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_highgui240d.lib")  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_objdetect240d.lib")
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_contrib240d.lib")   
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_features2d240d.lib")
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_flann240d.lib")   
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_gpu240d.lib")   
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_haartraining_engined.lib") 
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_legacy240d.lib")  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_ts240d.lib")  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_video240d.lib")
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_calib3d240d.lib")
#else    //Releaseモードの場合  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_core240.lib")  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_imgproc240.lib")  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_highgui240.lib")  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_objdetect240.lib")
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_contrib240.lib")  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_features2d240.lib") 
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_flann240.lib")  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_gpu240.lib")   
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_haartraining_engined.lib")  
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_legacy240.lib")   
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_ts240.lib")   
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_video240.lib")
#pragma comment(lib,"C:\\work\\cv\\opencv\\build\\x86\\vc10\\lib\\opencv_calib3d240.lib")
#endif
using namespace cv;
using namespace std;

