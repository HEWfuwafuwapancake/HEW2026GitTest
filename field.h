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
// ※ 水のシェーダー（shaderPixelWater.hlsl の TerrainHeight）にも同じ式がある。変えるときは両方そろえること
float FieldGetHeight(float x, float z);

// 高さに応じた地面の色（砂浜 → 草 → 濃い草）
// ※ shaderPixelWater.hlsl の TerrainColor と同じ式
XMFLOAT3 FieldGetColor(float height);
