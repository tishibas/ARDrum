//----------------------------------------------------
// File : XLoader.cpp
//
// DirectX X File Loader Library
//
// Date : May. 10, 2008
// Version : 1.0
// Author : Pocol
//----------------------------------------------------

//
// include
//
#include "XLoader.h"

//
// global Variables
//
static char* Pointer;
static char Token[1024];


//-------------------------------------------------------------------------------------------------
// Name : CheckToken()
// Desc : 指定された文字列とトークンをチェックする
//-------------------------------------------------------------------------------------------------
static bool CheckToken(const char* str)
{
	if ( strcmp(Token, str) == 0 )
		return true;
	else
		return false;
}

//-------------------------------------------------------------------------------------------------
// Name : GetToken()
// Desc : トークンを取得する
//-------------------------------------------------------------------------------------------------
static void GetToken()
{
	char *p = Pointer;
	char *q = Token;

	while ( *p != '\0' && strchr(" \t\r\n,;\"", *p) ) 
		p++;

	if ( *p == '{' || *p == '}' )
		(*q++) = (*p++);
	else
		while ( *p != '\0' && !strchr(" \t\r\n,;\"{}", *p ) )
			(*q++) = (*p++);

	Pointer = p;
	*q = '\0';
}

//--------------------------------------------------------------------------------------------------
// Name : GetToken()
// Desc : トークンを取得し指定された文字列を比較を行う
//--------------------------------------------------------------------------------------------------
static bool GetToken(const char* token)
{
	GetToken();
	if ( strcmp(Token, token) != 0 )
	{
		cout << "Error : 想定トークンと読込トークンが一致しません\n";
		cout << "想定トークン：" << token << endl;
		cout << "読込トークン：" << Token << endl;
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------------
// Name : GetFloatToken()
// Desc : トークンをfloat型に変換し取得する
//--------------------------------------------------------------------------------------------------
static float GetFloatToken()
{
	GetToken();
	return (float)atof(Token);
}

//--------------------------------------------------------------------------------------------------
// Name : GetIntToken()
// Desc : トークンをint型に変換し取得する
//--------------------------------------------------------------------------------------------------
static int GetIntToken()
{
	GetToken();
	return (int)atoi(Token);
}

//-------------------------------------------------------------------------------------------------
// Name : SkipNode()
// Desc : ノードを飛ばす
//-------------------------------------------------------------------------------------------------
static void SkipNode()
{
	while ( *Pointer != '\0' )
	{
		GetToken();
		if ( strchr(Token, '{' ) ) break;
	}
	int count = 1;
	while ( *Pointer != '\0' && count > 0 )
	{
		GetToken();
		if ( strchr(Token, '{' ) ) count++;
		else if ( strchr(Token, '}') ) count--;
	}
	if ( count > 0 )
	{
		cout << "Error : カッコが一致していません\n";
		return;
	}
}

////////////////////////////////////////////////////////////////////////
// XFace class
////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Name : XFace()
// Desc : コンストラクタ
//-------------------------------------------------------------------------------------------------
XFace::XFace()
{
	element = 0;
	indexMaterial = -1;
	for(int i=0; i<4; i++ )
	{
		indexVertices[i] = -1;
		indexNormals[i] = -1;
		indexTexCoords[i] = -1;
	}
}

//--------------------------------------------------------------------------------------------------
// operator =
//--------------------------------------------------------------------------------------------------
XFace& XFace::operator =(XFace &ob)
{
	element = ob.element;
	indexMaterial = ob.indexMaterial;
	for(int i=0; i<4; i++ )
	{
		indexVertices[i] = ob.indexVertices[i];
		indexNormals[i] = ob.indexNormals[i];
		indexTexCoords[i] = ob.indexTexCoords[i];
	}
	return (*this);
}

//--------------------------------------------------------------------------------------------------
// Name : SetVertexIndex()
// Desc : 頂点インデックスをセットする
//--------------------------------------------------------------------------------------------------
void XFace::SetVertexIndex(int index[])
{
	for ( int i=0; i<4; i++ ) indexVertices[i] = index[i];
}

//--------------------------------------------------------------------------------------------------
// Name : SetNormalIndex()
// Desc : 法線インデックスをセットする
//--------------------------------------------------------------------------------------------------
void XFace::SetNormalIndex(int index[])
{
	for ( int i=0; i<4; i++ ) indexNormals[i] = index[i];
}

//--------------------------------------------------------------------------------------------------
// Name : SetTexCoordIndex()
// Desc : テクスチャ座標インデックスをセットする
//--------------------------------------------------------------------------------------------------
void XFace::SetTexCoordIndex(int index[])
{
	for ( int i=0; i<4; i++ ) indexTexCoords[i] = index[i];
}

////////////////////////////////////////////////////////////////////////
// XMaterial class
////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------
// Name : XMaterial()
// Desc : コンストラクタ
//--------------------------------------------------------------------------------------------------
XMaterial::XMaterial()
{
	strcpy(name, "null");
	ambient.r = 0.0f;	ambient.g = 0.0f;	ambient.b = 0.0f;	ambient.a = 0.0f;
	diffuse.r = 0.0f;	diffuse.g = 0.0f;	diffuse.b = 0.0f;	diffuse.a = 0.0f;
	specular.r = 0.0f;	specular.g = 0.0f;	specular.b = 0.0f;	specular.a = 0.0f;
	emissive.r = 0.0f;	emissive.g = 0.0f;	emissive.g = 0.0f;	emissive.a = 0.0f;
	power = 0.0f;
	strcpy(textureFileName, "null");
}

//--------------------------------------------------------------------------------------------------
// operator =
//--------------------------------------------------------------------------------------------------
XMaterial& XMaterial::operator =(XMaterial &ob)
{
	strcpy(name, ob.name);
	ambient = ob.ambient;
	diffuse = ob.diffuse;
	specular = ob.specular;
	emissive = ob.emissive;
	power = ob.power;
	strcpy(textureFileName, ob.textureFileName);
	return (*this);
}

//--------------------------------------------------------------------------------------------------
// Name : SetName()
// Desc : 名前をセットする
//--------------------------------------------------------------------------------------------------
void XMaterial::SetName(char *str)
{
	strcpy(name, str);
}

//--------------------------------------------------------------------------------------------------
// Name : SetTextureFileName()
// Desc : テクスチャファイル名をセットする
//--------------------------------------------------------------------------------------------------
void XMaterial::SetTextureFileName(char *str)
{
	strcpy(textureFileName, str);
}

////////////////////////////////////////////////////////////////////////
// XMesh class
////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------
// Name : XMesh()
// Desc : コンストラクタ
//---------------------------------------------------------------------------------------------------
XMesh::XMesh()
{
	strcpy(name, "null");
	numVertices = 0;
	numNormals = 0;
	numTexCoords = 0;
	numFaces = 0;
	vertex = (XVector3*)malloc(1*sizeof(XVector3));
	normal = (XVector3*)malloc(1*sizeof(XVector3));
	texcoord = (XVector2*)malloc(1*sizeof(XVector2));
	face = (XFace*)malloc(1*sizeof(XFace));
}

//--------------------------------------------------------------------------------------------------
// operator =
//--------------------------------------------------------------------------------------------------
XMesh& XMesh::operator= (XMesh &ob)
{
	strcpy(name, ob.name);
	numVertices = ob.numVertices;
	numNormals = ob.numNormals;
	numTexCoords = ob.numTexCoords;
	numFaces = ob.numFaces;

	vertex = (XVector3*)malloc(numVertices*sizeof(XVector3));
	normal = (XVector3*)malloc(numNormals*sizeof(XVector3));
	texcoord = (XVector2*)malloc(numTexCoords*sizeof(XVector2));
	face = (XFace*)malloc(numFaces*sizeof(XFace));

	for(int i=0; i<numVertices; i++ )		vertex[i] = ob.vertex[i];
	for(int i=0; i<numNormals; i++ )		normal[i] = ob.normal[i];
	for(int i=0; i<numTexCoords; i++)		texcoord[i] = ob.texcoord[i];
	for(int i=0; i<numFaces; i++ )		face[i] = ob.face[i];
	
	return (*this);
}

//--------------------------------------------------------------------------------------------------
// Name : AddVertex()
// Desc : 頂点を追加する
//--------------------------------------------------------------------------------------------------
int XMesh::AddVertex(XVector3 &ob)
{
	numVertices++;
	vertex = (XVector3*)realloc(vertex, numVertices*sizeof(XVector3));
	vertex[numVertices-1] = ob;
	return numVertices;
}

//--------------------------------------------------------------------------------------------------
// Name : AddNormal()
// Desc : 法線を追加する
//--------------------------------------------------------------------------------------------------
int XMesh::AddNormal(XVector3 &ob)
{
	numNormals++;
	normal = (XVector3*)realloc(normal, numNormals*sizeof(XVector3));
	normal[numNormals-1] = ob;
	return numNormals;
}

//--------------------------------------------------------------------------------------------------
// Name : AddTexCoord()
// Desc : テクスチャ座標を追加する
//--------------------------------------------------------------------------------------------------
int XMesh::AddTexCoord(XVector2 &ob)
{
	numTexCoords++;
	texcoord = (XVector2*)realloc(texcoord, numTexCoords*sizeof(XVector2));
	texcoord[numTexCoords-1] = ob;
	return numTexCoords;
}

//--------------------------------------------------------------------------------------------------
// Name : AddFace()
// Desc : 面を追加する
//--------------------------------------------------------------------------------------------------
int XMesh::AddFace(XFace &ob)
{
	numFaces++;
	face = (XFace*)realloc(face, numFaces*sizeof(XFace));
	face[numFaces-1] = ob;
	return numFaces;
}

//--------------------------------------------------------------------------------------------------
// Name : Release()
// Desc : 確保したメモリを解放する
//--------------------------------------------------------------------------------------------------
void XMesh::Release()
{
	if ( vertex )
	{
		free((XVector3*)vertex);
		vertex = NULL;
	}

	if ( normal )
	{
		free((XVector3*)normal);
		normal = NULL;
	}

	if ( texcoord )
	{
		free((XVector2*)texcoord);
		texcoord = NULL;
	}

	if ( face )
	{
		free((XFace*)face);
		face = NULL;
	}

	numVertices = 0;
	numNormals = 0;
	numTexCoords = 0;
	numFaces = 0;
}

//---------------------------------------------------------------------------------------------------
// Name : SetName()
// Desc : 名前をセットする
//---------------------------------------------------------------------------------------------------
void XMesh::SetName(char *str)
{
	strcpy(name, str);
}

////////////////////////////////////////////////////////////////////////
// XModel class
////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------
// Name : XModel()
// Desc : コンストラクタ
//---------------------------------------------------------------------------------------------------
XModel::XModel()
{
	numMeshes = 0;
	numMaterials = 0;
	mesh = (XMesh*)malloc(1*sizeof(XMesh));
	material = (XMaterial*)malloc(1*sizeof(XMaterial));
}

//--------------------------------------------------------------------------------------------------
// Name : AddMesh()
// Desc : メッシュを追加する
//--------------------------------------------------------------------------------------------------
int XModel::AddMesh(XMesh ob)
{
	numMeshes++;
	mesh = (XMesh*)realloc(mesh, numMeshes*sizeof(XMesh));
	mesh[numMeshes-1] = ob;
	return numMeshes;
}

//--------------------------------------------------------------------------------------------------
// Name : AddMaterial()
// Desc : マテリアルを追加する
//--------------------------------------------------------------------------------------------------
int XModel::AddMaterial(XMaterial ob)
{
	numMaterials++;
	material = (XMaterial*)realloc(material, numMaterials*sizeof(XMaterial));
	material[numMaterials-1] = ob;
	return numMaterials;
}

//--------------------------------------------------------------------------------------------------
// Name : Release()
// Desc : 確保したメモリを解放する
//--------------------------------------------------------------------------------------------------
void XModel::Release()
{
	if ( material )
	{
		free((XMaterial*)material);
		material = NULL;
	}
	if ( mesh )
	{
		free((XMesh*)mesh);
		mesh = NULL;
	}
	numMaterials = 0;
	numMeshes = 0;
}

//--------------------------------------------------------------------------------------------------
// Name : ComputeBoundingSphere()
// Desc : バウンディングスフィアを計算する
//--------------------------------------------------------------------------------------------------
void XModel::ComputeBoundingSphere()
{
	int count = 0;
	float tempRadius = 0.0f;
	XVector3 tempCenter;
	tempCenter.x = 0.0f;
	tempCenter.y = 0.0f;
	tempCenter.z = 0.0f;

	//　中心座標の算出
	for ( int i=0; i<numMeshes; i++ )
	{
		for ( int j=0; j<mesh[i].numVertices; j++ )
		{
			tempCenter.x += mesh[i].vertex[j].x;
			tempCenter.y += mesh[i].vertex[j].y;
			tempCenter.z += mesh[i].vertex[j].z;
			count++;
		}
	}
	tempCenter.x /= (float)count;
	tempCenter.y /= (float)count;
	tempCenter.z /= (float)count;

	//　半径の算出
	for (int i=0; i<numMeshes; i++ )
	{
		for ( int j=0; j<mesh[i].numVertices; j++ )
		{
			float d = 0.0f;
			XVector3 temp;
			temp.x = mesh[i].vertex[j].x - tempCenter.x;
			temp.y = mesh[i].vertex[j].y - tempCenter.y;
			temp.z = mesh[i].vertex[j].z - tempCenter.z;
			d = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
			tempRadius = (tempRadius < d ) ? d : tempRadius;
		}
	}

	//　結果を格納
	sphere.center = tempCenter;
	sphere.radius = tempRadius;
}

//--------------------------------------------------------------------------------------------------
// Name : ComputeBoundingBox()
// Desc : バウンディングボックスを計算する
//--------------------------------------------------------------------------------------------------
void XModel::ComputeBoundingBox()
{
	XVector3 tempMin, tempMax;

	//　最初の頂点で初期化
	tempMin = tempMax = mesh[0].vertex[0];

	for ( int i=0; i<numMeshes; i++ )
	{
		for ( int j=0; j<mesh[i].numVertices; j++ )
		{
			//　x成分
			if ( tempMin.x > mesh[i].vertex[j].x ) tempMin.x = mesh[i].vertex[j].x;
			if ( tempMax.x < mesh[i].vertex[j].x ) tempMax.x = mesh[i].vertex[j].x;

			//　y成分
			if ( tempMin.y > mesh[i].vertex[j].y ) tempMin.y = mesh[i].vertex[j].y;
			if ( tempMax.y < mesh[i].vertex[j].y ) tempMax.y = mesh[i].vertex[j].y;

			//　z成分
			if ( tempMin.z > mesh[i].vertex[j].z ) tempMin.z = mesh[i].vertex[j].z;
			if ( tempMax.z < mesh[i].vertex[j].z ) tempMax.z = mesh[i].vertex[j].z;
		}
	}

	//　結果を格納
	box.min = tempMin;
	box.max = tempMax;
}

//--------------------------------------------------------------------------------------------------
// Name : Load()
// Desc : テキストフォーマットのXファイルをロードする
//--------------------------------------------------------------------------------------------------
bool XModel::Load(char *filename)
{
	//　カウント用変数など	
	bool b_Mesh = false;
	int matCount = 0;
	int vertCount = 0;
	int faceCount = 0;
	int normCount = 0;
	int uvCount = 0;
	int meshCount = 0;
	
	XFace* tempFace;
	XMesh tempMesh;

	//　ファイルを読み込む
	HANDLE file = CreateFileA(
		filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

	//　チェック
	if ( file == INVALID_HANDLE_VALUE )
	{
		cout << "Error : 指定されたファイルの読み込みに失敗しました\n";
		cout << "File Name : " << filename << endl;
		return false;
	}

	//　ファイルサイズを取得
	DWORD size = GetFileSize(file, NULL);
	char* buffer = new char[size+1];
	buffer[size] = '\0';

	//　バッファに格納
	DWORD read_size;
	ReadFile(file, buffer, size, &read_size, NULL);

	//　ファイルを閉じる
	CloseHandle(file);

	//　サイズチェック
	if ( read_size != size )
	{
		cout << "Error : 読み込みサイズとサイズが一致していません\n";
		return false;
	}

	//　Pointerに読み込んだバッファをセット
	Pointer = buffer;

	//　ループ
	while ( *Pointer != '\0' )
	{
		//　トークンを取得
		GetToken();

		//　templateの場合
		if ( CheckToken("template") )
		{
			//　スキップする
			SkipNode();
		}

		//　Meshの場合
		else if ( CheckToken("Mesh") )
		{
			//　トークンを取得
			GetToken();

			//　トークンが" { " でない場合
			if ( !CheckToken("{") )
			{
				//　名前をセット
				tempMesh.SetName(Token);

				//　トークンを取得
				GetToken("{");
			}

			//　トークンが" { "の場合
			else
			{
				//　名前を決める
				char tempName[MAX_PATH] = "obj";
				sprintf(tempName, "%s%d", tempName, meshCount+1);

				//　名前をセット
				tempMesh.SetName(tempName);
			}

			//　メッシュ数をカウントする
			meshCount++;

			//　カウント数が1より大きい場合
			if ( meshCount > 1 )
			{
				//　面データを追加
				for(int i=0; i<faceCount; i++ )
					tempMesh.AddFace(tempFace[i]);
				
				//　メッシュを追加
				AddMesh(tempMesh);

				//　確保したメモリを解放
				SafeDeleteArray(tempFace);

				//　ゼロに戻す
				ZeroMemory(&tempMesh, sizeof(tempMesh));
			}

			//　頂点数のカウンターを0に戻す
			vertCount = 0;

			//　トークンから頂点数を取得
			vertCount = GetIntToken();

			for( int i=0; i<vertCount; i++ )
			{
				//　トークンから頂点データを取得
				XVector3 tempVertex;
				tempVertex.x = GetFloatToken();
				tempVertex.y = GetFloatToken();
				tempVertex.z = GetFloatToken();

				//　頂点データを追加
				tempMesh.AddVertex(tempVertex);
			}

			//　頂点数をチェック
			if ( tempMesh.numVertices != vertCount )
			{
				cout << "Error : 頂点数が一致していません\n";
				return false;
			}

			//　面数のカウンターを0に戻す
			faceCount = 0;

			//　トークンから面数を取得
			faceCount = GetIntToken();

			//　メモリを確保
			tempFace = new XFace[faceCount];

			//　トークンから面データを取得
			for( int i=0; i<faceCount; i++ )
			{
				int tempIndex[4] = { -1, -1, -1, -1 };
				tempFace[i].element = 0;
				tempFace[i].indexMaterial = -1;

				//　トークンを取得
				GetToken();
			
				//　三角形の場合
				if ( CheckToken("3") )
				{
					//　要素数は3にする
					tempFace[i].element = 3;

					//　トークンから頂点インデックスを取得
					for( int j=0; j<3; j++ )
					{
						tempIndex[j] = GetIntToken();
					}

					//　4番目のインデックスには-1を格納
					tempIndex[3] = -1;

					//　頂点インデックスをセット
					tempFace[i].SetVertexIndex(tempIndex);

					//　テクスチャ座標インデックスをセット
					tempFace[i].SetTexCoordIndex(tempIndex);
				}

				//　四角形の場合
				else if ( CheckToken("4") )
				{
					//　要素数は4にする
					tempFace[i].element = 4;

					//　トークンから頂点インデックスを取得
					for ( int j=0; j<4; j++ )
					{
						tempIndex[j] = GetIntToken();
					}

					//　頂点インデックスをセット
					tempFace[i].SetVertexIndex(tempIndex);

					//　テクスチャ座標インデックスをセット
					tempFace[i].SetTexCoordIndex(tempIndex);
				}
			}
		}

		//　MeshNormalsの場合
		else if ( CheckToken("MeshNormals") )
		{
			//　トークンを取得
			GetToken("{");

			//　法線数のカウンターを0に戻す
			normCount = 0;

			//　トークンから法線数を取得
			normCount = GetIntToken();

			//　トークンから法線データを取得
			for ( int i=0; i<normCount; i++ )
			{
				XVector3 tempNormal;
				tempNormal.x = GetFloatToken();
				tempNormal.y = GetFloatToken();
				tempNormal.z = GetFloatToken();

				//　法線データを追加
				tempMesh.AddNormal(tempNormal);
			}

			//　法線数をチェック
			if ( tempMesh.numNormals != normCount )
			{
				cout << "Error : 法線数が一致していません\n";
				return false;
			}

			//　法線インデックス数をチェック
			if ( GetIntToken() != faceCount )
			{
				cout << "Error : 面数と法線インデックス数が一致していません\n";
				return false;
			}

			for ( int i=0; i<normCount; i++ )
			{
				int tempIndex[4] = { -1, -1, -1, -1 };

				//　トークンを取得
				GetToken();

				//　三角形の場合
				if ( CheckToken("3") )
				{
					//　トークンから法線インデックスを取得
					for ( int j=0; j<3; j++ )
					{
						tempIndex[j] = GetIntToken();
					}

					//　4番目のインデックスには-1をセット
					tempIndex[3] = -1;

					//　法線インデックスをセット
					tempFace[i].SetNormalIndex(tempIndex);
				}

				//　四角形の場合
				else if( CheckToken("4") )
				{
					//　法線インデックスを取得
					for ( int j=0; j<4; j++ )
					{
						tempIndex[j] = GetIntToken();
					}
					
					//　法線インデックスをセット
					tempFace[i].SetNormalIndex(tempIndex);
				}
			}
		}

		//　MeshTextureCoordsの場合
		else if ( CheckToken("MeshTextureCoords") )
		{
			//　トークンを取得
			GetToken("{");

			//　テクスチャ座標数のカウンターを0に戻す
			uvCount = 0;

			//　トークンからテクスチャ座標数を取得
			uvCount = GetIntToken();

			for ( int i=0; i<uvCount; i++ )
			{
				//　トークンからテクスチャ座標データを取得
				XVector2 tempUV;
				tempUV.x = GetFloatToken();
				tempUV.y = GetFloatToken();

				//　テクスチャ座標データを追加
				tempMesh.AddTexCoord(tempUV);
			}
		}

		//　MeshMaterialListの場合
		else if ( CheckToken("MeshMaterialList") )
		{
			//　トークンを取得
			GetToken("{");

			//　マテリアル数のカウンターを0に戻す
			matCount = 0;

			//　トークンからマテリアル数を取得
			matCount = GetIntToken();

			//　マテリアル数をチェック
			if ( GetIntToken() != faceCount )
			{
				cout << "Error : 面数とマテリアルリスト数が一致しません\n";
				return false;
			}

			//　トークンからマテリアルインデックスを取得
			for ( int i=0; i<faceCount; i++ )
			{
				tempFace[i].indexMaterial = GetIntToken();
			}

			for ( int i=0; i<matCount; i++ )
			{
				XMaterial tempMaterial;
			
				//　トークンを取得
				GetToken("Material");
				GetToken();

				//　トークンが" { "でない場合
				if ( !CheckToken("{") )
				{
					//　名前をセット
					tempMaterial.SetName(Token);
				
					//　トークンを取得
					GetToken("{");
				}

				//　トークンが" { "の場合
				else
				{
					//　名前を決める
					char tempName[MAX_PATH] = "mat";
					sprintf(tempName, "%s%d", tempName, i+1);
		
					//　名前をセット
					tempMaterial.SetName(tempName);
				}

				//　Ambient Color
				tempMaterial.ambient.r = 0.15f;
				tempMaterial.ambient.g = 0.15f;
				tempMaterial.ambient.g = 0.15f;
				tempMaterial.ambient.a = 1.0f;

				//　Diffuse Color
				tempMaterial.diffuse.r = GetFloatToken();
				tempMaterial.diffuse.g = GetFloatToken();
				tempMaterial.diffuse.b = GetFloatToken();
				tempMaterial.diffuse.a = GetFloatToken();

				//　Shiness
				tempMaterial.power = GetFloatToken();

				//　Specular Color
				tempMaterial.specular.r = GetFloatToken();
				tempMaterial.specular.g = GetFloatToken();
				tempMaterial.specular.b = GetFloatToken();
				tempMaterial.specular.a = 1.0f;

				//　Emissive Color
				tempMaterial.emissive.r = GetFloatToken();
				tempMaterial.emissive.g = GetFloatToken();
				tempMaterial.emissive.b = GetFloatToken();
				tempMaterial.emissive.a = 1.0f;

				//　トークンを取得
				GetToken();

				//　TextureFileNameの場合
				if ( CheckToken("TextureFileName") )
				{
					//　トークンを取得
					GetToken("{");

					//　トークンを取得
					GetToken();

					//　ファイル名をセット
					tempMaterial.SetTextureFileName(Token);

					//　トークンを取得
					GetToken("}");
				}

				//　トークンが" } "の場合
				else if ( CheckToken("}") )
				{
					//　マテリアルデータを追加
					AddMaterial(tempMaterial);
				}
			}
		}
	}

	//　メッシュ数が1以上の場合
	if ( meshCount >= 1 )
	{
		//　面データを追加
		for ( int i=0; i<faceCount; i++ )
			tempMesh.AddFace(tempFace[i]);
		
		//　メッシュデータを追加
		AddMesh(tempMesh);

		//　確保したメモリを解放
		SafeDeleteArray(tempFace);
	}

	//　バウンディングスフィアを計算
	ComputeBoundingSphere();

	//　バウンディングボックスを計算
	ComputeBoundingBox();

	return true;
}

//---------------------------------------------------------------------------------------------------
// Name : Render()
// Desc : メッシュのインデックスを指定して描画
//---------------------------------------------------------------------------------------------------
void XModel::Render(int index, float scale)
{
	int pre_mat = -1;
	int cur_mat = 0;
	bool exist_material = false;
	bool exist_texcoord = false;
	bool exist_normal = false;

	if ( numMaterials > 0 ) exist_material = true;
	if ( mesh[index].numTexCoords > 0 ) exist_texcoord = true;
	if ( mesh[index].numNormals > 0 ) exist_normal = true;

	for ( int i=0; i<mesh[index].numFaces; i++ )
	{
		//　マテリアルが存在する場合
		if ( exist_material )
		{
			//　マテリアルインデックスを取得
			cur_mat = mesh[index].face[i].indexMaterial;

			//　マテリアルインデックスが異なる場合
			if ( cur_mat != pre_mat )
			{
				//　基本色
				glColor4fv(material[cur_mat].diffuse);

				//　Ambient Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material[cur_mat].ambient);

				//　Diffuse Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material[cur_mat].diffuse);

				//　Specular Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material[cur_mat].specular);

				//　Emissive Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material[cur_mat].emissive);

				//　Shininess
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material[cur_mat].power);

				//　マテリアルインデックスを更新
				pre_mat = cur_mat;
			}
		}

		//　要素数が3の場合
		if ( mesh[index].face[i].element == 3 ) 
			glBegin(GL_TRIANGLES);

		//　要素数が4の場合
		else if ( mesh[index].face[i].element == 4 )
			glBegin(GL_QUADS);

		for ( int j=0; j<mesh[index].face[i].element; j++ )
		{
			//　テクスチャ座標が存在する場合
			if ( exist_texcoord )
				glTexCoord2fv(mesh[index].texcoord[mesh[index].face[i].indexTexCoords[j]]);

			//　法線ベクトルが存在する場合
			if ( exist_normal )
				glNormal3fv(mesh[index].normal[mesh[index].face[i].indexNormals[j]]);
			
			//　頂点座標
			glVertex3f(
				mesh[index].vertex[mesh[index].face[i].indexVertices[j]].x * scale,
				mesh[index].vertex[mesh[index].face[i].indexVertices[j]].y * scale,
				mesh[index].vertex[mesh[index].face[i].indexVertices[j]].z * scale );
		}

		//　
		glEnd();
	}
}

//---------------------------------------------------------------------------------------------------
// Name : Render()
// Desc : 描画処理
//---------------------------------------------------------------------------------------------------
void XModel::Render(float scale)
{
	for ( int i=0; i<numMeshes; i++ )
	{
		Render(i, scale);
	}
}