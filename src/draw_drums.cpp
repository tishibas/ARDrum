#include "common.h"
#include "GLMetaseq.h"	// モデルローダ
#include "procImg.h"
#include "Drum.h"
int mode = 0;

struct Model{
	MQO_MODEL mqo;
	int state;
	Model(){
		state = 0;
	}
};

int actFlg[6];
Model drumL2Model, drumL1Model, drumR1Model, drumR2Model, cymbalLModel, cymbalRModel, drumActModel, cymbalActModel;	// MQOモデル


Detector *pDetector;
Drum *pDrum;
vector<Marker> *mrkPos;
vector<int> act;

//CV
CvCapture *capture=NULL;
IplImage *frameImage=NULL;

// プロトタイプ宣言
void mySetLight(void);
void Draw(void);
void Reshape (int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Quit(void);
void renderModel(Model &model, Model &actModel, float x, float y, float z, float a, int actFlg);

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

	cvFlip( frameImage, frameImage, 1);
	
	//if(!mode)frameImage = cvLoadImage( "view000.bmp" ); 
    


	if(0){
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
			pDrum->tataku(LOW_TOM_1);
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
			// モデルの表示位置{ x, y, z, a }
			// 決め打ちで書いてるが実際はこの座標と角度をマーカーで取得したものにする！
			float sym_l[]   = {-200,    0, -500, 10};
			float drum_l2[] = {-150, -100, -450, 10};
			float drum_l1[] = { -50, -100, -400, 10};
			float drum_r1[] = {  50, -100, -400, 10};
			float drum_r2[] = { 150, -100, -450, 10};
			float sym_r[]   = { 200,    0, -500, 10};

			float *pDrumSet[100];
			{
				pDrumSet[0] = sym_l;
				pDrumSet[1] = drum_l2;
				pDrumSet[2] = drum_l1;
				pDrumSet[3] = drum_r1;
				pDrumSet[4] = drum_r2;
				pDrumSet[5] = sym_r;
			}
			
			Mat frame(frameImage);

		if(mode==0)
		{
			// マーカー検出モード
			pDetector->init(frame);
			pDetector->getMarkerPos();

			mrkPos = &pDetector->m_marker;
			for(int i = 0; i < mrkPos->size(); i++ ){
				pDrumSet[i][0] =  (*mrkPos)[i].pos.x;
				pDrumSet[i][1] =  (*mrkPos)[i].pos.y;
				pDrumSet[i][2] = -(*mrkPos)[i].pos.z;
				pDrumSet[i][3] =  (*mrkPos)[i].angle * 180/3.14;
			}	
			if( mrkPos->size() != 6 ){
				for(int i = 0; i < mrkPos->size(); i++ ){
					renderModel( drumL2Model,  drumActModel, pDrumSet[i][0], pDrumSet[i][1], pDrumSet[i][2], pDrumSet[i][3], 0 );
				}

			}
			else{
				renderModel( cymbalLModel, cymbalActModel, sym_l[0],   sym_l[1],   sym_l[2],   sym_l[3], actFlg[0] );
				renderModel( drumL2Model,  drumActModel, drum_l2[0], drum_l2[1], drum_l2[2], drum_l2[3], actFlg[1] );
				renderModel( drumL1Model,  drumActModel, drum_l1[0], drum_l1[1], drum_l1[2], drum_l1[3], actFlg[2] );
				renderModel( drumR1Model,  drumActModel, drum_r1[0], drum_r1[1], drum_r1[2], drum_r1[3], actFlg[3] );
				renderModel( drumR2Model,  drumActModel, drum_r2[0], drum_r2[1], drum_r2[2], drum_r2[3], actFlg[4] );
				renderModel( cymbalRModel, cymbalActModel, sym_r[0],   sym_r[1],   sym_r[2],   sym_r[3], actFlg[5] );
			}

		}else{
			pDetector->init(frame);
			vector<iVec2> dAction;

			pDetector->detectAction(dAction);

			for(int i = 0; i < dAction.size(); i++ ){
				int xPos = dAction[i].x;
				int yPos = dAction[i].y;
				int id;
				int maxDiff = 640;
				for( int j=0; j < 6;j++){
					int dX = ((*mrkPos)[j].center.x - xPos );
					int dY = ((*mrkPos)[j].center.y - yPos );
					double diff = sqrt((double)dX*dX+dY*dY);

					if( diff < maxDiff ){
						maxDiff = diff;
						id = j;
					}
				}
				if(maxDiff < 150)
				act.push_back(id);
			}

			for(int i = 0; i < act.size(); i++){
				int n = act[i];
				actFlg[n] = 1;
				switch(n){
					case 5:
					pDrum->tataku(OPEN_HIHAT);
					break;
					case 1:
					pDrum->tataku(HIGH_TOM_1);
					break;
					case 2:
					pDrum->tataku(LOW_TOM_1);
					break;
					case 3:
					pDrum->tataku(LOW_TOM_2);
					break;
					case 4:
					pDrum->tataku(MID_TOM_1);
					break;
					case 0:
					pDrum->tataku(CLOSED_HIHAT);
					break;
					default:
						break;

				}
			}


			for(int i = 0; i < 6; i++ ){
				pDrumSet[i][0] =  (*mrkPos)[i].pos.x;
				pDrumSet[i][1] =  (*mrkPos)[i].pos.y;
				pDrumSet[i][2] = -(*mrkPos)[i].pos.z;
				pDrumSet[i][3] =  (*mrkPos)[i].angle * 180/3.14;
			}
			// 叩かれたモデルのactFlgを1にして渡す（複数指定可）
			renderModel( cymbalLModel, cymbalActModel, sym_l[0],   sym_l[1],   sym_l[2],   sym_l[3], actFlg[0] );
			renderModel( drumL2Model,  drumActModel, drum_l2[0], drum_l2[1], drum_l2[2], drum_l2[3], actFlg[1] );
			renderModel( drumL1Model,  drumActModel, drum_l1[0], drum_l1[1], drum_l1[2], drum_l1[3], actFlg[2] );
			renderModel( drumR1Model,  drumActModel, drum_r1[0], drum_r1[1], drum_r1[2], drum_r1[3], actFlg[3] );
			renderModel( drumR2Model,  drumActModel, drum_r2[0], drum_r2[1], drum_r2[2], drum_r2[3], actFlg[4] );
			renderModel( cymbalRModel, cymbalActModel, sym_r[0],   sym_r[1],   sym_r[2],   sym_r[3], actFlg[5] );

			for(int i=0;i<6; i++) actFlg[i] = 0;
			act.clear();

		}
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
	pDetector = new Detector( frame.cols, frame.rows, 500.0);
	pDrum = new Drum();



	// 初期化
	glutInit(&argc,argv);										// OpenGL初期化
	glutInitWindowPosition(100, 50);							// ウィンドウの表示位置
	glutInitWindowSize(640, 480);								// ウィンドウのサイズ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// ディスプレイ設定
	glutCreateWindow("MQO Loader for OpenGL");					// ウィンドウの生成

	// モデルを表示させる準備
	mqoInit();											// GLMetaseqの初期化
	// モデルのロード
	drumL2Model.mqo = mqoCreateModel("drum.mqo",1.0);
	drumL1Model.mqo = mqoCreateModel("drum.mqo",1.0);
	drumR1Model.mqo = mqoCreateModel("drum.mqo",1.0);
	drumR2Model.mqo = mqoCreateModel("drum.mqo",1.0);
	drumActModel.mqo = mqoCreateModel("drumAct.mqo",1.0);
	cymbalLModel.mqo = mqoCreateModel("cymbal.mqo",1.0);
	cymbalRModel.mqo = mqoCreateModel("cymbal.mqo",1.0);	
	cymbalActModel.mqo = mqoCreateModel("cymbalAct.mqo",1.0);	

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
		case 'a': actFlg[0]=1; break;
		case 's': actFlg[1]=1; break;
		case 'd': actFlg[2]=1; break;
		case 'f': actFlg[3]=1; break;
		case 'g': actFlg[4]=1; break;
		case 'h': actFlg[5]=1; break;
		case 'q': actFlg[2]=1; actFlg[5]=1; break;
		case 'm': mode=0; break;
		case 'p': mode=1; break;
		case '0': act.push_back(0); break;
		case '1': act.push_back(1); break;
		case '2': act.push_back(2); break;
		case '3': act.push_back(3); break;
		case '4': act.push_back(4); break;
		case '5': act.push_back(5); break;
		case 'Q':
		case 0x1b:
			Quit();	// （このあと終了処理関数が呼ばれる）
		default:
			break;
	}
}


// 終了処理関数
void Quit(void)
{
	// モデルの削除
	mqoDeleteModel( drumL2Model.mqo );
	mqoDeleteModel( drumL1Model.mqo );	
	mqoDeleteModel( drumR1Model.mqo );	
	mqoDeleteModel( drumR2Model.mqo );	
	mqoDeleteModel( drumActModel.mqo );	
	mqoDeleteModel( cymbalLModel.mqo );
	mqoDeleteModel( cymbalRModel.mqo );
	mqoDeleteModel( cymbalActModel.mqo );

	// GLMetaseqの終了処理
	mqoCleanup();			
}
