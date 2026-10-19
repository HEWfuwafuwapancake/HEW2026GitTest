#pragma once

#include "dx11.h"

// カメラ（注視点のまわりを回るカメラ）
//   ←→：左右に回る  ↑↓：上下に回る  W/S：近づく・離れる

void CameraInitialize(void);
void CameraFinalize(void);
void CameraUpdate(void);
void CameraDraw(void);		// ビュー行列とプロジェクション行列をシェーダーに設定する（3D描画の前に呼ぶ）

XMFLOAT3 CameraGetPosition(void);
void CameraSetTarget(const XMFLOAT3& target);	// 注視点（カメラが見る場所）を変える
