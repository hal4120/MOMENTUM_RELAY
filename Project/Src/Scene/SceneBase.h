#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "../Object/Common/ActorBase/ActorDrawTypeDefine.h"

class CollisionManager;
class CameraBase;
class ActorBase;
struct Vector2I;

class PostEffectBase;

class SceneBase
{
public:
	// シーンのライフサイクル状態
	enum class STATE
	{
		Created,     // コンストラクタ直後
		Loaded,      // Load完了
		Initialized, // Init完了。Update / Draw可能
		Released,    // Release完了
	};

	// コンストラクタ
	SceneBase(void);
	// デストラクタ
	virtual ~SceneBase(void) = default;

	// コピー / ムーブ操作を禁止
	SceneBase(const SceneBase&) = delete;
	SceneBase& operator=(const SceneBase&) = delete;
	SceneBase(SceneBase&&) = delete;
	SceneBase& operator=(SceneBase&&) = delete;

	// 読み込み。派生先ではoverrideせずSubPreLoad / SubPostLoadを使う
	void Load(void);
	// 初期化。派生先ではoverrideせずSubPreInit / SubPostInitを使う
	void Init(void);
	// 更新。派生先ではoverrideせずSubPreUpdate / SubPostUpdateを使う
	void Update(void);
	// 描画。派生先ではoverrideせずSubDraw等を使う
	void Draw(void);
	// 解放。複数回呼ばれても安全
	void Release(void);

	// 現在のライフサイクル状態
	STATE GetState(void)const { return state; }

	// Update / Draw可能な状態か
	bool IsInitialized(void)const { return state == STATE::Initialized; }

	// カメラを取得する。カメラを使用しない場合はnullptrを返す。
	CameraBase* GetCamera(void)const { return camera; }

	// ポストエフェクトを追加
	void AddPostEffect(PostEffectBase* postEffect);

	// 指定した種類のポストエフェクトを削除
	template<class T>
	void RemovePostEffect() {
		for (auto it = postEffects.begin(); it != postEffects.end(); ++it) {
			T* effect = dynamic_cast<T*>(*it);

			if (effect == nullptr) { continue; }

			effect->Release();

			delete effect;

			postEffects.erase(it);

			return;
		}
	}

#pragma region シーンスタック設定

	/// <summary>
	/// 下にあるシーンも描画するかどうか（デフォルトはする）
	/// </summary>
	virtual bool IsLowerSceneDraw(void)const { return true; }

	/// <summary>
	/// 下にあるシーンも更新するかどうか（デフォルトはしない）
	/// </summary>
	virtual bool IsLowerSceneUpdate(void)const { return false; }

	/// <summary>
	/// シーン読み込み時にLoadingクラスを使用するかどうか。
	/// 重い通常シーンはtrue、軽いポーズ画面はfalseを返す。
	/// </summary>
	virtual bool IsUseLoadingScreen(void)const { return true; }

#pragma endregion

protected:

	// 既に生成済みのActorを追加する場合に使用する。所有権はSceneBaseへ移る
	void AddActor(ActorBase* newActor);

#pragma region 派生先の主要関数

	// 読み込み（メイン処理の前）
	virtual void SubPreLoad(void) {}
	// 読み込み（メイン処理の後）
	virtual void SubPostLoad(void) {}

	// 初期化（メイン処理の前）
	virtual void SubPreInit(void) {}
	// 初期化（メイン処理の後）
	virtual void SubPostInit(void) {}

	// 更新（Actor / Collision / Camera更新の前）
	virtual void SubPreUpdate(void) {}
	// 更新（Actor更新・当たり判定の後、Camera更新の前）
	virtual void SubPostUpdate(void) {}

	// 描画（メイン処理の前）
	virtual void SubPreDraw(void) {}
	// 描画（メイン処理の後）
	virtual void SubPostDraw(void) {}
	// 半透明描画
	virtual void SubAlphaDraw(void) {}
	// UI描画
	virtual void SubUiDraw(void) {}

	// 解放（メイン処理の前）
	virtual void SubPreRelease(void) {}
	// 解放（メイン処理の後）
	virtual void SubPostRelease(void) {}

#pragma endregion

	// 当たり判定管理クラスを使用するかどうか
	virtual bool UseCollisionManager(void)const { return true; }

private:

	// 二重Load / 二重Releaseを防止する「状態」
	STATE state;

	// 当たり判定管理
	CollisionManager* collision;

	// ポストエフェクト配列
	std::vector<PostEffectBase*> postEffects;

	// ポストエフェクト用のスクリーンハンドル
	int mainScreen;
	int tempScreen[2];

	// アクター描画関数
	void ActorsDraw(const std::vector<ActorBase*>& actors, ACTOR_DRAW_TYPE drawType);

	void ActorsColliderDebugDraw(const std::vector<ActorBase*>& actors);

protected:

	// カメラ
	CameraBase* camera;

	// カメラ生成
	virtual void CreateCamera(void) { camera = nullptr; }

	// Actor格納用配列
	std::vector<ActorBase*> actors;
};