#include "common.h"
#include "procImg.h"

void Detector::detectAction( vector<Vec2> &act ){
	// 各種閾値
	const int diffThresh = 10;
	const int detectThresh = 5;
	const int actionThresh = 20;
	const int actionLimit = 5;

	// 画像配列のポインタを移行
	int step = srcRGB->step;
	unsigned char *pSrcRGB = srcRGB->data;
	unsigned char *pSrcGray = srcGray.data;
	unsigned char *pPreGray = preGray.data;
	unsigned char *pDstRed = dstRed.data;
	unsigned char *pDstBlue = dstBlue.data;
	unsigned char *pDiffMask = diffMask.data;

	//動き残差計算
	for( int v = 0; v < h; v++ ){
		for( int u = 0; u < w; u++ ){
			int grayOffset = w*v + u;
			int val = pSrcGray[grayOffset] - pPreGray[grayOffset];
			pDiffMask[grayOffset] = (unsigned char)( abs(val) );
		}
	}

	//閾値以上の動き領域で、所定成分の色を取得
	for( int v = 0; v < h; v++ ){
		histRedH[v] = 0;
		for( int u = 0; u < w; u++ ){
			int rgbOffset = step*v + 3*u;
			int grayOffset = w*v + u;

			unsigned char r =  pSrcRGB[rgbOffset + 2];
			unsigned char g =  pSrcRGB[rgbOffset + 1];
			unsigned char b =  pSrcRGB[rgbOffset + 0];

			int dstRed  = r - (g + b)/2 - abs( g - b ); // max 2*256 赤検出
			int dstBlue = b - (g + r)/2 - abs( g - r ); // max 2*256 赤検出

			if( dstRed  < 0 ) dstRed  = 0;
			if( dstBlue < 0 ) dstBlue = 0;

			if( pDiffMask[grayOffset] < diffThresh ){
				dstRed = 0;
			}
			histRedH[v] += dstRed;
			histBlueH[v] += dstBlue;

			pDstRed [grayOffset] = (unsigned char)(dstRed);
			pDstBlue[grayOffset] = (unsigned char)(dstBlue);
		}
	}

	act.clear();
	{// 検出
		//検出判定
		bool detectFlagRed = true;

		//検出位置
		Vec2 posRed;

		{// y探索
			int maxValRed =  0, maxValBlue = 0;
			int maxPosRed = -1, maxPosBlue = -1;
			int meanRed = 0, meanBlue = 0;
			for( int v = kernelSize; v < h - kernelSize; v++ ){
				int valRed = 0, valBlue = 0;
				for( int i = v - kernelSize; i < v + kernelSize; i++ ){
					valRed  += (int)(kernel[abs(v - i)] * histRedH [v] + 0.5);
					valBlue += (int)(kernel[abs(v - i)] * histBlueH[v] + 0.5);
				}
				if( valRed > maxValRed ){
					maxPosRed = v;
					maxValRed =valRed;
				}
				if( valBlue > maxValBlue ){
					maxPosBlue = v;
					maxValBlue =valBlue;
				}

				meanRed  += valRed;
				meanBlue += valBlue;
			}
			posRed.y = maxPosRed;

			meanRed = meanRed / (h - 2*kernelSize);
			if( maxValRed < detectThresh * meanRed ){
				detectFlagRed = false;
			}
		}

		{// x探索
			int maxVal =  0;
			int maxPos = -1;
			int mean = 0;
			for( int u = 0; u < w; u++ ){
				int v = posRed.y - kernelSize;
				int greyOffset = w*v + u;
				histRedW[u] = pDstRed[greyOffset];
			}
			for( int v = posRed.y - kernelSize + 1; v < posRed.y + kernelSize; v++ ){
				for( int u = 0; u < w; u++ ){
					int greyOffset = w*v + u;
					histRedW[u] += pDstRed[greyOffset];
				}
			}
			for( int u = kernelSize; u < w - kernelSize; u++ ){
				int val = 0;
				for( int i = u - kernelSize; i < u + kernelSize; i++ ){
					val += (int)(kernel[abs(u - i)] * histRedW[u] + 0.5);
				}
				if( val > maxVal ){
					maxPos = u;
					maxVal =val;
				}
			}
			posRed.x = maxPos;

		}

		// fordebbug
		//if( detectFlag ){
		//	line( *srcRGB, Point(x-20, y) , Point(x+20, y), Scalar(0, 0, 255), 4, CV_AA );
		//}

		posBuffRed[lastPosRed].pos = posRed;
		posBuffRed[lastPosRed].detect = detectFlagRed;

		bool action = true;
		{// 赤検出
			PosBuff buff1 = posBuffRed[(lastPosRed + 3 - 2) % 3];
			PosBuff buff2 = posBuffRed[(lastPosRed + 3 - 1) % 3];
			PosBuff buff3 = posBuffRed[(lastPosRed + 3 - 0) % 3];

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
		lastPosRed++;
		lastPosRed = lastPosRed % 3;
	
		if( action ){
			act.push_back( posRed );
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