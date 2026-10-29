#include "PlayerMomentumQuickStealFind.h"

#include "../../Common/Collider/SphereCollider.h"

PlayerMomentumQuickStealFind::PlayerMomentumQuickStealFind(Transform& playerTrans):
	ActorBase(),

	playerTrans(playerTrans),

	target(nullptr),

	collRef(nullptr),

	targetDistance(0.0f)
{
}

void PlayerMomentumQuickStealFind::Load(void)
{
	// 動的有効
	SetDynamicFlg(true);
	// 重力無効
	SetGravityFlg(false);
	// 押し出し無効
	SetPushFlg(false);

	// コライダー生成
	collRef = new SphereCollider(COLLIDER_TAG::Player, 1000.0f);
	// コライダー追加
	AddCollider(collRef);
}

void PlayerMomentumQuickStealFind::SubUpdate(void)
{
	// 探索用のコライダーが有効なときのみ処理をする
	if (!collRef->GetJudgeFlg()) { return; }

	// 探索が成功していたら（ターゲットが設定されていたら）終了
	if (target != nullptr) {

		// 当たり判定を消す
		collRef->SetJudgeFlg(false);

		// 終了
		return;
	}

	// 座標をプレイヤーに追従
	trans.pos = playerTrans.pos + playerTrans.rotation.Rotate(Vector3::Zonly(50.0f));
}

void PlayerMomentumQuickStealFind::OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)
{
	// 新しいターゲット候補
	ActorBase* newTarget = other.GetActor();

	// 現在ターゲットが未設定の場合、設定して終了
	if (target == nullptr) {
		// ターゲット設定
		target = newTarget;

		// ターゲットとプレイヤーの距離を保存
		targetDistance = (target->GetTrans().pos - playerTrans.pos).LengthSq();
	}
	// 現在すでにターゲットが設定されていたら、距離比較
	else {
		// 新しいターゲット候補とプレイヤーとの距離
		float newTargetDistance = (newTarget->GetTrans().pos - playerTrans.pos).LengthSq();

		// 現在ターゲットのプレイヤーの距離と比較
		if (newTargetDistance < targetDistance) {

			// 新しいターゲット候補との距離の方が短ければ、ターゲット入れ替え
			target = newTarget;

			// 新しいターゲットとの距離を保存
			targetDistance = newTargetDistance;
		}
	}
}

void PlayerMomentumQuickStealFind::Find(void)
{
	// ターゲット初期化
	target = nullptr;

	// ターゲットの距離比較用の変数を初期化
	targetDistance = 0.0f;

	// 当たり判定を起動
	collRef->SetJudgeFlg(true);

	// 座標をプレイヤーに追従
	trans.pos = playerTrans.pos + playerTrans.rotation.Rotate(Vector3::Zonly(50.0f));
}