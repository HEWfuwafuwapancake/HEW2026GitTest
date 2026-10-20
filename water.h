#pragma once

/// @file water.h
/// @brief 湖の水面（色はレイトレーシングで決める。shaderPixelWater.hlsl）

/// @brief 水面の頂点バッファを作る
void WaterInitialize(void);

/// @brief 水面の頂点バッファを解放する
void WaterFinalize(void);

/// @brief 経過時間を進める（波を動かす。毎フレーム呼ぶ）
void WaterUpdate(void);

/// @brief 水面を描く（地形や立方体など、ほかの3Dを描いたあとに呼ぶ）
void WaterDraw(void);
