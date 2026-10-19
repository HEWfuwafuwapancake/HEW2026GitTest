#include "camera.h"
#include "shader.h"
#include "keyboard.h"


static XMFLOAT3 g_Position;		// カメラの位置（CameraUpdate で計算する）
static XMFLOAT3 g_Target;		// 注視点
static float g_Yaw;				// 左右の回転角度（ラジアン）
static float g_Pitch;			// 上下の回転角度（ラジアン）
static float g_Distance;		// 注視点からの距離

static constexpr float ROTATE_SPEED = XMConvertToRadians(2.0f);	// 1フレームに回る角度
static constexpr float ZOOM_SPEED = 0.2f;						// 1フレームに近づく距離
static constexpr float FOV = XMConvertToRadians(60.0f);			// 視野角（縦）
static constexpr float NEAR_Z = 0.1f;							// これより近いものは描かない
static constexpr float FAR_Z = 1000.0f;							// これより遠いものは描かない


void CameraInitialize(void)
{
	g_Target = { 0.0f, 0.0f, 0.0f };
	g_Yaw = 0.0f;
	g_Pitch = XMConvertToRadians(25.0f);
	g_Distance = 10.0f;

	CameraUpdate();
}

void CameraFinalize(void)
{

}

void CameraUpdate(void)
{
	// キー入力で回転・ズーム
	if (KeyboardIsPress(VK_LEFT))  g_Yaw += ROTATE_SPEED;
	if (KeyboardIsPress(VK_RIGHT)) g_Yaw -= ROTATE_SPEED;
	if (KeyboardIsPress(VK_UP))    g_Pitch += ROTATE_SPEED;
	if (KeyboardIsPress(VK_DOWN))  g_Pitch -= ROTATE_SPEED;
	if (KeyboardIsPress('W'))      g_Distance -= ZOOM_SPEED;
	if (KeyboardIsPress('S'))      g_Distance += ZOOM_SPEED;

	// 真上・真下を越えると上下が反転するので止める
	const float PITCH_LIMIT = XMConvertToRadians(89.0f);
	if (g_Pitch > PITCH_LIMIT)  g_Pitch = PITCH_LIMIT;
	if (g_Pitch < -PITCH_LIMIT) g_Pitch = -PITCH_LIMIT;
	if (g_Distance < 1.0f) g_Distance = 1.0f;

	// 角度と距離から、注視点のまわりのカメラ位置を求める
	g_Position.x = g_Target.x + g_Distance * cosf(g_Pitch) * sinf(g_Yaw);
	g_Position.y = g_Target.y + g_Distance * sinf(g_Pitch);
	g_Position.z = g_Target.z - g_Distance * cosf(g_Pitch) * cosf(g_Yaw);
}

void CameraDraw(void)
{
	// ビュー行列（カメラの位置と向き）
	XMVECTOR eye = XMLoadFloat3(&g_Position);
	XMVECTOR target = XMLoadFloat3(&g_Target);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Shader_SetViewMatrix(XMMatrixLookAtLH(eye, target, up));

	// プロジェクション行列（遠くのものほど小さく見える透視投影）
	float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	Shader_SetProjectionMatrix(XMMatrixPerspectiveFovLH(FOV, aspect, NEAR_Z, FAR_Z));
}

XMFLOAT3 CameraGetPosition(void)
{
	return g_Position;
}

void CameraSetTarget(const XMFLOAT3& target)
{
	g_Target = target;
}
