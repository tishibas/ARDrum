#include "common.h"
#include "procImg.h"

void Detector::detectAction( vector<iVec2> &act ){
	// 各種閾値
	const int diffThresh = 5;
	const int detectThresh = 2;
	const int actionThresh = 20;
	const int actionLimit = 5;

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
	cv::erode(diffMask, diffMask, Mat(), Point(-1, -1));

	int dstMaxVal[2] = {0};
	//閾値以上の動き領域で、所定成分の色を取得
	for( int v = 0; v < h; v++ ){
		histH[0][v] = 0;
		histH[1][v] = 0;
		for( int u = 0; u < w; u++ ){
			int rgbOffset  = step*v + 3*u;
			int grayOffset = w*v + u;

			unsigned char r =  pSrcRGB[rgbOffset + 2];
			unsigned char g =  pSrcRGB[rgbOffset + 1];
			unsigned char b =  pSrcRGB[rgbOffset + 0];

			int dstCol[2];
			dstCol[0] = r - MAX(g,b) - 5 * abs(g - b); // max 2*256 赤検出
			dstCol[1] = b - MAX(g,r) - abs(g - r); // max 2*256 青検出

			for( int i = 0; i < 2; i++ ){
				if( dstCol[i] < 0 ) dstCol[i] = 0;
				if( pDiffMask[grayOffset] == 0 ) dstCol[i] = 0;
				histH[i][v] += dstCol[i];
				pDstCol[i][grayOffset] = dstCol[i];
				if( dstCol[i] > dstMaxVal[i] ) dstMaxVal[i] = dstCol[i];
			}
		}
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

			for( int i = 0; i < 2; i++ ){
				for( int v = kernelSize; v < h - kernelSize; v++ ){
					int val = 0;
					for( int j = v - kernelSize; j < v + kernelSize; j++ ){
						int k = abs(j - v);
						val += (int)(kernel[k] * histH[i][j] * (255.0/dstMaxVal[i] ) + 0.5);
					}

					if( val > maxVal[i] ){
						maxPos[i] = v;
						maxVal[i] = val;
					}
					mean[i] += val;
					//printf("%d\t %d\n", int( histH[i][v] * (255.0/dstMaxVal[i] )), val);
				}
				pos[i].y = maxPos[i];

				mean[i] = mean[i] / (h - 2*kernelSize);
				if( maxVal[i] < detectThresh*mean[i] ){
					detectFlag[i] = false;

				}

			}
		}

		{// x探索
			int maxVal[2] = {0};
			int maxPos[2] = {0};
			int mean[2] = {0};

			for( int i = 0; i < 2; i++ ){
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
						val += (int)(kernel[abs(j - u)] * histW[i][j] * (255.0/dstMaxVal[i] ) + 0.5);
					}
					if( val >= maxVal[i] ){
						maxPos[i] = u;
						maxVal[i] = val;
					}
				}
				pos[i].x = maxPos[i];
			}
		}

		// fordebbug
		//if( detectFlag ){
		//	line( *srcRGB, Point(pos[0].x-20, pos[0].y) , Point(pos[0].x+20, pos[0].y), Scalar(0, 0, 255), 4, CV_AA );
		//}


		{
			for( int i = 0; i < 2; i++ ){
				posBuff[i][lastPos[i]].pos = pos[i];
				posBuff[i][lastPos[i]].detect = detectFlag[i];

				PosBuff buff1 = posBuff[i][(lastPos[i] + 3 - 2) % 3];
				PosBuff buff2 = posBuff[i][(lastPos[i] + 3 - 1) % 3];
				PosBuff buff3 = posBuff[i][(lastPos[i] + 3 - 0) % 3];

				{// 振り上げ検出
					if( buff1.pos.y - buff2.pos.y < 0 ){
						detectStart[i] = true;
					}
				}

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
				if( detectStart[i] && action ){
					detectStart[i] = false;
					act.push_back( pos[i] );
				}
				lastPos[i]++;  lastPos[i] = lastPos[i] % 3;
			}
		}
	}


	srcGray.copyTo( preGray );
}



//Mat minImg, gray, hsv, bin;
//Mat pre, preBin;
//void procImg(Mat &frame, Mat &dst, int val1, int val2){
//	static bool once = true;
//	resize( frame, minImg, Size(), 0.5, 0.5 );
//	GaussianBlur(minImg, minImg, Size(3, 3), 5);
//	cvtColor( minImg, hsv, CV_BGR2HSV );
//	cvtColor( minImg, dst, CV_BGR2GRAY );
//	cvtColor( minImg, bin, CV_BGR2GRAY );
//
//	if ( !once ){
//		int w = hsv.cols, h = hsv.rows;
//		unsigned char *phsv = hsv.data;
//		unsigned char *ppre = pre.data;
//		for( int v = 0; v < h; v++ ){
//			for( int u = 0; u < w; u++ ){
//				int iH = phsv[hsv.step*v + 3*u + 0];
//				int pH = ppre[hsv.step*v + 3*u + 0];
//
//				int iS = phsv[hsv.step*v + 3*u + 1];
//				int pS = ppre[hsv.step*v + 3*u + 1];
//
//				int iV = phsv[hsv.step*v + 3*u + 2];
//				int pV = ppre[hsv.step*v + 3*u + 2];
//
//				if( /*(pH<val1||pH>val2) && */(iH>val1 && iH<val2) && iV >= 50 && iS >= 50) //HSVでの検出
//				{
//					dst.data[dst.step*v + u] = 255; //肌色部分を青に
//				}
//				else{
//					dst.data[dst.step*v + u] = 0; //肌色部分を青に
//				}
//			}
//		}
//	}
//	once = false;
//	bin.copyTo(preBin);
//	hsv.copyTo(pre);
//	resize( dst, dst, Size(), 2, 2 );
//}