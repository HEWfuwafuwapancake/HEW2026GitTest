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

/// @file shader.h
/// @brief シェーダーの読み込みと、行列・マテリアル・ライト・水の情報をシェーダーに渡す関数

/// @brief マテリアル（物体の色）
/// @details 定数バッファは16バイト単位なので Dummy で詰めている。shaderCommon.hlsli と並びを合わせること
struct MATERIAL
{
	XMFLOAT4 Diffuse;		///< 拡散色（ライトが当たっている部分の色）
	XMFLOAT4 Ambient;		///< 環境色（影の部分の色）
	BOOL     TextureEnable;	///< テクスチャを使うか
	float    Dummy[3];
};

/// @brief 平行光源（太陽のような、一方向から当たる光）
struct LIGHT
{
	BOOL     Enable;		///< ライトの計算をするか（2D描画のときは FALSE）
	float    Dummy[3];
	XMFLOAT4 Direction;		///< 光の進む向き
	XMFLOAT4 Diffuse;		///< 光の色
	XMFLOAT4 Ambient;		///< 環境光の色（光が当たらない面の明るさ）
};

/// @brief シェーダー（ふつうの3D用と水用）と定数バッファを作る
/// @param pDevice デバイス
/// @param pContext デバイスコンテキスト
/// @return 成功したら true（.cso が見つからないときはメッセージボックスを出して false）
bool Shader_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

/// @brief シェーダーと定数バッファを解放する
void Shader_Finalize();

/// @brief ワールド行列を設定する（物の位置・回転・大きさ）
/// @param matrix ワールド行列
void Shader_SetWorldMatrix(const DirectX::XMMATRIX& matrix);

/// @brief ビュー行列を設定する（カメラの位置と向き）
/// @param matrix ビュー行列
void Shader_SetViewMatrix(const DirectX::XMMATRIX& matrix);

/// @brief プロジェクション行列を設定する（遠近の付け方）
/// @param matrix プロジェクション行列
void Shader_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);

/// @brief 2D描画用（左上が原点・ピクセル単位）の行列をまとめて設定する
void Shader_Set2DMatrix();

/// @brief マテリアル（物体の色）を設定する
/// @param material マテリアル
void Shader_SetMaterial(const MATERIAL& material);

/// @brief ライトを設定する
/// @param light 平行光源（2D描画のときは Enable を FALSE にする）
void Shader_SetLight(const LIGHT& light);

/// @brief 水のレイトレーシングに使う情報を設定する
/// @details 地形のテクスチャ（高さ・法線・色）は water.cpp で t1・t2 に設定する
/// @param cameraPosition カメラの位置（視線の向きを求める）
/// @param time 経過時間（秒）。波を動かす
/// @param cubeWorld 水に映す立方体のワールド行列
/// @param cubeColor 水に映す立方体の色
/// @param fieldSize 地形の1辺の長さ（地形テクスチャの UV を求める）
/// @param fieldMaxHeight 地形のいちばん高いところ
void Shader_SetWater(const XMFLOAT3& cameraPosition, float time, const XMMATRIX& cubeWorld, const XMFLOAT4& cubeColor,
	float fieldSize, float fieldMaxHeight);

/// @brief ふつうの3D／2D用のシェーダーに切り替える
void Shader_Begin();

/// @brief 水用のシェーダーに切り替える（描き終わったら Shader_Begin で戻す）
void Shader_BeginWater();
