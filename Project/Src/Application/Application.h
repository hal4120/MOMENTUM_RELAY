#pragma once

// クラスの前方宣言
class FPS;

class Application
{
public:
	// スクリーンサイズ
	static constexpr int SCREEN_SIZE_X = 1440;	// スクリーン横幅;
	static constexpr int SCREEN_SIZE_Y = 810;	// スクリーン縦幅;

	// スクリーンの真ん中
	static constexpr int SCREEN_SIZE_X_HALF = SCREEN_SIZE_X / 2;
	static constexpr int SCREEN_SIZE_Y_HALF = SCREEN_SIZE_Y / 2;

public:
	// シングルトン（生成・取得・削除）
	static void CreateIns(void) { if (ins == nullptr) { ins = new Application(); ins->Init(); } }
	static Application& GetIns(void) { return *ins; }
	static void DeleteIns(void) { if (ins != nullptr) delete ins; ins = nullptr; }

public:

	void Init(void);	// 初期化
	void Run(void);		// ゲームループの開始
	void Release(void);	// 解放

	// 初期化成功／失敗の判定
	bool IsInitFail(void) const { return isInitFail; };

	// 解放成功／失敗の判定
	bool IsReleaseFail(void) const { return isReleaseFail; };

	// ゲーム終了
	void GameEnd(void) { gameEnd = true; }

	// デバッグ描画（true = 「描画する」、false = 「描画しない」）
	bool IsDrawDebug(void)const { return drawDebug; }
	// デバッグ描画の設定（true = 「描画する」に設定、false = 「描画しない」に設定）
	void SetDrawDebug(bool flg) { drawDebug = flg; }
	// デバッグ描画の切り替え（現在の逆に切り替える）
	void DrawDebugSwitch(void) { drawDebug = !drawDebug; }


private:
	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	Application(void);

	// デストラクタも同様
	~Application(void);

	// コピー・ムーブ操作を禁止
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(Application&&) = delete;

	// 下記をコンパイルエラーさせるため 上記を追加
	// Application copy = *Application::GetInstance();
	// Application copied(*Application::GetInstance());
	// Application moved = std::move(*Application::GetInstance());
private:

	// 静的インスタンス
	static Application* ins;

	// 初期化失敗
	bool isInitFail;

	// 解放失敗
	bool isReleaseFail;

	// ゲーム終了
	bool gameEnd;

	// FPS
	FPS* fps;

	// デバッグ描画（true = 「描画する」、false = 「描画しない」）
	bool drawDebug;
};

// Applicationの省略形
using App = Application; 