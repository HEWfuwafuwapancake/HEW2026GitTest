#include "dx11.h"
#include "shader.h"
#include "field.h"


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ

static constexpr float FIELD_SIZE = 20.0f;	// 1辺の長さ


void FieldInitialize(void)
{
	const float half = FIELD_SIZE * 0.5f;

	// 上から見て 左上（奥）→右上→左下（手前）→右下 の順（時計回り）
	VERTEX_3D v[4];
	v[0].Position = { -half, 0.0f,  half };
	v[1].Position = {  half, 0.0f,  half };
	v[2].Position = { -half, 0.0f, -half };
	v[3].Position = {  half, 0.0f, -half };

	v[0].TexCoord = { 0.0f, 0.0f };
	v[1].TexCoord = { 1.0f, 0.0f };
	v[2].TexCoord = { 0.0f, 1.0f };
	v[3].TexCoord = { 1.0f, 1.0f };

	for (int i = 0; i < 4; i++) {
		v[i].Normal = { 0.0f, 1.0f, 0.0f };		// 上向き
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
}

void FieldFinalize(void)
{
	SAFE_RELEASE(g_pVertexBuffer);
}

void FieldUpdate(void)
{

}

void FieldDraw(void)
{
	// ワールド行列（原点に置いたまま）
	Shader_SetWorldMatrix(XMMatrixIdentity());

	// マテリアル（緑）
	MATERIAL material{};
	material.Diffuse = { 0.3f, 0.7f, 0.3f, 1.0f };
	material.Ambient = { 0.3f, 0.7f, 0.3f, 1.0f };
	Shader_SetMaterial(material);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	DirectXGetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// プリミティブトポロジの設定
	DirectXGetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴン描画命令発行
	DirectXGetDeviceContext()->Draw(4, 0);
}
