#include "common.h"
#include "procImg.h"

int main(){
	
	VideoCapture cap(2);
	if(!cap.isOpened()) return -1;

	char *wName = "test";
	namedWindow( wName );

	int toggle = 1;
	int key = 0;

	Mat frame;
	cap >> frame;

	Detector detector(frame.cols, frame.rows, 1000.0);

	while( key != 'q' ){
		cap >> frame;
		detector.init( frame );			// データ入力

		vector<Vec2> act;
		detector.detectAction( act );	// スティック検出

		{// 描画
			for( int i = 0; i < act.size(); i++ ){
				int x = act[i].x, y = act[i].y;
				line( frame, Point(x-15, y) , Point(x+15, y), Scalar(255, 255, 0), 4, CV_AA );
				line( frame, Point(x, y-15) , Point(x, y+15), Scalar(255, 255, 0), 4, CV_AA );
				circle( frame, Point(x, y), 20, Scalar(0, 255, 255), 2 );
			}
		}

		Mat *show = toggle ? &frame : &detector.dstRed;
		imshow( wName, *show );

		key = waitKey(1);
		switch( key ){
			case 't' :{ toggle ^= 1; break;} 
			default  :{ break;}
		}
	}
	return 0;
}