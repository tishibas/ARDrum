/*************************************************************************
　　Mouse.cpp
　　
　　Mouse Function Library

　　Version : 1.0
　　Date : Nov. 02, 2007
　　Author : Pocol
*************************************************************************/

//
//　includes
//
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "Mouse.h"
using namespace std;


//
//　global
//
char axisLabel[3] = { 'x', 'y', 'z' };
GLfloat axisColor_Red[4] = {1.0, 0.0, 0.0, 1.0};
GLfloat axisColor_Blue[4] = {0.0, 0.0, 1.0, 1.0};
GLfloat axisColor_Green[4] = {0.0, 1.0, 0.0, 1.0};
GLfloat axisColor_Cyan[4] = {0.0, 1.0, 1.0, 1.0};
GLfloat axisColor_Black[4] = {0.0, 0.0, 0.0, 1.0};


///////////////////////////////////////////////////////////////////////////
//　　MouseButton class
///////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------------
//　　MouseButton
//　　Desc : コンストラクタ
//------------------------------------------------------------------------------------------------------
MouseButton::MouseButton()
{
	Reset();
}


//------------------------------------------------------------------------------------------------------
//　　~MouseButton
//　　Desc : デストラクタ
//------------------------------------------------------------------------------------------------------
MouseButton::~MouseButton()
{
}


//------------------------------------------------------------------------------------------------------
//　　Reset
//　　Desc : リセットする
//------------------------------------------------------------------------------------------------------
void MouseButton::Reset()
{
	before.x = 0.0;
	before.y = 0.0;
	current.x = 0.0;
	current.y = 0.0;
	after.x = 0.0;
	after.y = 0.0;
	state = None;
}

//------------------------------------------------------------------------------------------------------
//　　ConsoleOut
//　　Desc : コンソール表示
//-------------------------------------------------------------------------------------------------------
void MouseButton::ConsoleOut()
{
	cout << "before"; 
	before.ConsoleOut();
	cout << "current";
	current.ConsoleOut();
	cout << "after";
	after.ConsoleOut();
}

///////////////////////////////////////////////////////////////////////////
//　　ViewCamera
///////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------------
//　　ViewCamera
//　　Desc : コンストラクタ
//-------------------------------------------------------------------------------------------------------
ViewCamera::ViewCamera(double dist)
{
	distance = dist;
	Reset();
}

//-------------------------------------------------------------------------------------------------------
//　　~ViewCamera
//　　Desc : デストラクタ
//-------------------------------------------------------------------------------------------------------
ViewCamera::~ViewCamera()
{
}

//-------------------------------------------------------------------------------------------------------
//　　MouseMotion
//　　Desc：マウスの移動処理
//-------------------------------------------------------------------------------------------------------
void ViewCamera::MouseMotion( int x, int y ) 
{
	//　左ボタンの処理
	if ( left.state == Push )
	{
		//　移動量を計算
		left.current.x = (double)x - left.before.x + left.after.x;
		left.current.y = (double)y - left.before.y + left.after.y;

		if ( left.current.y >= 360.0 ) left.current.y -= 360.0;
		else if ( left.current.y < 0.0 ) left.current.y += 360.0;
		
		angle[0] = DegToRad(angle[0] + left.current.x);
		angle[1] = DegToRad(angle[1] + left.current.y);
	}

	//　右ボタンの処理
	if ( right.state == Push )
	{
		right.current.x = (double)x - right.before.x + right.after.x;
		right.current.y  = -(double)y -right.before.y + right.after.y;
	}

	//　中ボタンの処理
	if ( middle.state == Push )
	{
		//　移動量を計算
		middle.current.x = (double)x -middle.before.x + middle.after.x;
		middle.current.y = (double)y -middle.before.y + middle.after.y;
		translate[0] = middle.current.x * 0.005;
		translate[1] = -middle.current.y * 0.005;
	}
}


//-------------------------------------------------------------------------------------------------------
//　　MouseInput
//　　Desc：マウスのボタン処理
//-------------------------------------------------------------------------------------------------------
void ViewCamera::MouseInput( int button, int state, int x, int y ) 
{
	switch( button )
	{
	//　左ボタン
	case GLUT_LEFT_BUTTON :
		if( state == GLUT_DOWN )
		{
			left.before.x = (double)x;
			left.before.y = (double)y;
			left.state = Push;
		}
		else if( state == GLUT_UP )
		{	
			left.after.x = left.current.x;
			left.after.y = left.current.y;
			left.state = Release;
		}
		break;

	//　右ボタン
	case GLUT_RIGHT_BUTTON :
		if( state == GLUT_DOWN ) 
		{ 		
			right.before.x = (double)x;
			right.before.y = -(double)y;
			right.state = Push;
		}
		else if( state == GLUT_UP )
		{			
			right.after.x = right.current.x;
			right.after.y = right.current.y;
			right.state = Release;
		}
		break;

	//　中ボタン
	case GLUT_MIDDLE_BUTTON :
		if ( state == GLUT_DOWN )
		{
			middle.before.x = (double)x;
			middle.before.y = (double)y;
			middle.state = Push;
		}
		else if ( state == GLUT_UP )
		{
			middle.after.x = middle.current.x;
			middle.after.y = middle.current.y;
			middle.state = Release;
		}
		break;
	}

}


//------------------------------------------------------------------------------------------------------
//　　Reset
//　　Desc：パラメータをリセットする
//------------------------------------------------------------------------------------------------------
void ViewCamera::Reset()
{
	left.Reset();
	right.Reset();
	middle.Reset();

	angle[0] = DegToRad(45.0);
	angle[1] = DegToRad(45.0);
	angle[2] = 0.0;
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;
	target[0] = 0.0;
	target[1] = 0.0;
	target[2] = 0.0;
	upvector[0] = 0.0;
	upvector[1] = 1.0;
	upvector[2] = 0.0;
	translate[0] = 0.0;
	translate[1] = 0.0;
	translate[2] = 0.0;
}
		

//------------------------------------------------------------------------------------------------------
//　　drawDisk
//　　Desc:円盤を描画する
//------------------------------------------------------------------------------------------------------
void drawDisk()
{
	GLUquadricObj *qobj;
	qobj = gluNewQuadric();
	glPushMatrix();
		glRotated( 180, 1.0, 0.0, 0.0 );
		gluDisk( qobj, 0.0, 0.35, 10, 10 );
	glPopMatrix();
}


//------------------------------------------------------------------------------------------------------
//　　Set
//　　Desc：視点の設定
//------------------------------------------------------------------------------------------------------
void ViewCamera::Set()
{
	double zoom = distance;
	zoom += right.current.y/30.0;

	//　視点位置を決定
	position[0] = sin(angle[0]) * cos(angle[1]) * zoom;
	position[1] = sin(angle[1]) * zoom;
	position[2] = cos(angle[0]) * cos(angle[1]) * zoom;

	//　アップベクトルの設定
	if( angle[1] > DegToRad(90.0)  &&  angle[1] < DegToRad(270.0) ) upvector[1] = -1.0;
	else upvector[1] = 1.0;

	//　平行移動
	glTranslated( translate[0], translate[1], translate[2] );

	//　視点位置の設定
	gluLookAt(
		position[0], position[1], position[2],
		target[0], target[1], target[2],
		upvector[0], upvector[1], upvector[2] );

}

//------------------------------------------------------------------------------------------------------
//　　RenderSubAxis
//　　Desc：補助軸の描画
//------------------------------------------------------------------------------------------------------
void ViewCamera::RenderSubAxis(int w, int h)
{
	const double zoom = 15.0;
	double eye[3] = {0.0, 0.0, 0.0};
	
	//　ウィンドウ全体をビューポートにする
	glViewport( w-100, h-100, 100, 100);
	
	//　透視変換行列の設定
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1, 1, 100000.0);

	//　モデルビュー変換の設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	eye[0] = sin(angle[0])*cos(angle[1])*zoom;
	eye[1] = sin(angle[1])*zoom;
	eye[2] = cos(angle[0])*cos(angle[1])*zoom;

	//　視点位置の設定
	gluLookAt( 
		eye[0], eye[1], eye[2],
		0.0, 0.0, 0.0,
		upvector[0],upvector[1], upvector[2] );

	//　軸の文字
	glPushMatrix();
	//glDisable(GL_LIGHTING);
	glColor4fv(axisColor_Black ); 
	glRasterPos3d(2.25, 0.0, 0.0);		
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, (int)axisLabel[0]);
	glRasterPos3d(0.0, 2.25, 0.0);		
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, (int)axisLabel[1]);
	glRasterPos3d(0.0, 0.0, 2.25);		
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, (int)axisLabel[2]);
	//glEnable(GL_LIGHTING);
	glPopMatrix();
	

	//　x軸正
	glPushMatrix();
	glColor4fv(axisColor_Red);
	glMaterialfv(GL_FRONT, GL_AMBIENT, axisColor_Red);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, axisColor_Red);
	glMaterialfv(GL_FRONT, GL_SPECULAR, axisColor_Red);
	glTranslated(1.75, 0.0, 0.0);
	glRotated(-90.0, 0.0, 1.0, 0.0);
	glutSolidCone(0.35, 1.0, 10, 10);
	drawDisk();
	glPopMatrix();	

	//　y軸正
	glPushMatrix();	
	glColor4fv(axisColor_Green);
	glMaterialfv(GL_FRONT, GL_AMBIENT, axisColor_Green);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, axisColor_Green);
	glMaterialfv(GL_FRONT, GL_SPECULAR, axisColor_Green);
	glTranslated(0.0, 1.75, 0.0);
	glRotated(90.0, 1.0, 0.0, 0.0);
	glutSolidCone(0.35, 1.0, 10, 10);
	drawDisk();
	glPopMatrix();

	//　z軸正
	glPushMatrix();
	glColor4fv(axisColor_Blue);
	glMaterialfv(GL_FRONT, GL_AMBIENT, axisColor_Blue);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, axisColor_Blue);
	glMaterialfv(GL_FRONT, GL_SPECULAR, axisColor_Blue);
	glTranslated(0.0, 0.0, 1.75);
	glRotated(180.0, 1.0, 0.0, 0.0);
	glutSolidCone(0.35, 1.0, 10, 10);
	drawDisk();
	glPopMatrix();

	//　キューブ
	glPushMatrix();
	glColor4fv(axisColor_Cyan);
	glMaterialfv(GL_FRONT, GL_AMBIENT, axisColor_Cyan);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, axisColor_Cyan);
	glMaterialfv(GL_FRONT, GL_SPECULAR, axisColor_Cyan);
	glutSolidCube(1.0);
	glPopMatrix();

	//　x軸負
	glPushMatrix();
	glTranslated(-1.75, 0.0, 0.0);
	glRotated(90.0, 0.0, 1.0, 0.0);
	glutSolidCone(0.35, 1.0, 10, 10);
	drawDisk();
	glPopMatrix();

	//　y軸負
	glPushMatrix();
	glTranslated(0.0, -1.75, 0.0);
	glRotated(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(0.35, 1.0, 10, 10);
	drawDisk();
	glPopMatrix();

	//　z軸負
	glPushMatrix();
	glTranslated(0.0, 0.0, -1.75);
	glutSolidCone(0.35, 1.0, 10, 10);
	drawDisk();
	glPopMatrix();	

}

//------------------------------------------------------------------------------------------------------
//　　Reset
//　　Desc：パラメータをセットする
//------------------------------------------------------------------------------------------------------
void ViewCamera::SetParams()
{
	left.Reset();
	right.Reset();
	middle.Reset();

	angle[0] = DegToRad(180.0);
	angle[1] = 0.0;
	angle[2] = 0.0;//DegToRad(180.0);
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;
	target[0] = 0.0;
	target[1] = 0.0;
	target[2] = 0.0;
	upvector[0] = 0.0;
	upvector[1] = 1.0;
	upvector[2] = 0.0;
	translate[0] = 0.0;
	translate[1] = 0.0;
	translate[2] = 0.0;
}
