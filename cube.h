#pragma once

#include "dx11.h"

// 立方体（3D描画のサンプル。インデックスバッファを使った描画）

void CubeInitialize(void);
void CubeFinalize(void);
void CubeUpdate(void);
void CubeDraw(void);

// 水のレイトレーシングで立方体を映すために使う
XMMATRIX CubeGetWorldMatrix(void);
XMFLOAT4 CubeGetColor(void);

// カメラの球と立方体が重なるか調べる（立方体の回転を含む保守的な判定）
bool CubeIntersectsSphere(XMFLOAT3 center, float radius);
