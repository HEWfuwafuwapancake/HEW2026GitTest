#pragma once // インクルードガード

#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;



#define SAFE_RELEASE(o)      do { if (o) { (o)->Release(); (o) = NULL; } } while (0)

bool DirectXInitialize(HWND hWnd);
void DirectXFinalize(void);

ID3D11Device* DirectXGetDevice(void);
ID3D11DeviceContext* DirectXGetDeviceContext(void);

void SetRenderTarget(void);

// 深度テストの切り替え（3D描画は true、2D（UI）描画は false）
void SetDepthEnable(bool enable);

void Clear(void);
void Present(void);

#define SCREEN_WIDTH	(1280)		// ウインドウの幅
#define SCREEN_HEIGHT	(720)		// ウインドウの高さ

struct VERTEX_3D
{
	XMFLOAT3 Position;	// 頂点座標
	XMFLOAT3 Normal;	// 法線ベクトル（ライトの計算に使う）
	XMFLOAT4 Diffuse;	// 頂点カラー
	XMFLOAT2 TexCoord;	// UV座標
};
