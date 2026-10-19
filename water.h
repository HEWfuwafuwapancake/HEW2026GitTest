#pragma once

// 湖の水面（色はレイトレーシングで決める。shaderPixelWater.hlsl）

void WaterInitialize(void);
void WaterFinalize(void);
void WaterUpdate(void);
void WaterDraw(void);		// 地形や立方体など、ほかの3Dを描いたあとに呼ぶ
