
//プロジェクトのプロパティ⇒C/C++⇒全般　の追加のインクルードディレクトリに
// opencv2のあるフォルダ『C:\OpenCV\include』などを追加のこと
#include "opencv2\opencv.hpp"
 
#ifdef _DEBUG
    //Debugモードの場合
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_core240d.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_imgproc240d.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_highgui240d.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_objdetect240d.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_contrib240d.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_features2d240d.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_flann240d.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_gpu240d.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_haartraining_engined.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_legacy240d.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_ts240d.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_video240d.lib")
#else
    //Releaseモードの場合
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_core240.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_imgproc240.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_highgui240.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_objdetect240.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_contrib240.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_features2d240.lib")

#pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_flann240.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_gpu240.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_haartraining_engined.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_legacy240.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_ts240.lib")
    #pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_video240.lib")
#endif
 
#include <windows.h>

#include <GL/gl.h>

#include <GL/glut.h> // for glutSolidCube()

#include <iostream>
#include "XLoader.h"
#include "Mouse.h"
using namespace std;

//
// Global Variable
//
int WindowPositionX = 100;
int WindowPositionY = 100;
int WindowWidth = 512;
int WindowHeight = 512;
char WindowTitle[] = "★　Xファイルの読み込み　★";
char Version[50];
char Vender[50];
char Renderer[50];
char FpsString[50] = {0};
double CurrentTime = 0.0;
double LastTime = 0.0;
double CurrentCount = 0.0;
double LastCount = 0.0;
int FrameCount = 0;
float Fps = 0.0f;
XModel model;
ViewCamera camera;

//
// Forward declarations
//
void Render3D();



// OpenGLのためのフォーマット指定

static PIXELFORMATDESCRIPTOR pfd = {

  sizeof (PIXELFORMATDESCRIPTOR),

  1,

  PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,

  PFD_TYPE_RGBA,

  24,

  0, 0, 0,

  0, 0, 0,

  0, 0,

  0, 0, 0, 0, 0,

  32,

  0,

  0,

  PFD_MAIN_PLANE,

  0,

  0,

  0,

  0

};

  

int main( int argc, char *argv[] )

{

  CvCapture *capture=NULL;

  IplImage *frameImage=NULL, *copyImage=NULL, *outputImage=NULL;

  // モデルファイルの読み込み
  model.Load("Models/dosei.x");

  int key;

  int count=0;

  HWND hwnd;

  HDC hdc;

  HGLRC hglrc;

  int pfdID;

  GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};

  GLfloat light_position[] = {3.0, 3.0, 3.0, 0.0};

  

  capture = cvCreateCameraCapture( -1 );

  

  frameImage = cvQueryFrame( capture ); // カメラ画像の取得

  cvNamedWindow( "OpenCV", CV_WINDOW_AUTOSIZE ); // ウインドウ作成

  cvResizeWindow( "OpenCV", frameImage->width, frameImage->height );

  // 画像バッファ作成

  copyImage = cvCreateImage( cvSize(frameImage->width, frameImage->height), 8, 4 ); // RGBA の4バイト

  outputImage = cvCreateImage( cvSize(frameImage->width, frameImage->height), 8, 3 ); // BGR の3バイト

  

  hwnd = (HWND)cvGetWindowHandle( "OpenCV" ); // ウインドウハンドルを取得

  hdc = GetDC(hwnd); // デバイスコンテキストを取得

  pfdID = ChoosePixelFormat(hdc, &pfd);

  SetPixelFormat(hdc, pfdID, &pfd); // デバイスコンテキストにピクセルフォーマットを設定

  hglrc = wglCreateContext(hdc); // OpenGL用のコンテキストを作成

  wglMakeCurrent(hdc, hglrc); // ここからOpwnGLコマンドが有効になる

 
  

  // 光源設定

  //glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

  //glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  //glEnable(GL_LIGHTING);

  //glEnable(GL_LIGHT0);

  // 陰面処理を有効に

  glEnable(GL_DEPTH_TEST);

  

  while( 1 ) {

    frameImage = cvQueryFrame( capture );

    cvCvtColor(frameImage, copyImage, CV_BGR2RGBA ); // OpenGL用に画素の並びを変換

    cvFlip( copyImage, copyImage, 0 ); // OpenGLの原点に合わせて上下反転

	//　バックバッファをクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//　モデルビュー行列の設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


    // OpenCV の画像データを描画

    glDrawPixels( copyImage->width, copyImage->height, GL_RGBA, GL_UNSIGNED_BYTE, copyImage->imageData ); //キャプチャした画像を背景として描画

  

    glClear( GL_DEPTH_BUFFER_BIT ); // デプスバッファのみをクリア

  
	glOrtho( -4.0, 4.0, -3.0, 3.0, -5.0, 5.0 ); // とりあえず 横:縦=4:3 で正射影

  

	//　視点の設定
	camera.SetParams();

	camera.Set();
  

	//
	glPushMatrix();
	
	//　3Dシーンの描画
	Render3D();	

	//　2Dシーンの描画
	//Render2D();

	//
	glPopMatrix();

	//　ダブルバッファ
	//glutSwapBuffers();



    // OpenGLバッファの内容を画像として取得

    glReadPixels( 0, 0, outputImage->width, outputImage->height, GL_RGB, GL_UNSIGNED_BYTE, outputImage->imageData ); // RGBで取得

    cvCvtColor( outputImage, outputImage, CV_RGB2BGR ); // OpenCVのBGR並びに変換

    cvFlip( outputImage, outputImage, 0 ); // OpenCV に合わせて上下反転

  

    cvShowImage( "OpenCV", outputImage ); // 画像表示

  

    key = cvWaitKey(10);

    if( key == 27 ){

      break;

    }

  }

  

  wglMakeCurrent(hdc, 0);

  wglDeleteContext(hglrc);

  ReleaseDC(hwnd, hdc);

  cvReleaseImage( &copyImage );
  cvReleaseImage( &outputImage );

  cvDestroyWindow( "OpenCV" );

  cvReleaseCapture( &capture );

  

  return 0;

}





//--------------------------------------------------------------------------------------------------
// Name : Render3D()
// Desc : 3次元シーンの描画
//--------------------------------------------------------------------------------------------------
void Render3D()
{
	//　モデルの描画
	float scale = 1.0f/model.sphere.radius;	//　拡大係数
	model.Render(scale);	//　描画
}

