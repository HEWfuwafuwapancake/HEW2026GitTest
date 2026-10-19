#pragma once

#include "dx11.h"

// カメラ（ワールドの中を自由に動けるカメラ）
//   マウス：見回す（感度は camera.cpp の MOUSE_SENSITIVITY）
//   W/A/S/D：前後左右に移動  Space：上昇  Ctrl：下降  Shift：速く動く
//   ←→↑↓：キーボードで見回す

void CameraInitialize(void);
void CameraFinalize(void);
void CameraUpdate(void);
void CameraDraw(void);		// ビュー行列とプロジェクション行列をシェーダーに設定する（3D描画の前に呼ぶ）

XMFLOAT3 CameraGetPosition(void);
