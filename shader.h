/*==============================================================================

   シェーダー [shader.h]
                                                         Author : Youhei Sato
                                                         Date   : 2025/05/15
--------------------------------------------------------------------------------
   授業のひな形（DX21）をもとに、3D用に World / View / Projection・
   マテリアル・ライトを渡せるように拡張したもの
==============================================================================*/
#pragma once

#include "dx11.h"

// マテリアル（物体の色）
// ※ 定数バッファは16バイト単位なので Dummy で詰めている。shaderCommon.hlsli と並びを合わせること
struct MATERIAL
{
	XMFLOAT4 Diffuse;		// 拡散色（ライトが当たっている部分の色）
	XMFLOAT4 Ambient;		// 環境色（影の部分の色）
	BOOL     TextureEnable;	// テクスチャを使うか
	float    Dummy[3];
};

// 平行光源（太陽のような、一方向から当たる光）
struct LIGHT
{
	BOOL     Enable;		// ライトの計算をするか（2D描画のときは FALSE）
	float    Dummy[3];
	XMFLOAT4 Direction;		// 光の進む向き
	XMFLOAT4 Diffuse;		// 光の色
	XMFLOAT4 Ambient;		// 環境光の色（光が当たらない面の明るさ）
};

bool Shader_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader_Finalize();

void Shader_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void Shader_SetViewMatrix(const DirectX::XMMATRIX& matrix);
void Shader_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);
void Shader_Set2DMatrix();	// 2D描画用（左上が原点・ピクセル単位）の行列をまとめて設定

void Shader_SetMaterial(const MATERIAL& material);
void Shader_SetLight(const LIGHT& light);

// 水のレイトレーシングに使う情報（カメラの位置・経過時間・水に映す立方体・地形の大きさと高さ）
// 地形のテクスチャ（高さ・法線・色）は water.cpp で t1・t2 に設定する
void Shader_SetWater(const XMFLOAT3& cameraPosition, float time, const XMMATRIX& cubeWorld, const XMFLOAT4& cubeColor,
	float fieldSize, float fieldMaxHeight);

void Shader_Begin();		// ふつうの3D／2D用のシェーダーに切り替える
void Shader_BeginWater();	// 水用のシェーダーに切り替える（描き終わったら Shader_Begin で戻す）
