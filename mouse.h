#pragma once

#include <Windows.h>

// マウス入力（カメラを回すための移動量を取る）
// ロック中はカーソルを隠してウィンドウの中に閉じ込め、どれだけ動かしたかだけを読む

void MouseInitialize(HWND hWnd);
void MouseFinalize(void);
void MouseUpdate(void);					// 毎フレーム、Update の最初のほうで呼ぶ
void MouseOnRawInput(LPARAM lParam);	// WndProc の WM_INPUT から呼ぶ

void MouseSetLock(bool lock);			// true：カーソルを隠して閉じ込める  false：元に戻す
bool MouseIsLocked(void);

int MouseGetDeltaX(void);				// このフレームで動いた量（右がプラス）
int MouseGetDeltaY(void);				// このフレームで動いた量（下がプラス）
