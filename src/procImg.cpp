#include "common.h"
#include "procImg.h"

Detector::~Detector(){
	for( int i = 0; i < 2; i++ ){
		delete []histH[i];
		delete []histW[i];
	}
	delete []kernel;
}

void Detector::init( Mat &frame ){
	srcRGB = &frame;

	// 平滑化
	GaussianBlur( *srcRGB, *srcRGB, Size(5,5), 3 );

	// RGB→GRAY
	cvtColor( *srcRGB, srcGray, CV_BGR2GRAY );
}

Detector::Detector( int w, int h, float f ){
	this->w = w, this->h = h, this->f = f;

	// 配列確保
	srcGray  = Mat(Size(w, h),CV_8UC1); 
	preGray  = Mat(Size(w, h),CV_8UC1);
	diffMask = Mat(Size(w, h),CV_8UC1);

	debug1 = Mat(Size(256, h),CV_8UC1);
	debug2 = Mat(Size(w, 256),CV_8UC1);


	{
		for( int i = 0; i < 2; i++ ){
			detectStart[i] = true;
			dstCol[i] = Mat(Size(w, h), CV_8UC1);
			lastPos[i] = 0;
			memset( &posBuff[i], 0, 3 * sizeof(PosBuff) );

			valMax[i] = 0;
		}
	}
	{// 密度推定用
		for( int i = 0; i < 2; i++ ){
			histH[i] = new int[h];
			histW[i] = new int[w];
		}

		kernelSize = 10;
		kernelSum = -20;
		kernel = new int[kernelSize+1];
		for(int i = 0 ; i < kernelSize + 1; i++ ){
			kernel[i] = (int)(20 * exp((double)(-i*0.2)) + 0.5);
			kernelSum += 2*kernel[i];
		}
	}
}



void Detector::detectAction( vector<iVec2> &act ){
	// 各種閾値
	const int diffThresh = 5;
	const double detectThresh = 1.2;
	const int actionThresh = 15;
	const int actionLimit = 8;
	const int countMax = 5;

	const int Margin = 50;

	const int colNum = 2;

	// 画像配列のポインタを移行
	int step = srcRGB->step;
	unsigned char *pSrcRGB   = srcRGB->data;
	unsigned char *pSrcGray  = srcGray.data;
	unsigned char *pPreGray  = preGray.data;
	unsigned char *pDiffMask = diffMask.data;

	unsigned char *pDstCol[2];
	pDstCol[0] = dstCol[0].data;
	pDstCol[1] = dstCol[1].data;

	//動き残差計算
	for( int v = 0; v < h; v++ ){
		for( int u = 0; u < w; u++ ){
			int grayOffset = w*v + u;
			int val = pSrcGray[grayOffset] - pPreGray[grayOffset];
			pDiffMask[grayOffset] = (unsigned char)( abs(val) );
		}
	}
	cv::threshold( diffMask, diffMask, diffThresh, 255, CV_THRESH_BINARY );
	cv::erode ( diffMask, diffMask, Mat(), Point(-1, -1), 2);
	cv::dilate( diffMask, diffMask, Mat(), Point(-1, -1));

	int dstMaxVal[2] = {0};
	//閾値以上の動き領域で、所定成分の色を取得
	for( int v = 0; v < h; v++ ){
		histH[0][v] = 0;
		histH[1][v] = 0;

		if( v < Margin || v > h - Margin ) continue;
		for( int u = 0; u < w; u++ ){
			int rgbOffset  = step*v + 3*u;
			int grayOffset = w*v + u;

			unsigned char r = pSrcRGB[rgbOffset + 2];
			unsigned char g = pSrcRGB[rgbOffset + 1];
			unsigned char b = pSrcRGB[rgbOffset + 0];

			int dstCol[2];
			//dstCol[0] = r - MAX(g,b) - 5 * abs(g - b); // max 2*256 赤検出
			dstCol[0] = (r + g)/2 - b - 0.5 * abs(r - g);  // max 2*256 赤検出
			dstCol[1] = b - MAX(g,r) - 0.5 * abs(g - r);// max 2*256 青検出

			for( int i = 0; i < colNum; i++ ){
				if( dstCol[i] < 0 ) dstCol[i] = 0;
				if( pDiffMask[grayOffset] == 0 ) dstCol[i] = 0;
				if( dstCol[i] > dstMaxVal[i] ) dstMaxVal[i] = dstCol[i];

				if( dstCol[i] > histH[i][v] ){
					histH[i][v] = dstCol[i];
				}
				pDstCol[i][grayOffset] = dstCol[i];

			}
		}
	}

	for(int i = 0; i < colNum; i++ ){
		if( valMax[i] > dstMaxVal[i] )	valMax[i] = dstMaxVal[i];
	}
	//equalizeHist( dstRed, dstRed );

	act.clear();
	{// 検出
		//検出判定
		bool detectFlag[2];
		detectFlag[0] = true;
		detectFlag[1] = true;

		//検出位置
		iVec2 pos[2];

		{// y探索
			int maxVal[2] = {0};
			int maxPos[2] = {0};
			int mean[2] = {0};
			int max = 0;
			int meanCount = 1;
			for( int i = 0; i < colNum; i++ ){
				for( int v = kernelSize; v < h - kernelSize; v++ ){
					int val = 0;
					for( int j = v - kernelSize; j < v + kernelSize; j++ ){
						int k = abs(j - v);
						val += (int)(kernel[k] * histH[i][j]  + 0.5);
					}

					val /= kernelSum;
					if( val < 10){
						val = 0;
					}

					if( val > max ){
						max = val;
					}
					//for( int j = 0; j < 256; j++ ){
					//	if( j < val ) debug1.data[256*v + j] = 255;
					//	else debug1.data[256*v + j] = 0;
					//}
					if( val > maxVal[i] ){
						maxPos[i] = v;
						maxVal[i] = val;
					}
					mean[i] += val;

					if( histH[i][v] > 0 ) meanCount++;
				}

				pos[i].y = maxPos[i];

				mean[i] = mean[i] / meanCount;
				if( maxVal[i] < detectThresh*mean[i] ){
					detectFlag[i] = false;

				}
				if( max > valMax[i] ){
					valMax[i] = max;
				}
			}

			//int mn = 0;
			//for( int i = 0; i < 1; i++ ){
			//	for( int v = 0; v < h; v++ ){
			//		int val = histH[i][v];
			//		for( int j = 0; j < 256; j++ ){
			//			if( j < val ) debug1.data[256*v + j] = 255;
			//			else debug1.data[256*v + j] = 0;
			//		}
			//		mn+=histH[i][v];
			//	}
			//}
			//mn /= h;
			//line( debug1, Point(detectThresh*mean[0], 0), Point(detectThresh*mean[0], h), Scalar(255,255,255));
			//imshow( "debug1", debug1);
		}

		{// x探索
			int maxVal[2] = {0};
			int maxPos[2] = {0};
			int mean[2] = {0};

			for( int i = 0; i < colNum; i++ ){
				memset(histW[i], 0, sizeof(int) * w );
				for( int v = pos[i].y - kernelSize; v < pos[i].y + kernelSize; v++ ){
					for( int u = 0; u < w; u++ ){
						int greyOffset = w*v + u;
						histW[i][u] += pDstCol[i][greyOffset];;
					}
				}
				for( int u = kernelSize; u < w - kernelSize; u++ ){
					int val = 0;
					for( int j = u - kernelSize; j < u + kernelSize; j++ ){
						val += (int)(kernel[abs(j - u)] * histW[i][j]  + 0.5);
					}
					if( val >= maxVal[i] ){
						maxPos[i] = u;
						maxVal[i] = val;
					}
				}
				pos[i].x = maxPos[i];
			}
			//imshow( "debug2", debug2);
		}

		// fordebbug
		//if( detectFlag ){
		//	line( *srcRGB, Point(pos[0].x-20, pos[0].y) , Point(pos[0].x+20, pos[0].y), Scalar(0, 0, 255), 4, CV_AA );
		//}


		{
			for( int i = 0; i < colNum; i++ ){
				posBuff[i][lastPos[i]].pos = pos[i];
				posBuff[i][lastPos[i]].detect = detectFlag[i];

				PosBuff buff1 = posBuff[i][(lastPos[i] + 3 - 2) % 3];
				PosBuff buff2 = posBuff[i][(lastPos[i] + 3 - 1) % 3];
				PosBuff buff3 = posBuff[i][(lastPos[i] + 3 - 0) % 3];

				bool action = true;
				{// 降り下げ検出
					//
					if( abs(buff1.pos.x - buff2.pos.x) > actionLimit * actionThresh || abs(buff1.pos.y - buff2.pos.y) > actionLimit * actionThresh ){
						action = false;
						buff1.detect = false;
					}
					if( abs(buff2.pos.x - buff3.pos.x) > actionLimit * actionThresh || abs(buff2.pos.y - buff3.pos.y) > actionLimit * actionThresh ){
						action = false;
					}

					if( !buff1.detect || !buff2.detect || !buff3.detect ){
						action = false;
					}
					if( buff2.pos.y - buff1.pos.y < actionThresh || buff3.pos.y - buff2.pos.y > buff2.pos.y - buff1.pos.y ){
						action = false;
					}
				}
				detectStart[i]++;
				if( detectStart[i] >= 0 && action ){
					detectStart[i] = - countMax;
					act.push_back( pos[i] );
				}
				lastPos[i]++;  lastPos[i] = lastPos[i] % 3;
			}
		}
	}

	srcGray.copyTo( preGray );
}

void conv( vector<Vec3> &src, vector<Vec3> &dst ){

	if( src.size() == 4 ){
		int id[4] = {0};
		for( int i = 0; i < 4; i++ ){

		}

	}
	else{
		dst = src;
	}
}

void Detector::getMarkerPos(/*vector<Vec3> &posList*/){

	m_marker.clear();
	getMarker( m_marker );

	for( int i = 0; i < m_marker.size(); i++){
		m_marker[i].calcZ(f, w, h);
	}

	int drumPos[4] = {0};
	if( m_marker.size() == 4 ){
		vector<Marker> tmpMrk;

		for( int i = 0; i < 4; i++ ){
			int min = w;
			int pos;
			for( int j = 0; j < 4; j++ ){
				if( !drumPos[j] && m_marker[j].center.x < min){
					min = m_marker[i].center.x;
					pos = j;
				}
			}
			drumPos[pos] = 1;
			tmpMrk.push_back(m_marker[pos]);
		}

		m_marker.clear();
		m_marker.push_back(tmpMrk[0]);
		for( int i = 0; i < 4; i++ ){
			m_marker.push_back(tmpMrk[i]);
		}
		m_marker.push_back(tmpMrk[3]);

		//m_marker[0].pos.x -= fabs(tmpMrk[0].pos.x - tmpMrk[1].pos.x);
		//m_marker[5].pos.x += fabs(tmpMrk[2].pos.x - tmpMrk[3].pos.x);

		//m_marker[0].center.x -= abs(tmpMrk[0].center.x - tmpMrk[1].center.x);
		//m_marker[5].center.x += abs(tmpMrk[2].center.x - tmpMrk[3].center.x);



		m_marker[0].pos.x -= fabs(tmpMrk[0].pos.x - tmpMrk[1].pos.x);
		m_marker[5].pos.x += fabs(tmpMrk[2].pos.x - tmpMrk[3].pos.x);

		//m_marker[0].pos.y += 100;fabs(tmpMrk[0].pos.x - tmpMrk[1].pos.x);
		//m_marker[5].pos.y += 100;fabs(tmpMrk[2].pos.x - tmpMrk[3].pos.x);

		m_marker[0].center.x -=  abs(tmpMrk[0].center.x - tmpMrk[1].center.x);
		m_marker[5].center.x +=  abs(tmpMrk[2].center.x - tmpMrk[3].center.x);

		//m_marker[0].center.y -=100;
		//m_marker[5].center.y -=100;
		//for( int i = 0; i < 6; i++ ){
		//	printf( "%d %d\n",  m_marker[i].center.x, m_marker[i].center.y);
		//	printf( "%4.2lf %4.2lf %4.2lf %4.2lf\n", m_marker[i].pos.x, m_marker[i].pos.y, m_marker[i].pos.z,  m_marker[i].angle );

		//}
		//getch();
	}

}



void Detector::selectMarkerCand( vector<MarkerCand> &markerCands, vector<Marker> &markers, LabelingBS &labelingBs){
	
	for( vector<MarkerCand>::iterator it1 = markerCands.begin(); it1 != markerCands.end(); it1++){

		bool markFlag = true;

		for( vector<MarkerCand>::iterator it2 = markerCands.begin(); it2 != markerCands.end(); it2++){
			if(it1 == it2) continue;

			// 中心
			if( it1->minNode.x < it2->center.x && it2->center.x < it1->maxNode.x && it1->minNode.y < it2->center.y && it2->center.y < it1->maxNode.y){
					markFlag = false;
					//cout << "markerCand in markerCands" << endl; 
					continue;
			}
		}

		// 中にラベルがあるかどうか
		bool labelFlug = true;
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

			if( !(it1->minNode.x < cx && cx < it1->maxNode.x && it1->minNode.y < cy && cy < it1->maxNode.y)){
				labelFlug = true;
				//cout << "labels in markerCands" << endl; 
				continue;
			}
		}

		if( markFlag && labelFlug ){
			Marker mark;
			mark.setCenter(it1->center);
			mark.calcAngle(it1->size_x, it1->size_y);
			//cout << "center: " << mark.center.x << "," << mark.center.y << ","  << mark.center.z << endl;
			//cout << "size x:" << mark.size_x << "size_y :" << mark.size_y << endl;  
			//cout << "angle:" << mark.angle << endl;
			markers.push_back(mark);
		} 
	}

};

void Detector::getMarker( vector<Marker> &markers ){

	threshold (srcGray, srcBW, 125, 255, CV_THRESH_BINARY_INV );//マーカーが浮き出る
	//imshow( "bin", srcBW );

	Mat label(srcRGB->size(), CV_16SC1);
	LabelingBS labeling;
	labeling.Exec(srcBW.data, (short *)label.data, srcRGB->cols, srcRGB->rows, false, 0);

	// ラベリング結果を出力する、真っ白な状態で初期化
	markerCands.clear();


	// ラベルされた領域をひとつずつ描画
	for( int i = 0; i < labeling.GetNumOfRegions(); i++)
	{
		RegionInfoBS* lb =  labeling.GetResultRegionInfo(i);
		MarkerCand cand;

		// 中心
		float cx, cy;
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
		if(size_x > ( (float)w/2.0)) continue;
		if(size_y > ( (float)h/1.5)) continue;

		// あまりにも小さいのはマーカーではない(x方向のみ)
		if (size_x < ( (float)w/10.0)) continue;
		//if (size_y < ( (float)h/10.0)) continue;

		// 比率
		if ((double)size_y/size_x > 1.2) continue;

		// 中が塗りつぶされていないもの
		if ( 3.0 * pix > size ) continue;

		// マーカー候補
		cand.setCenter(center);
		cand.calcAngle(size_x, size_y);
		markerCands.push_back(cand);
	}

	//markers = markerCands;
	selectMarkerCand(markerCands, markers, labeling);

};
