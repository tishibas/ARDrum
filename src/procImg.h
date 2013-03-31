#ifndef PROCIMG_H
#define PROCIMG_H
#include "Labeling.h"

using namespace cv;

struct iVec2{
	int x, y;
};
struct fVec3{
	float x, y, z;
};


struct Vec2{
	int x,y;
	void set(int _x, int _y){ x = _x; y = _y; };
	Vec2(){ x = 0; y = 0;};
	~Vec2(){}
};

struct Vec3{
	float x, y, z;
	Vec3(){ x=0; y = 0; };
	~Vec3(){}
};

struct Marker{
	iVec2 center;
	Vec3 pos;
	double angle;
	int size_x, size_y;
	void calcZ(float f, int w, int h){
		pos.z = 100.0 * f / size_x; 
		pos.x = (center.x - w/2) * pos.z / f;
		pos.y = (h/2 - center.y) * pos.z / f;
	};
	void setCenter(iVec2 center3){
		center = center3;
	};
	void setCenter(Vec2 center2){
		center.x = center2.x;
		center.y = center2.y;
	};
	void calcAngle(int _size_x, int _size_y){
		size_x = _size_x;
		size_y = _size_y;
		if(size_x < size_y)
			size_y = size_x;
		double cos = (double)size_y/(double)size_x;
		angle = acos(cos);
	};
};
struct MarkerCand : public Marker{
	Vec2 minNode, maxNode;
};


class Detector{
	int w, h;	// 画像サイズ
	float f;	// 焦点

	Mat *srcRGB;	//入力画像のポインタ
	Mat srcGray;	//入力画像のグレー
	Mat preGray;	//一個前のグレー
    Mat srcBW;      //入力画像の白黒

	//動きマップの座標軸ヒストグラム 2色
	int *histH[2], *histW[2];	
	int detectStart[2];

	int kernelSize; // 密度推定用のカーネルサイズ
	int *kernel;	// 密度推定用のカーネル(指数関数)
	int kernelSum;

	// 検出したスティックの情報
	struct PosBuff{
		iVec2 pos;
		bool detect;
	};

	PosBuff posBuff[2][3];
	int lastPos[2];
	int valMax[2];


public:
	// デバッグ用に一時 public
	Mat diffMask;	//輝度動きマップ
	Mat dstCol[2];	//　色動きマップ
	vector<Marker> m_marker;
	vector<MarkerCand> markerCands;

	Mat debug1, debug2;

	Detector( int w, int h, float f );

	~Detector();

	void init( Mat &frame );

	// スティックのアクション検出
	void detectAction( vector<iVec2> &act );

	    // マーカ検出
	void getMarkerPos(/*vector<Vec3> &posList*/);
    void selectMarkerCand( vector<MarkerCand> &markerCands, vector<Marker> &markers, LabelingBS &labelingBs);
    void getMarker( vector<Marker> &markers );
};

#endif



