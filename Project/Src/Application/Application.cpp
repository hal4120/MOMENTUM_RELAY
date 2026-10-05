#include "Application.h"

#include "../pch.h"

#include "../Manager/FPS/FPS.h"
#include "../Manager/TimeScale/TimeScale.h"
#include "../Manager/Input/InputManager.h"
#include "../Manager/Sound/SoundManager.h"
#include "../Manager/Font/FontManager.h"
#include "../Manager/Shader/ShaderResourceManager.h"
#include "../Scene/SceneManager.h"

Application* Application::ins = nullptr;

// コンストラクタ
Application::Application(void) :
	isInitFail(false),
	isReleaseFail(false),

	gameEnd(false),

	fps(nullptr),

	drawDebug(false)
{
}

// デストラクタ
Application::~Application(void)
{
}

// 初期化
void Application::Init(void)
{
	// アプリケーションの初期設定
	SetWindowText("");
	SetWindowIconID(123);

	// ウィンドウ関連
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);	// サイズ変更
	ChangeWindowMode(false);
#ifdef _DEBUG
	ChangeWindowMode(true);
#endif // _DEBUG

	// ネットワーク動作テストの為、多重起動を可能な仕様に設定しておく
	SetDoubleStartValidFlag(true);

	SetAlwaysRunFlag(true);

	// DxLibの初期化
	isInitFail = false;
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	if (DxLib_Init() == -1) { isInitFail = true; return; }

	if (Effekseer_Init(EFFECT_MAX_NUM) == -1) { isInitFail = true; return; }
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// 描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);

	// キー制御初期化
	SetUseDirectInputFlag(true);

	// FPS初期化
	fps = new FPS;
	fps->Init();

	// 時間倍率管理クラスの生成
	TimeScale::CreateIns();

	// 入力管理クラスの生成 / 初期化処理
	Input::CreateIns();

	// サウンド管理クラスの生成 / 初期化処理
	Snd::CreateIns();

	// フォントデータ生成 / 初期化処理
	Font::CreateIns();

	// シェーダーデータ生成
	ShaderResourceManager::CreateIns();

	// シーン管理初期化 / 初期化処理
	SceneManager::CreateIns();
}

// ゲームループ
void Application::Run(void)
{
	// ゲームループ
	while (ProcessMessage() == 0 && !gameEnd)
	{
		// フレームレート上限まで経過していないなら再ループさせる
		if (!fps->UpdateFrameRate()) { continue; }

		// 時間倍率管理クラスの更新
		TimeScale::GetIns().Update();

		// 入力管理クラスの更新
		Input::GetIns().Update();

		// シーン管理更新
		SceneManager::GetIns().Update();

		// 音声管理クラスの更新
		Snd::GetIns().Update();

		// デバッグ表示切替
		if (Input::GetIns().GetInfo(KEY_TYPE::DebugDrawSwitch).down) { DrawDebugSwitch(); }

		// フレームレート計算
		fps->CalcFrameRate();

		// 背面描画画面をクリア
		ClearDrawScreen();

		// シーン管理描画
		SceneManager::GetIns().Draw();

#ifdef _DEBUG
		// フレームレートデバッグ描画
		fps->DrawFrameRate();
#endif // DEBUG

		// 描画が完了した背面画面を表に持ってくる
		ScreenFlip();
	}
}

// 解放
void Application::Release(void)
{
	// シーン管理解放・削除	
	SceneManager::DeleteIns();

	// シェーダーデータの削除
	ShaderResourceManager::DeleteIns();

	// フォントデータの削除
	Font::DeleteIns();

	// サウンド管理解放・削除
	Snd::DeleteIns();

	// 入力制御削除
	Input::DeleteIns();

	// 時間倍率管理クラス削除
	TimeScale::DeleteIns();

	// フレームレート解放
	delete fps;

	// エフェクト終了
	Effkseer_End();
	// DxLib終了
	if (DxLib_End() == -1) { isReleaseFail = true; }
}
