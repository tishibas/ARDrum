#ifndef FIND_CIRCLE_H
#define FIND_CIRCLE_H

using namespace cv;

struct Vec2{
    int x, y;
};

class FindCircle{
    int w, h;   // 画像サイズ
    float f;    // 焦点

    Mat *srcRGB;    //入力画像のポインタ
    Mat srcGray;    //入力画像のグレー
    Mat srcBW;      //入力画像の白黒
    Mat preGray;    //一個前のグレー

    //動きマップの座標軸ヒストグラム 2色
    int *histRedH, *histRedW;   
    int *histBlueH, *histBlueW;

    int kernelSize; // 密度推定用のカーネルサイズ
    int *kernel;    // 密度推定用のカーネル(指数関数)

    // 検出したスティックの情報
    struct PosBuff{
        Vec2 pos;
        bool detect;
    };

    PosBuff posBuffRed[3], posBuffBlue[3];
    int lastPosRed, lastPosBlue;

public:
    // デバッグ用に一時 public
    Mat diffMask;   //動きマップ
    Mat dstRed;     //赤動きマップ
    Mat dstBlue;    //青動きマップ


    FindCircle( int w, int h, float f ){
        this->w = w, this->h = h;

        // 配列確保
        srcGray = Mat(Size(w, h),CV_8UC1);
        preGray = Mat(Size(w, h),CV_8UC1);
        diffMask = Mat(Size(w, h),CV_8UC1);

        dstRed = Mat(Size(w, h),CV_8UC1);
        dstBlue = Mat(Size(w, h),CV_8UC1);

        {// 密度推定用
            histRedH  = new int[h];
            histRedW  = new int[w];
            histBlueH = new int[h];
            histBlueW = new int[w];

            kernelSize = 10;
            kernel = new int[kernelSize];
            for(int i = 0 ; i < 10; i++ ){
                kernel[i] = (int)(20 * exp((double)(-i)) + 0.5);
            }
        }

        lastPosRed = 0;
        memset( &posBuffRed, 0, 3 * sizeof(PosBuff) );
    }

    ~FindCircle(){
        delete []histRedH;
        delete []histRedW;
        delete []kernel;
    }

    void init( Mat &frame ){
        srcRGB = &frame;

        // RGB→GRAY
        cvtColor( *srcRGB, srcGray, CV_BGR2GRAY );

        //平滑化
        GaussianBlur( srcGray, srcGray, Size(5,5), 3 );
    }

    // スティックのアクション検出
    void Marker( vector<Vec2> &act );
};




#endif
