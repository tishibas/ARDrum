#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <vector>
#include <Windows.h>
#include <time.h>

#include <opencv2/opencv.hpp>
#include <glut.h>
#include <MIDIIO.h>
#ifdef _DEBUG 
//Debugモードの場合private\\program\\opencv
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_core244d.lib")    
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_imgproc244d.lib")  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_highgui244d.lib")  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_objdetect244d.lib")
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_contrib244d.lib")   
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_features2d244d.lib")
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_flann244d.lib")   
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_gpu244d.lib")   
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_haartraining_engined.lib") 
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_legacy244d.lib")  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_ts244d.lib")  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_video244d.lib")
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_calib3d244d.lib")
#pragma comment(lib,"C:\\private\\program\\midi\\MIDIIOLib0.8\\Debug\\MIDIIOd.lib")
#else    //Releaseモードの場合  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_core244.lib")  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_imgproc244.lib")  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_highgui244.lib")  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_objdetect244.lib")
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_contrib244.lib")  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_features2d244.lib") 
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_flann244.lib")  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_gpu244.lib")   
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_haartraining_engined.lib")  
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_legacy244.lib")   
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_ts244.lib")   
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_video244.lib")
#pragma comment(lib,"C:\\private\\program\\opencv\\opencv\\build\\x86\\vc10\\lib\\opencv_calib3d244.lib")
#pragma comment(lib,"C:\\private\\program\\midi\\MIDIIOLib0.8\\Release\\MIDIIO.lib")
#endif

#pragma comment(lib,"C:\\private\\program\\glut\\glut-3.7.6-bin\\glut32.lib")


using namespace cv;
using namespace std;

