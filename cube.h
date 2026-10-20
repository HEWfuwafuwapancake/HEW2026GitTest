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

// カメラの当たり判定に使う（立方体の回転と大きさを含めて、箱の形そのもので判定する）
XMFLOAT3 CubeGetPosition(void);
bool CubeOverlapsSphere(const XMFLOAT3& center, float radius);	// 球（中心 center、半径 radius）が立方体と重なっているか
bool CubePushOutSphere(XMFLOAT3& center, float radius);		// 重なっていたら center を立方体の外へ押し出す。押し出したら true
