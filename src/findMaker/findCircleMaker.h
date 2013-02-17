#ifndef FIND_CIRCLE_H
#define FIND_CIRCLE_H

using namespace cv;

class Vec2{
    public:
        int x,y;
        void set(int _x, int _y){ x = _x; y = _y; };
        Vec2(){ x = 0; y = 0;};
        ~Vec2(){}
};

class Vec3{
    public:
        int x, y, z;
        Vec3(){ x=0; y = 0; };
        ~Vec3(){}
};

class Marker{
    public:
        Vec3 center;
        double angle;
        int size_x, size_y;
        int calcZ(Vec2 center2){
            int cz = 0;
            return cz;
        };
        void setCenter(Vec3 center3){
            center = center3;
        };
        void setCenter(Vec2 center2){
           center.x = center2.x;
           center.y = center2.y;
           center.z = calcZ(center2);
        };
        void calcAngle(int _size_x, int _size_y){
            size_x = _size_x;
            size_y = _size_y;
            if(size_x < size_y)
                size_y = size_x;
            double cos = (double)size_y/(double)size_x;
            angle = acos(cos);
        };
        Marker(){};
        ~Marker(){}
};

class MarkerCand : public Marker{
    public:
        Vec2 minNode, maxNode;
};

class FindCircle{
    int w, h;   // 画像サイズ
    float f;    // 焦点

    Mat *srcRGB;    //入力画像のポインタ
    Mat srcGray;    //入力画像のグレー
    Mat srcBW;      //入力画像の白黒

public:
    
    FindCircle( int w, int h, float f ){
        this->w = w, this->h = h;

        // 配列確保
        srcGray = Mat(Size(w, h),CV_8UC1);
    }

    ~FindCircle(){
    }


    void init( Mat &frame ){
        srcRGB = &frame;

        // RGB→GRAY
        cvtColor( *srcRGB, srcGray, CV_BGR2GRAY );

        //平滑化
        GaussianBlur( srcGray, srcGray, Size(5,5), 3 );
    }

    // マーカ検出
    void selectMarkerCand( vector<MarkerCand> &markerCands, vector<Marker> &markers);
    void getMarker( vector<Marker> &markers );
};




#endif
