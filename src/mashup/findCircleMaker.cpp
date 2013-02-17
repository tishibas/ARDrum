#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "findCircleMaker.h"
#include <iostream>
#include <iterator>
#include <stdio.h>
#include "./Labeling.h"

using namespace std;
using namespace cv;
cv::RNG rnd(1192);
cv::Scalar randomColor()
{
    return cv::Scalar(rnd.next() & 0xFF, rnd.next() & 0xFF, rnd.next() & 0xFF);
}

int main( int argc, const char** argv ){
    VideoCapture cap(0);
    if(!cap.isOpened()) return -1;

    char *wName = "test";
    namedWindow( wName );

    int toggle = 1;
    int key = 0;

    Mat frame;
    cap >> frame;

    FindCircle findCircle(frame.cols, frame.rows, 1000.0);

    while( key != 'q' ){
        cap >> frame;
        findCircle.init( frame );         // データ入力

        vector<Vec2> act;
        findCircle.Marker( act );   // 円認識

        {// 描画
            for( int i = 0; i < act.size(); i++ ){
                int x = act[i].x, y = act[i].y;
                line( frame, Point(x-15, y) , Point(x+15, y), Scalar(255, 255, 0), 4, CV_AA );
                line( frame, Point(x, y-15) , Point(x, y+15), Scalar(255, 255, 0), 4, CV_AA );
                circle( frame, Point(x, y), 20, Scalar(0, 255, 255), 2 );
            }
        }

        Mat *show = toggle ? &frame : &findCircle.dstRed;
        imshow( wName, *show );

        key = waitKey(1);
        switch( key ){
            case 't' :{ toggle ^= 1; break;} 
            default  :{ break;}
        }
    }
    return 0;	
}

void FindCircle::Marker( vector<Vec2> &act ){
    
    //輪郭保存用のストレージを確保
	CvMemStorage *storage = cvCreateMemStorage (0);//輪郭用
	CvMemStorage *storagepoly = cvCreateMemStorage (0);//輪郭近似ポリゴン用
	
    threshold (srcGray, srcBW, 125, 255, CV_THRESH_BINARY_INV );//マーカーが浮き出る
 //   imshow("bin", srcBW);
    Mat label(srcRGB->size(), CV_16SC1);
    LabelingBS labeling;
    labeling.Exec(srcBW.data, (short *)label.data, srcRGB->cols, srcRGB->rows, false, 0);

    // ラベリング結果を出力する、真っ白な状態で初期化
    Mat outimg(srcRGB->size(), CV_8UC3, Scalar(255, 255, 255));
    // ラベルされた領域をひとつずつ描画
    for( int i = 0; i < labeling.GetNumOfRegions(); i++)
    {
        RegionInfoBS* lb =  labeling.GetResultRegionInfo(i);
        int size_x, size_y;
        lb->GetSize( size_x, size_y );
        cout << "size: " << size_x << "," << size_y << endl;
/*       
領域中央の座標(!=重心) を返す
void GetCenter( float& x, float& y )
領域のサイズを返す
void GetSize( int& x, int& y )
領域の左上座標を返す
void GetMin( int& x, int& y )
領域の右下座標を返す
void GetMax( int& x, int& y )
領域の重心を返す
void GetCenterOfGravity( float& x, float& y )
領域の入力バッファにおける値を返す
SrcT GetSourceValue( void )
領域の連続領域番号を返す
DstT GetResult( void )
*/
        // ラベリング結果でイロイロする。
        // ラベリング結果の領域を抽出する。
        if(size_x < 100){
            Mat labelarea;
            compare(label, i + 1, labelarea, CV_CMP_EQ);
            // 抽出した領域にランダムな色を設定して出力画像に追加。
            Mat color(srcRGB->size(), CV_8UC3, randomColor());
            color.copyTo(outimg, labelarea);
        }
    }
    imshow("label", outimg);
};
