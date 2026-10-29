#pragma once

#include "../../Common/ActorBase/ActorBase.h"

class SphereCollider;

class PlayerMomentumQuickStealFind : public ActorBase
{
public:

	PlayerMomentumQuickStealFind(Transform& playerTrans);

	~PlayerMomentumQuickStealFind()override = default;

	// ロード
	void Load(void)override;

	// 当たり判定コールバック
	void OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result);

	// 探索開始
	void Find(void);

	// 探索成功判定
	ActorBase* GetTarget(void) { return target; }

private:

#pragma region 参照

	// プレイヤーのモデル制御情報の参照
	Transform& playerTrans;

	// 現在ターゲット
	ActorBase* target;

	// 当たり判定管理
	SphereCollider* collRef;

#pragma endregion

	// 現在ターゲットのプレイヤーまでの距離
	float targetDistance;

	// 追加更新
	void SubUpdate(void)override;
	// 追加解放
	void SubRelease(void)override { collRef = nullptr; }
};