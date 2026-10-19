#pragma once

// キーボード入力
// キーは仮想キーコードで指定する（例：VK_LEFT、VK_SPACE、'W'（英字は大文字））

void KeyboardUpdate(void);				// 毎フレーム、Update の最初に呼ぶ
bool KeyboardIsPress(int key);			// 押している間ずっと true
bool KeyboardIsTrigger(int key);		// 押した瞬間の1フレームだけ true
bool KeyboardIsRelease(int key);		// 離した瞬間の1フレームだけ true
