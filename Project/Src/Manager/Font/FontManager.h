#pragma once

class FontManager
{
private:

	// コンストラクタ/デストラクタ
	FontManager();
	~FontManager() = default;

	// コピー・ムーブ操作を禁止
	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;
	FontManager(FontManager&&) = delete;
	FontManager& operator=(FontManager&&) = delete;

	static FontManager* ins;

public:
#pragma region シングルトン定義
	// 生成/初期化処理
	static void CreateIns(void) { if (ins == nullptr) { ins = new FontManager(); ins->Init(); } }
	// 取得
	static FontManager& GetIns(void) { return *ins; }
	// 終了処理/削除
	static void DeleteIns(void) { if (ins != nullptr) { ins->Release(); delete ins; ins = nullptr; } }
#pragma endregion

	enum class FONT_KINDS_SIZE
	{
		Default20,
		Default45,
		Default64,

		Gokusyou32,
		Gokusyou60,
		Gokusyou64,
		Gokusyou80,
		Gokusyou110,

		Marumiya40,
		Marumiya50,
		Marumiya80,

		HitUi32,
		HitUi64,

		DelasukoGothic45,
		DelasukoGothic64,

		Max
	};

	// フォント取得
	const int& GetFont(FONT_KINDS_SIZE kinds)const { return font[(int)kinds]; }

	// サイズテーブル
	const unsigned char FONT_SIZE_TABLE[(int)FONT_KINDS_SIZE::Max] =
	{
		20,		// Default20
		45,		// Default45
		64,		// Default64

		32,		// Gokusyou32
		60,		// Gokusyou60
		64,		// Gokusyou64
		80,		// Gokusyou80
		110,	// Gokusyou110

		40,		// Marumiya40
		50,		// Marumiya50
		80,		// Marumiya80

		32,		// HitUi32
		64,		// HitUi64

		45,		// DelasukoGothic45
		64,		// DelasukoGothic64
	};

private:
	// 初期化処理
	void Init(void);
	// 終了処理
	void Release(void);

	// フォント
	int font[(int)FONT_KINDS_SIZE::Max];

	// フォント名テーブル
	const char* FONT_NAME_TABLE[(int)FONT_KINDS_SIZE::Max] =
	{
		nullptr,				// Default20
		nullptr,				// Default45
		nullptr,				// Default64

		"x10y12pxDonguriDuel",	// Gokusyou32
		"x10y12pxDonguriDuel",	// Gokusyou60
		"x10y12pxDonguriDuel",	// Gokusyou64
		"x10y12pxDonguriDuel",	// Gokusyou80
		"x10y12pxDonguriDuel",	// Gokusyou110

		"x12y12pxMaruMinya",	// Marumiya40
		"x12y12pxMaruMinya",	// Marumiya50
		"x12y12pxMaruMinya",	// Marumiya80

		"KKM-アナログテレビフォントv2",	// HitUi32
		"KKM-アナログテレビフォントv2",	// HitUi64

		"DelaSukoGothicOne",	// DelasukoGothic45
		"DelaSukoGothicOne",	// DelasukoGothic64
	};

	// フォントの太さテーブル
	const int FONT_THICK_TABLE[(int)FONT_KINDS_SIZE::Max] =
	{
		-1,	// Default20
		-1,	// Default45
		-1,	// Default64

		-1,	// Gokusyou32
		-1,	// Gokusyou60
		-1,	// Gokusyou64
		-1,	// Gokusyou80
		-1,	// Gokusyou110

		-1,	// Marumiya40
		-1,	// Marumiya50
		20,	// Marumiya80

		-1, // HitUi32
		-1,	// HitUi64

		-1,	// DelasukoGothic45
		-1,	// DelasukoGothic64
	};

#pragma region フォントのタイプ一覧
	// 0x00	-> ノーマルフォント
	// 0x01	-> エッジつきフォント
	// 0x02	-> アンチエイリアスフォント( 標準機能アンチエイリアス )
	// 0x12	-> アンチエイリアスフォント( 4x4サンプリング )
	// 0x22	-> アンチエイリアスフォント( 8x8サンプリング )
	// 0x32	-> アンチエイリアスフォント( 16x16サンプリング )
	// 0x03	-> アンチエイリアス＆エッジ付きフォント( 標準機能アンチエイリアス )
	// 0x13	-> アンチエイリアス＆エッジ付きフォント( 4x4サンプリング )
	// 0x23	-> アンチエイリアス＆エッジ付きフォント( 8x8サンプリング )
	// 0x33	-> アンチエイリアス＆エッジ付きフォント( 16x16サンプリング )
#pragma endregion

	// フォントタイプテーブル
	const int FONT_TYPE_TABLE[(int)FONT_KINDS_SIZE::Max] =
	{
		0x01,	// Default20
		0x01,	// Default45
		0x01,	// Default64

		0x01,	// Gokusyou32
		0x01,	// Gokusyou60
		0x01,	// Gokusyou64
		0x01,	// Gokusyou80
		0x01,	// Gokusyou110

		0x01,	// Marumiya40
		0x01,	// Marumiya50
		0x01,	// Marumiya80

		0x01,	// HitUi32
		0x01,	// HitUi64

		0x02,	// DelasukoGothic45
		0x02,	// DelasukoGothic64
	};


	// フォントをウィンドウズに一時保持するためのフォントデータパス
	const char* FONT_PATH_TABLE[4] =
	{
		"Data/Font/x10y12pxDonguriDuel.ttf",
		"Data/Font/x12y12pxMaruMinya.ttf",
		"Data/Font/DamageUi.ttf",
		"Data/Font/DelaSukoGothicOne.ttf",
	};

};

using Font = FontManager;
using FontKinds = Font::FONT_KINDS_SIZE;