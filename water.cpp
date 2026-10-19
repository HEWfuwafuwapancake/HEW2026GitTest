#include "dx11.h"
#include "shader.h"
#include "camera.h"
#include "cube.h"
#include "field.h"
#include "water.h"


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ

static constexpr float WATER_SIZE = 40.0f;	// 水面の1辺（湖より大きければよい。岸の外は地面に隠れる）

static float g_Time = 0.0f;	// 経過時間（秒）


void WaterInitialize(void)
{
	const float half = WATER_SIZE * 0.5f;
	const float y = FIELD_WATER_LEVEL;

	// 上から見て 左上（奥）→右上→左下（手前）→右下 の順（時計回り）
	VERTEX_3D v[4];
	v[0].Position = { -half, y,  half };
	v[1].Position = {  half, y,  half };
	v[2].Position = { -half, y, -half };
	v[3].Position = {  half, y, -half };

	v[0].TexCoord = { 0.0f, 0.0f };
	v[1].TexCoord = { 1.0f, 0.0f };
	v[2].TexCoord = { 0.0f, 1.0f };
	v[3].TexCoord = { 1.0f, 1.0f };

	for (int i = 0; i < 4; i++) {
		v[i].Normal = { 0.0f, 1.0f, 0.0f };
		v[i].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = v;
	DirectXGetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

	g_Time = 0.0f;
}

void WaterFinalize(void)
{
	SAFE_RELEASE(g_pVertexBuffer);
}

void WaterUpdate(void)
{
	g_Time += 1.0f / 60.0f;	// Update は1秒に60回呼ばれる
}

void WaterDraw(void)
{
	// 水用のシェーダーに切り替えて、レイトレーシングに使う情報を渡す
	Shader_BeginWater();
	Shader_SetWorldMatrix(XMMatrixIdentity());
	Shader_SetWater(CameraGetPosition(), g_Time, CubeGetWorldMatrix(), CubeGetColor());

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	DirectXGetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// プリミティブトポロジの設定
	DirectXGetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴン描画命令発行
	DirectXGetDeviceContext()->Draw(4, 0);

	// ふつうのシェーダーに戻す
	Shader_Begin();
}
