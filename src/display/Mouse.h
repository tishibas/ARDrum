/***************************************************************************
		Mouse.h

		Mouse Function Library

		Version : 1.0
		Date : Nov. 02, 2007
		Author : Pocol
****************************************************************************/

#ifndef _MOUSE_H_INCLUDED_
#define _MOUSE_H_INCLUDED_

//
//　include
//
#include <iostream>
using namespace std;


//
//　global
//
const double PI = 3.14159265358979323846264338327;
template<class T> static inline T RadToDeg(T rad) { return ( (rad)*(180.0/PI) ); }
template<class T> static inline T DegToRad(T deg) { return ( (deg)*(PI/180.0) ); }
typedef enum MState	{ Push, Release, None };

//////////////////////////////////////////////////////////////////////////
//　　MPosition class
//////////////////////////////////////////////////////////////////////////
class MPosition
{
public:
	double x;
	double y;

	MPosition(double nx=0.0, double ny=0.0) { x=nx; y=ny; }
	~MPosition() {}
	void Reset() { x = 0.0, y=0.0; }
	void ConsoleOut() { cout << "(x, y) = (" << x << ", " << y << ")\n"; } 
};


//////////////////////////////////////////////////////////////////////////
//　　MouseButton class
//////////////////////////////////////////////////////////////////////////
class MouseButton
{
public:
	MPosition before;
	MPosition current;
	MPosition after;
	MState state;

	MouseButton();
	~MouseButton();
	void Reset();
	void ConsoleOut();
};

//////////////////////////////////////////////////////////////////////////
//　　ViewCamara class
//////////////////////////////////////////////////////////////////////////
class ViewCamera
{
public:
	MouseButton right;
	MouseButton left;
	MouseButton middle;

	double distance;
	double angle[3];
	double position[3];
	double target[3];
	double upvector[3];
	double translate[3];

	ViewCamera(double dist=5.0);
	~ViewCamera();
	void Reset();
	void MouseInput(int button, int state, int x, int y);
	void MouseMotion(int x, int y);
	void Set();
	void RenderSubAxis(int w, int h);
	void SetParams();  //追記
};



#endif		//　_MOUSE_WIN_H_INCLUDED_
