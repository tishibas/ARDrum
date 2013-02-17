#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#include "GLMetaseq.h"

struct Model{
	MQO_MODEL mqo;
	int state;
};

void renderModel(Model &model, float x, float y, float z, float a){


	glPushMatrix();
	glLoadIdentity();
	
	if( model.state == 0 ){

		glTranslatef(x, y, z);						// 平行移動（横、縦、奥行き）
		glRotatef( 45/180.0 * 3.14, 0, 1, 0 );		// Y軸回転
		glRotatef( a, 1, 0, 0 );					// X軸回転
		glScalef( 0.5f, 0.5f, 0.5f );

		mqoCallModel(model.mqo);						// MQOモデルのコール
		glPopMatrix();	
	}
	else{




	}
	

}

