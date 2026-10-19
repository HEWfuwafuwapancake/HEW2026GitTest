#pragma once

#include "dx11.h"

// 地面（湖のある地形）
// 原点を中心に湖があり、まわりはなだらかな丘になっている

static constexpr float FIELD_WATER_LEVEL = 0.0f;	// 水面の高さ

void FieldInitialize(void);
void FieldFinalize(void);
void FieldUpdate(void);
void FieldDraw(void);

// 地形の高さ（x, z の位置の地面の y）
// ※ 地形の式はここ（field.cpp）だけ。水のシェーダーには、この式で作ったテクスチャを渡している
float FieldGetHeight(float x, float z);

// 高さに応じた地面の色（砂浜 → 草 → 濃い草）
XMFLOAT3 FieldGetColor(float height);

// 水のシェーダーに渡す地形の情報
// テクスチャは地形全体（FieldGetSize() 四方）を覆う。地形の中心が UV (0.5, 0.5)
ID3D11ShaderResourceView* FieldGetHeightNormalTexture(void);	// r：高さ、gba：法線
ID3D11ShaderResourceView* FieldGetColorTexture(void);			// rgb：地面の色
float FieldGetSize(void);		// 地形の1辺の長さ
float FieldGetMaxHeight(void);	// 地形のいちばん高いところの高さ
