#ifndef PROCIMG_H
#define PROCIMG_H

using namespace cv;

struct iVec2{
	int x, y;
};

class Detector{
	int w, h;	// 画像サイズ
	float f;	// 焦点

	Mat *srcRGB;	//入力画像のポインタ
	Mat srcGray;	//入力画像のグレー
	Mat preGray;	//一個前のグレー

	//動きマップの座標軸ヒストグラム 2色
	int *histH[2], *histW[2];	
	int detectStart[2];

	int kernelSize; // 密度推定用のカーネルサイズ
	int *kernel;	// 密度推定用のカーネル(指数関数)

	// 検出したスティックの情報
	struct PosBuff{
		iVec2 pos;
		bool detect;
	};

	PosBuff posBuff[2][3];
	int lastPos[2];

public:
	// デバッグ用に一時 public
	Mat diffMask;	//輝度動きマップ
	Mat dstCol[2];	//　色動きマップ

	Detector( int w, int h, float f ){
		this->w = w, this->h = h;

		// 配列確保
		srcGray  = Mat(Size(w, h),CV_8UC1); 
		preGray  = Mat(Size(w, h),CV_8UC1);
		diffMask = Mat(Size(w, h),CV_8UC1);


		
		{
			for( int i = 0; i < 2; i++ ){
				detectStart[i] = true;
				dstCol[i] = Mat(Size(w, h), CV_8UC1);
				lastPos[i] = 0;
				memset( &posBuff[i], 0, 3 * sizeof(PosBuff) );
			}
		}
		{// 密度推定用
			for( int i = 0; i < 2; i++ ){
				histH[i] = new int[h];
				histW[i] = new int[w];
			}

			kernelSize = 20;
			kernel = new int[kernelSize+1];
			for(int i = 0 ; i < kernelSize + 1; i++ ){
				kernel[i] = (int)(20 * exp((double)(-i*0.2)) + 0.5);
			}
		}
	}

	~Detector(){
		for( int i = 0; i < 2; i++ ){
			delete []histH[i];
			delete []histW[i];
		}
		delete []kernel;
	}

	void init( Mat &frame ){
		srcRGB = &frame;

		// 平滑化
		GaussianBlur( *srcRGB, *srcRGB, Size(5,5), 3 );

		// RGB→GRAY
		cvtColor( *srcRGB, srcGray, CV_BGR2GRAY );
	}

	// スティックのアクション検出
	void detectAction( vector<iVec2> &act );
};

#endif