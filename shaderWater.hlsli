// 水のシェーダー（頂点・ピクセル）で共通の定義
#include "shaderCommon.hlsli"

// 水用の定数バッファ（shader.cpp の WATER 構造体と並びを合わせる）
cbuffer WaterBuffer : register(b5)
{
	float3 CameraPosition;	// カメラの位置（視線の向きを求める）
	float  Time;			// 経過時間（秒）。波を動かす
	matrix CubeInvWorld;	// 立方体のワールド行列の逆行列（光線を立方体の座標に直す）
	float4 CubeColor;		// 立方体の色
	float  FieldSize;		// 地形の1辺の長さ（地形テクスチャの UV を求める）
	float  FieldMaxHeight;	// 地形のいちばん高いところ
	float2 WaterDummy;
}

// 水の頂点シェーダーの出力 = ピクセルシェーダーの入力
struct WATER_PS_IN
{
	float4 Position : SV_POSITION;
	float3 WorldPos : TEXCOORD0;	// 水面上の位置（ワールド座標）
};
