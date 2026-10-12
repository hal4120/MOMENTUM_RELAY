#include "PlayerAttackCollOperator.h"

#include "../../Common/Collider/SphereCollider.h"

PlayerAttackCollOperator::PlayerAttackCollOperator(
	float COLL_RADIUS,
	const Vector3& COLL_LOCAL_POS,

	const Transform& playerTrans
) :
	COLL_LOCAL_POS(COLL_LOCAL_POS),

	playerTrans(playerTrans),

	attackColl(new SphereCollider(COLLIDER_TAG::PlayerKickDownAttack, COLL_RADIUS))
{
}

void PlayerAttackCollOperator::Load(void)
{
#pragma region オブジェクト設定

	// 動的オブジェクトとしての処理を有効にする
	SetDynamicFlg(true);

	// 重力を無効にする
	SetGravityFlg(false);

	// 当たり判定による押し出しを無効にする
	SetPushFlg(false);

#pragma endregion

	attackColl->SetJudgeFlg(false);

	// コライダーを追加
	AddCollider(attackColl);
}

void PlayerAttackCollOperator::SubUpdate(void)
{
	// 判定が発生しているときだけ処理する
	if (!attackColl->GetJudgeFlg()) { return; }

	// 追従し続ける
	trans.pos = playerTrans.pos + playerTrans.VTrans(COLL_LOCAL_POS);
}

void PlayerAttackCollOperator::On(void)
{
	attackColl->SetJudgeFlg(true);

	// プレイヤーの座標、プレイヤーの角度、攻撃判定の相対座標をつかい
	// 判定の座標を割り出す
	trans.pos = playerTrans.pos + playerTrans.VTrans(COLL_LOCAL_POS);
}

void PlayerAttackCollOperator::Off(void)
{
	attackColl->SetJudgeFlg(false);
}