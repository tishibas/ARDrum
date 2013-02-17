#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#include "GLMetaseq.h"

struct Model{
	MQO_MODEL mqo;
	int state;
};


void renderModel(Model &model, Model &actModel, float x, float y, float z, float a, int actFlg){
	// アクションがあれば3フレームアクション用の画像を表示する
	if(actFlg == 1) model.state = 3;

	glPushMatrix();
	
	if( model.state == 0 ){
		// 通常時
		glTranslatef(x, y, z);						// 平行移動（横、縦、奥行き）
		glRotatef( 45/180.0 * 3.14, 0, 1, 0 );		// Y軸回転
		glRotatef( a, 1, 0, 0 );					// X軸回転
		glScalef( 0.5f, 0.5f, 0.5f );				// スケール変換

		mqoCallModel(model.mqo);					// MQOモデルのコール		
	}
	else{
		// アクション用
		glTranslatef(x, y-15, z);					// 平行移動（横、縦、奥行き）
		glRotatef( 45/180.0 * 3.14, 0, 1, 0 );		// Y軸回転
		glRotatef( a, 1, 0, 0 );					// X軸回転
		glScalef( 0.5f, 0.5f, 0.5f );				// スケール変換

		mqoCallModel(actModel.mqo);					 // MQOモデルのコール	
	
		if(model.state > 0) model.state--;
	}
	
	glPopMatrix();	
}

