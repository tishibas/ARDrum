#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "findCircleMaker.h"
#include <iostream>
#include <iterator>
#include <stdio.h>

using namespace std;
using namespace cv;

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

        vector<Marker> markers;
        findCircle.getMarker( markers );   // 円認識
        key = waitKey(1);
        switch( key ){
            case 't' :{ toggle ^= 1; break;} 
            default  :{ break;}
        }
    }
    return 0;	
}

void FindCircle::selectMarkerCand( vector<MarkerCand> &markerCands, vector<Marker> &markers, LabelingBS &labelingBs){
    for( vector<MarkerCand>::iterator it1 = markerCands.begin(); it1 != markerCands.end(); it1++){
            bool markFlug = true;
        for( vector<MarkerCand>::iterator it2 = markerCands.begin(); it2 != markerCands.end(); it2++){
            if(it1 == it2)
                continue;
            // 中心
            if( it1->minNode.x < it2->center.x && it2->center.x < it1->maxNode.x 
                 && it1->minNode.y < it2->center.y && it2->center.y < it1->maxNode.y){
                markFlug = false;
                cout << "markerCand in markerCands" << endl; 
                continue;
            }
        }
        // 中にラベルがあるかどうか
        bool labelFlug = false;
        for( int i = 0; i < labelingBs.GetNumOfRegions(); i++){
            RegionInfoBS* lb =  labelingBs.GetResultRegionInfo(i);
            // 中心
            float cx, cy;
            lb->GetCenter(cx, cy);
            int size_x, size_y;
            lb->GetSize( size_x, size_y );
            // 面積
            int size = size_x * size_y;
            if(size > (double)w*h/30.0)
                continue;
            if( !(it1->minNode.x < cx && cx < it1->maxNode.x 
                 && it1->minNode.y < cy && cy < it1->maxNode.y)){
                labelFlug = true;
                cout << "labels in markerCands" << endl; 
                continue;
            }
        }
        if(markFlug && labelFlug){
            Marker mark;
            mark.setCenter(it1->center);
            mark.calcAngle(it1->size_x, it1->size_y);
            cout << "center: " << mark.center.x << "," << mark.center.y << ","  << mark.center.z << endl;
            cout << "size x:" << mark.size_x << "size_y :" << mark.size_y << endl;  
            cout << "angle:" << mark.angle << endl;
            markers.push_back(mark);
        } 
    }
};

void FindCircle::getMarker( vector<Marker> &markers ){
    
    threshold (srcGray, srcBW, 100, 255, CV_THRESH_BINARY_INV );//マーカーが浮き出る
    imshow("bin", srcBW);
    Mat label(srcRGB->size(), CV_16SC1);
    LabelingBS labeling;
    labeling.Exec(srcBW.data, (short *)label.data, srcRGB->cols, srcRGB->rows, false, 0);

    // ラベリング結果を出力する、真っ白な状態で初期化
    vector<MarkerCand> markerCands;
    // ラベルされた領域をひとつずつ描画
    for( int i = 0; i < labeling.GetNumOfRegions(); i++)
    {
        MarkerCand cand;
        RegionInfoBS* lb =  labeling.GetResultRegionInfo(i);
        // 中心
        float cx, cy;;
        lb->GetCenter(cx, cy);
        Vec2 center;
        center.set(cx, cy);
        // サイズ
        int size_x, size_y;
        lb->GetSize( size_x, size_y );
        // 面積
        int size = size_x * size_y;
        // ピクセル数  
        int pix = lb->GetNumOfPixels();
        Vec2 minNode, maxNode;
        lb->GetMin(cand.minNode.x, cand.minNode.y);
        lb->GetMax(cand.maxNode.x, cand.maxNode.y);

        // あまりにも大きいのはマーカーではない
        if(size_x > ( (float)w/2.0))
             continue;
        if(size_y > ( (float)h/2.0 ))
            continue;
        // あまりにも小さいのはマーカーではない(x方向のみ)
        if (size_x < ( (float)w/10.0))
            continue;
        // 中が塗りつぶされていないもの
        if ( 5.0 * pix > size )
            continue;
        
        // マーカー候補
        cand.setCenter(center);
        cand.calcAngle(size_x, size_y);
        markerCands.push_back(cand);
    }
    selectMarkerCand(markerCands, markers, labeling);
    for(int i=0; i < markers.size(); i++){
        ellipse( *srcRGB, Point(markers[i].center.x, markers[i].center.y), Size(markers[i].size_x, markers[i].size_y), 0, 0, 360, Scalar(0,0,200), 3, 4);
    }
    imshow("label", *srcRGB);
};
