#pragma once

#include <cstddef>

#include <deque>

#include <memory>

#include <vector>

#include "Common/Fade/FadeDefine.h"

class SceneBase;

class FadeSceneBase;

class SceneManager
{
private:

	// コンストラクタ
	SceneManager(void);
	// デストラクタ
	~SceneManager(void);

	// インスタンス
	static SceneManager* ins;

	// コピー / ムーブ操作を禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:

#pragma region シングルトン定義
	// 生成 / 初期化処理
	static void CreateIns(void) { if (ins == nullptr) { ins = new SceneManager(); ins->Init(); } };
	// 取得
	static SceneManager& GetIns(void) { return *ins; };
	// 終了処理 / 削除
	static void DeleteIns(void) { if (ins != nullptr) { ins->Release(); delete ins; ins = nullptr; } }
#pragma endregion

	// メインシーンの列挙型
	enum class SCENE_ID
	{
		Title,
		Game,
		GameClear,
		GameOver,
	};

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

#pragma region シーン遷移要求

	// 末尾のシーンだけを入れ替える
	void ChangeScene(std::unique_ptr<SceneBase> scene);
	// 末尾のシーンだけを入れ替える
	void ChangeScene(SCENE_ID scene);

	// 末尾のシーンをフェード付きで入れ替える
	void ChangeSceneFade(std::unique_ptr<SceneBase> scene, FADE_TYPE fadeType = FADE_TYPE::Default, unsigned short FADE_TIME = 30, unsigned int FADE_OUT_COLOR = 0x000000, unsigned int FADE_IN_COLOR = 0x000000);
	// 末尾のシーンをフェード付きで入れ替える
	void ChangeSceneFade(SCENE_ID scene, FADE_TYPE fadeType = FADE_TYPE::Default, unsigned short FADE_TIME = 30, unsigned int FADE_OUT_COLOR = 0x000000, unsigned int FADE_IN_COLOR = 0x000000);

	// 現在のシーンを残したまま、新しいシーンを末尾へ積む
	void PushScene(std::unique_ptr<SceneBase> scene);

	// 末尾から指定数のシーンを取り除く
	void PopScene(std::size_t popNum = 1);

	// 現在のスタックをすべて破棄して、指定シーンだけにする
	void JumpScene(std::unique_ptr<SceneBase> scene);
	// 現在のスタックをすべて破棄して、指定シーンだけにする
	void JumpScene(SCENE_ID scene);

	// 現在のスタックをフェード付きですべて破棄して、指定シーンだけにする
	void JumpSceneFade(std::unique_ptr<SceneBase> scene, FADE_TYPE fadeType = FADE_TYPE::Default, unsigned short FADE_TIME = 30, unsigned int FADE_OUT_COLOR = 0x000000, unsigned int FADE_IN_COLOR = 0x000000);
	// 現在のスタックをフェード付きですべて破棄して、指定シーンだけにする
	void JumpSceneFade(SCENE_ID scene, FADE_TYPE fadeType = FADE_TYPE::Default, unsigned short FADE_TIME = 30, unsigned int FADE_OUT_COLOR = 0x000000, unsigned int FADE_IN_COLOR = 0x000000);

	// 遷移、ロード、フェードのいずれかが進行中か
	bool IsTransition(void)const {
		return !requests.empty() || loadingScene != nullptr || fade != nullptr || isWaitFadeOut;
	}

#pragma endregion

#pragma region シーンをまたいでの保持が必要な情報の操作

#pragma endregion


private:

	// 予約される操作の種類
	enum class REQUEST_TYPE
	{
		// 現在のシーンを破棄して新しいシーンに入れ替える
		Change,

		// 現在のシーンを破棄せずに新しいシーンを積む
		Push,

		// 現在のシーンを破棄
		Pop,

		// 現在のシーンを全て破棄して新しいシーン1つにする
		Jump,

		// 現在のシーンを破棄して新しいシーンに入れ替える（フェード付き）
		ChangeFade,

		// 現在のシーンを全て破棄して新しいシーン1つにする（フェード付き）
		JumpFade,

	};

	// 読み込み完了後にどのようにスタックへ反映するか
	enum class LOAD_COMMIT
	{
		// 未設定
		None,

		// 現在のシーンを破棄して新しいシーンに入れ替える
		Change,

		// 現在のシーンを破棄せずに新しいシーンを積む
		Push,

		// 現在のシーンを全て破棄して新しいシーン1つにする
		Jump,

		// 現在のシーンを破棄して新しいシーンに入れ替える（フェード付き）
		ChangeFade,

		// 現在のシーンを全て破棄して新しいシーン1つにする（フェード付き）
		JumpFade,
	};

	// 1件分のシーン操作要求
	struct SceneRequest
	{
		REQUEST_TYPE type = REQUEST_TYPE::Change;
		std::unique_ptr<SceneBase> scene = nullptr;
		std::size_t popNum = 1;

		// フェード付き遷移で使用する設定
		FADE_TYPE fadeType = FADE_TYPE::Default;
		unsigned short fadeTime = 30;
		unsigned int fadeOutColor = 0x000000;
		unsigned int fadeInColor = 0x000000;
	};

	// 初期化
	void Init(void);

	// 解放
	void Release(void);

	// 3D描画関連の初期設定
	void Init3D(void);

	// SCENE_IDから実体を生成
	std::unique_ptr<SceneBase> CreateScene(SCENE_ID scene)const;

	// FADE_TYPEから対応するフェードシーンを生成
	std::unique_ptr<FadeSceneBase> CreateFadeScene(FADE_TYPE fadeType, unsigned short FADE_TIME, unsigned int FADE_OUT_COLOR, unsigned int FADE_IN_COLOR)const;

	// 遷移要求を登録
	void AddRequest(SceneRequest request);

	// 更新終了後に予約を1件適用
	void ApplyRequest(void);

	// 予約内容の実処理
	void ApplyRequest(SceneRequest request);

	// 指定シーンのLoadを開始
	void StartLoad(std::unique_ptr<SceneBase> scene, LOAD_COMMIT commit);

	// Loadingクラス更新とロード完了判定
	void UpdateLoading(void);

	// ロード済みシーンをスタックへ反映
	void CommitLoadedScene(void);

	// 実際にスタック構造を変更
	void CommitScene(std::unique_ptr<SceneBase> scene, LOAD_COMMIT commit);

	// 更新する一番下のシーン番号を調べる
	std::size_t GetFirstUpdateIndex(void)const;

	// 描画する一番下のシーン番号を調べる
	std::size_t GetFirstDrawIndex(void)const;

	// カレントカメラの更新
	void SetCurrentCamera(void)const;

	// シーンスタック
	std::vector<std::unique_ptr<SceneBase>> scenes;

	// 遷移予約キュー（現在は事故防止のため、遷移中は新しい要求を受け付けず実質1件だけ保持する）
	std::deque<SceneRequest> requests;

	// 現在読み込み中のシーン
	std::unique_ptr<SceneBase> loadingScene;

	// 読み込み完了後の反映方法
	LOAD_COMMIT loadCommit;

	// 現在使用中のフェードシーン。
	// 通常のシーンスタックとは別に最前面で更新・描画する。
	std::unique_ptr<FadeSceneBase> fade;

	// フェードアウト完了後に読み込むシーン
	std::unique_ptr<SceneBase> fadeScene;

	// フェードアウト完了待ちか
	bool isWaitFadeOut;

	// フェード後の反映方法
	LOAD_COMMIT fadeCommit;

	// 追加ライトハンドル
	int fillLight;
	int rimLight;

#pragma region シーンをまたいでの保持が必要な情報をシーンマネージャーで一元管理する

#pragma endregion

};

using SCENE_ID = SceneManager::SCENE_ID;

