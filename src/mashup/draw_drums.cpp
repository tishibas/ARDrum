#include "common.h"
#include "GLMetaseq.h"	// モデルローダ
#include "procImg.h"
#include "Drum.h"


struct Model{
	MQO_MODEL mqo;
	int state;
	Model(){
		state = 0;
	}
};

Model drumModel, cymbalModel;	// MQOモデル

Detector *pDetector;
Drum *pDrum;


//CV
CvCapture *capture=NULL;
IplImage *frameImage=NULL;

// プロトタイプ宣言
void mySetLight(void);
void Draw(void);
void Reshape (int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Quit(void);
void renderModel(Model &model, float x, float y, float z, float a);

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
    


	{
		// detect marker

		// detect action
		Mat frame(frameImage);
		pDetector->init(frame);

		// スティック検出
		vector<iVec2> act;
		pDetector->detectAction( act );

		{// 検出結果描画
			for( int i = 0; i < act.size(); i++ ){
				int x = act[i].x, y = act[i].y;
				line( frame, Point(x-15, y) , Point(x+15, y), Scalar(255, 255, 0), 4, CV_AA );
				line( frame, Point(x, y-15) , Point(x, y+15), Scalar(255, 255, 0), 4, CV_AA );
				circle( frame, Point(x, y), 20, Scalar(255, 255, 255), 2 );
			}
		}
		*frameImage = frame;



		// out sound
		if(act.size() > 0){
			pDrum->tataku(SCRATCH_PUSH);
		}
		

	}

	


	cvCvtColor(frameImage, copyImage, CV_BGR2RGBA ); // OpenGL用に画素の並びを変換
    cvFlip( copyImage, copyImage, 0 ); // OpenGLの原点に合わせて上下反転
	
	glMatrixMode(GL_MODELVIEW);

	glEnable( GL_DEPTH_TEST );		// 隠面処理の適用
	mySetLight();					// 光源の設定
	glEnable( GL_LIGHTING );		// 光源ON

	// OpenCV の画像データを描画（キャプチャした画像を背景として描画）
	glDrawPixels( copyImage->width, copyImage->height, GL_RGBA, GL_UNSIGNED_BYTE, copyImage->imageData );
	glClear( GL_DEPTH_BUFFER_BIT ); // デプスバッファのみをクリア



	{// renderModel
		//if(aciont)
		{
			//g_mqoModel.state = 20;

		}
		renderModel( drumModel,   -160, -100, -400, 10 );
		renderModel( drumModel,    -80, -100, -400, 10 );
		renderModel( drumModel,     80, -100, -400, 10 );
		renderModel( drumModel,    160, -100, -400, 10 );
		renderModel( cymbalModel, -200,    0, -400, 10 );
		renderModel( cymbalModel,  200,    0, -400, 10 );
	}

	glutSwapBuffers();
	cvReleaseImage( &copyImage );
}


// main関数
int main(int argc, char *argv[])
{
	capture = cvCreateCameraCapture( -1 );
	frameImage = cvQueryFrame( capture ); // カメラ画像の取得

	Mat frame(frameImage);

	// 初期設定 
	pDetector = new Detector( frame.cols, frame.rows, 1000.0);
	pDrum = new Drum();



	// 初期化
	glutInit(&argc,argv);										// OpenGL初期化
	glutInitWindowPosition(100, 50);							// ウィンドウの表示位置
	glutInitWindowSize(640, 480);								// ウィンドウのサイズ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// ディスプレイ設定
	glutCreateWindow("MQO Loader for OpenGL");					// ウィンドウの生成

	// モデルを表示させる準備
	mqoInit();											// GLMetaseqの初期化
	drumModel.mqo = mqoCreateModel("drum.mqo",1.0);		// モデルのロード
	cymbalModel.mqo = mqoCreateModel("cymbal.mqo",1.0);		// モデルのロード

	// 終了処理関数の設定
	atexit(Quit);

	// コールバック関数の設定
	glutDisplayFunc(Draw);			// 描画処理関数の設定
	glutIdleFunc(Draw);				// アイドル時の処理関数の設定
	glutReshapeFunc(Reshape);		// ウィンドウ変形時の処理を行う関数の設定
	glutKeyboardFunc(Keyboard);		// キー入力時の処理関数の設定

	// イベント待ちの無限ループに入る
	glutMainLoop();

    // OpenCV後片付け
    cvReleaseCapture( &capture );

	return 0;
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


// 終了処理関数
void Quit(void)
{
	mqoDeleteModel( drumModel.mqo );	// モデルの削除
	mqoDeleteModel( cymbalModel.mqo );	// モデルの削除
	mqoCleanup();					// GLMetaseqの終了処理
}
