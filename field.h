#pragma once

#include "dx11.h"

/// @file field.h
/// @brief 地面（湖のある地形）
/// @details 原点を中心に湖があり、まわりはなだらかな丘になっている

static constexpr float FIELD_WATER_LEVEL = 0.0f;	///< 水面の高さ

/// @brief 地形の頂点・インデックスバッファと、水のシェーダーに渡すテクスチャを作る
void FieldInitialize(void);

/// @brief 地形のバッファとテクスチャを解放する
void FieldFinalize(void);

/// @brief 地形の更新（今は何もしない）
void FieldUpdate(void);

/// @brief 地形を描く
void FieldDraw(void);

/// @brief 地形の高さを取得する
/// @details 地形の式はここ（field.cpp）だけ。水のシェーダーには、この式で作ったテクスチャを渡している
/// @param x 位置の x（ワールド座標）
/// @param z 位置の z（ワールド座標）
/// @return その位置の地面の y
float FieldGetHeight(float x, float z);

/// @brief 高さに応じた地面の色を取得する（砂浜 → 草 → 濃い草）
/// @param height 地面の高さ
/// @return 地面の色（RGB）
XMFLOAT3 FieldGetColor(float height);

/// @brief 水のシェーダーに渡す、地形の高さと法線のテクスチャを取得する
/// @details テクスチャは地形全体（FieldGetSize() 四方）を覆う。地形の中心が UV (0.5, 0.5)
/// @return r：高さ、gba：法線（Release しないこと）
ID3D11ShaderResourceView* FieldGetHeightNormalTexture(void);

/// @brief 水のシェーダーに渡す、地面の色のテクスチャを取得する
/// @details テクスチャは地形全体（FieldGetSize() 四方）を覆う。地形の中心が UV (0.5, 0.5)
/// @return rgb：地面の色（Release しないこと）
ID3D11ShaderResourceView* FieldGetColorTexture(void);

/// @brief 地形の1辺の長さを取得する
/// @return 地形の1辺の長さ
float FieldGetSize(void);

/// @brief 地形のいちばん高いところの高さを取得する
/// @return いちばん高いところの y
float FieldGetMaxHeight(void);
