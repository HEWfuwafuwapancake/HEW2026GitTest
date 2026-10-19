#include "camera.h"
#include "shader.h"
#include "keyboard.h"
#include "mouse.h"
#include "field.h"


//------------------------------------------------------------------------------
//	調整用の値
//------------------------------------------------------------------------------
// マウス感度：マウスを1カウント動かしたときに回る角度（ラジアン）
// 大きくすると速く回る。0.001f（ゆっくり）〜 0.005f（速い）くらいが目安
static constexpr float MOUSE_SENSITIVITY = 0.0025f;
static constexpr bool  MOUSE_INVERT_Y = false;							// true にすると上下が逆になる

static constexpr float KEY_ROTATE_SPEED = XMConvertToRadians(2.0f);	// 矢印キーで1フレームに回る角度
static constexpr float MOVE_SPEED = 0.15f;							// 1フレームに進む距離
static constexpr float RUN_MULTIPLIER = 3.0f;						// Shift を押したときの速さの倍率
static constexpr float EYE_MIN_HEIGHT = 0.5f;						// 地面・水面からこれ以上は下がらない
static constexpr float WORLD_LIMIT = 48.0f;							// ワールドの端（これより外へは行けない）

static constexpr float FOV = XMConvertToRadians(60.0f);				// 視野角（縦）
static constexpr float NEAR_Z = 0.1f;								// これより近いものは描かない
static constexpr float FAR_Z = 1000.0f;								// これより遠いものは描かない


static XMFLOAT3 g_Position;		// カメラの位置
static float g_Yaw;				// 左右の向き（ラジアン。0 で +Z 方向を見る）
static float g_Pitch;			// 上下の向き（ラジアン。プラスで上を見る）


void CameraInitialize(void)
{
	g_Position = { 0.0f, 4.0f, -18.0f };
	g_Yaw = 0.0f;
	g_Pitch = XMConvertToRadians(-12.0f);
}

void CameraFinalize(void)
{

}

void CameraUpdate(void)
{
	// ---- 向き ----
	// マウス
	if (MouseIsLocked()) {
		float dy = (float)MouseGetDeltaY() * (MOUSE_INVERT_Y ? -1.0f : 1.0f);
		g_Yaw += (float)MouseGetDeltaX() * MOUSE_SENSITIVITY;
		g_Pitch -= dy * MOUSE_SENSITIVITY;
	}

	// 矢印キー
	if (KeyboardIsPress(VK_LEFT))  g_Yaw -= KEY_ROTATE_SPEED;
	if (KeyboardIsPress(VK_RIGHT)) g_Yaw += KEY_ROTATE_SPEED;
	if (KeyboardIsPress(VK_UP))    g_Pitch += KEY_ROTATE_SPEED;
	if (KeyboardIsPress(VK_DOWN))  g_Pitch -= KEY_ROTATE_SPEED;

	// 真上・真下を越えると上下が反転するので止める
	const float PITCH_LIMIT = XMConvertToRadians(89.0f);
	if (g_Pitch > PITCH_LIMIT)  g_Pitch = PITCH_LIMIT;
	if (g_Pitch < -PITCH_LIMIT) g_Pitch = -PITCH_LIMIT;

	// ---- 移動 ----
	// 前と右（上下には傾けない。下を見ながら W を押しても地面にもぐらないように）
	XMFLOAT3 forward = { sinf(g_Yaw), 0.0f, cosf(g_Yaw) };
	XMFLOAT3 right = { cosf(g_Yaw), 0.0f, -sinf(g_Yaw) };

	float speed = MOVE_SPEED;
	if (KeyboardIsPress(VK_SHIFT)) speed *= RUN_MULTIPLIER;

	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };
	if (KeyboardIsPress('W')) { move.x += forward.x; move.z += forward.z; }
	if (KeyboardIsPress('S')) { move.x -= forward.x; move.z -= forward.z; }
	if (KeyboardIsPress('D')) { move.x += right.x;   move.z += right.z; }
	if (KeyboardIsPress('A')) { move.x -= right.x;   move.z -= right.z; }
	if (KeyboardIsPress(VK_SPACE))   move.y += 1.0f;
	if (KeyboardIsPress(VK_CONTROL)) move.y -= 1.0f;

	// 斜め移動が速くならないように長さを1にそろえる
	XMVECTOR m = XMLoadFloat3(&move);
	if (XMVectorGetX(XMVector3LengthSq(m)) > 0.0f) {
		m = XMVector3Normalize(m) * speed;
	}
	XMFLOAT3 delta;
	XMStoreFloat3(&delta, m);
	g_Position.x += delta.x;
	g_Position.y += delta.y;
	g_Position.z += delta.z;

	// ワールドの外に出ない
	if (g_Position.x > WORLD_LIMIT)  g_Position.x = WORLD_LIMIT;
	if (g_Position.x < -WORLD_LIMIT) g_Position.x = -WORLD_LIMIT;
	if (g_Position.z > WORLD_LIMIT)  g_Position.z = WORLD_LIMIT;
	if (g_Position.z < -WORLD_LIMIT) g_Position.z = -WORLD_LIMIT;

	// 地面と水面にもぐらない（水の中から見る描画には対応していないため）
	float ground = FieldGetHeight(g_Position.x, g_Position.z);
	if (ground < FIELD_WATER_LEVEL) ground = FIELD_WATER_LEVEL;
	if (g_Position.y < ground + EYE_MIN_HEIGHT) g_Position.y = ground + EYE_MIN_HEIGHT;
}

void CameraDraw(void)
{
	// ビュー行列（カメラの位置と、見ている向き）
	XMVECTOR eye = XMLoadFloat3(&g_Position);
	XMVECTOR dir = XMVectorSet(cosf(g_Pitch) * sinf(g_Yaw), sinf(g_Pitch), cosf(g_Pitch) * cosf(g_Yaw), 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Shader_SetViewMatrix(XMMatrixLookToLH(eye, dir, up));

	// プロジェクション行列（遠くのものほど小さく見える透視投影）
	float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	Shader_SetProjectionMatrix(XMMatrixPerspectiveFovLH(FOV, aspect, NEAR_Z, FAR_Z));
}

XMFLOAT3 CameraGetPosition(void)
{
	return g_Position;
}
