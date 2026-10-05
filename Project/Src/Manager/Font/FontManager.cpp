#include"FontManager.h"

#include "../../pch.h"

FontManager* FontManager::ins = nullptr;

FontManager::FontManager() :
	font()
{
}

// 初期化処理
void FontManager::Init(void)
{
	for (const char*& name : FONT_PATH_TABLE) { AddFontResourceExA(name, FR_PRIVATE, NULL); }

	// フォントを作成
	for (int i = 0; i < (int)FONT_KINDS_SIZE::Max; i++) {
		font[i] = CreateFontToHandle(FONT_NAME_TABLE[i], (int)FONT_SIZE_TABLE[i], (int)FONT_THICK_TABLE[i], FONT_TYPE_TABLE[i]);
	}
}

// 終了処理
void FontManager::Release(void)
{
	// フォントを削除
	for (int& f : font) { DeleteFontToHandle(f); }

	// ウィンドウズに一時的に保持していたフォントデータを削除
	for (const char*& name : FONT_PATH_TABLE) { RemoveFontResourceExA(name, FR_PRIVATE, NULL); }
}
