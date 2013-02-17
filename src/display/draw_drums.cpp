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
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "GLMetaseq.h"	// モデルローダ


// グローバル変数
int g_mouseX = 0;	// マウスX座標
int g_mouseY = 0;	// マウスY座標
int g_rotX = 10.0f; // X軸周りの回転
int	g_rotY = 10.0f;		// Y軸周りの回転

MQO_MODEL g_mqoModel, g_mqoModel2;	// MQOモデル

//CV
CvCapture *capture=NULL;
IplImage *frameImage=NULL;

// プロトタイプ宣言
void mySetLight(void);
void Draw(void);
void Reshape (int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);


// 光源の設定を行う関数
void mySetLight(void)
{
	GLfloat light_diffuse[]  = { 0.9, 0.9, 0.9, 1.0 };	// 拡散反射光
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// 鏡面反射光
	GLfloat light_ambient[]  = { 0.3, 0.3, 0.3, 0.1 };	// 環境光
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };	// 位置と種類

	// 光源の設定
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  light_diffuse );	 // 拡散反射光の設定
	glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular ); // 鏡面反射光の設定
	glLightfv( GL_LIGHT0, GL_AMBIENT,  light_ambient );	 // 環境光の設定
	glLightfv( GL_LIGHT0, GL_POSITION, light_position ); // 位置と種類の設定

	glShadeModel( GL_SMOOTH );	// シェーディングの種類の設定
	glEnable( GL_LIGHT0 );		// 光源の有効化
}


// 描画関数
void Draw(void)
{
	// 画像バッファ作成
	IplImage *copyImage = cvCreateImage( cvSize(frameImage->width, frameImage->height), 8, 4 ); // RGBA の4バイト

	frameImage = cvQueryFrame( capture );
    cvCvtColor(frameImage, copyImage, CV_BGR2RGBA ); // OpenGL用に画素の並びを変換
    cvFlip( copyImage, copyImage, 0 ); // OpenGLの原点に合わせて上下反転

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 初期化
	//glClearColor(0.f, 0.f, 0.2f, 1.0);					// 背景色
	glMatrixMode(GL_MODELVIEW);

	glEnable( GL_DEPTH_TEST );		// 隠面処理の適用
	mySetLight();					// 光源の設定
	glEnable( GL_LIGHTING );		// 光源ON

	// OpenCV の画像データを描画
	glDrawPixels( copyImage->width, copyImage->height, GL_RGBA, GL_UNSIGNED_BYTE, copyImage->imageData ); //キャプチャした画像を背景として描画
	glClear( GL_DEPTH_BUFFER_BIT ); // デプスバッファのみをクリア
	cvReleaseImage( &copyImage );

	glPushMatrix();
		glTranslatef(120.0, -100.0, -650.0); // 平行移動（横、縦、奥行き）
		glRotatef( g_rotX, 1, 0, 0 );		 // X軸回転
		glRotatef( g_rotY, 0, 1, 0 );		 // Y軸回転
		//glRotatef( 20.0f, 0, 0, 1 );		 // Z軸回転
		mqoCallModel(g_mqoModel);			 // MQOモデルのコール
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-120.0, -100.0, -650.0); // 平行移動（横、縦、奥行き）
		glRotatef( g_rotX, 1, 0, 0 );		 // X軸回転
		glRotatef( g_rotY, 0, 1, 0 );		 // Y軸回転
		//glRotatef( -20.0f, 0, 0, 1 );		 // Z軸回転
		mqoCallModel(g_mqoModel);			 // MQOモデルのコール
	glPopMatrix();

	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );

	glutSwapBuffers();
}


// ウィンドウ変形時に呼ばれる関数
void Reshape (int w, int h)
{
	double	znear = 10;
	double	fovy = 50;
	double	zfar = 10000;

	// ビューポート設定
	glViewport(0, 0, w, h);

	// 射影設定
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (double)w/h, znear, zfar);
}


// キーイベント関数
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 'q':
		case 'Q':
		case 0x1b:
			exit(0);	// （このあと終了処理関数が呼ばれる）
		default:
			break;
	}
}


// マウスイベント関数
void Mouse(int button, int state, int x, int y)
{
	if ( state == GLUT_DOWN ) {
		g_mouseX = x;
		g_mouseY = y;				
	}
}


// マウスドラッグの処理関数
void Motion(int x, int y)
{
	int xd, yd;

	// マウス移動量の算出
	xd = x - g_mouseX;
	yd = y - g_mouseY;

	// 回転量の設定
	g_rotX += yd;
	g_rotY += xd;

	// マウス座標の更新
	g_mouseX = x;
	g_mouseY = y;
}


// 終了処理関数
void Quit(void)
{
	mqoDeleteModel( g_mqoModel );	// モデルの削除
	mqoCleanup();					// GLMetaseqの終了処理
}


// main関数
int main(int argc, char *argv[])
{
	capture = cvCreateCameraCapture( -1 );
	frameImage = cvQueryFrame( capture ); // カメラ画像の取得

	// 初期化
	glutInit(&argc,argv);										// OpenGL初期化
	glutInitWindowPosition(100, 50);							// ウィンドウの表示位置
	glutInitWindowSize(640, 480);								// ウィンドウのサイズ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// ディスプレイ設定
	glutCreateWindow("MQO Loader for OpenGL");					// ウィンドウの生成

	// モデルを表示させる準備
	mqoInit();											// GLMetaseqの初期化
	g_mqoModel = mqoCreateModel("drum.mqo",1.0);		// モデルのロード
	g_mqoModel2 = mqoCreateModel("drum.mqo",1.0);		// モデルのロード

	// 終了処理関数の設定
	atexit(Quit);

	// コールバック関数の設定
	glutDisplayFunc(Draw);			// 描画処理関数の設定
	glutIdleFunc(Draw);				// アイドル時の処理関数の設定
	glutReshapeFunc(Reshape);		// ウィンドウ変形時の処理を行う関数の設定
	glutKeyboardFunc(Keyboard);		// キー入力時の処理関数の設定
	glutMouseFunc(Mouse);			// マウス入力時の処理関数の設定
	glutMotionFunc(Motion);			// マウスドラッグ時の処理関数の設定

	// イベント待ちの無限ループに入る
	glutMainLoop();

    // OpenCV後片付け
    cvReleaseCapture( &capture );

	return 0;
}
