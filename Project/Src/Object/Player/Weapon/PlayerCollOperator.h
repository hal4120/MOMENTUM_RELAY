#pragma once

#include "../../Common/ActorBase/ActorBase.h"

class SphereCollider;

class PlayerCollOperator : public ActorBase
{
public:

	/// <summary>
	/// プレイヤーの汎用コライダー管理
	/// </summary>
	/// <param name="COLL_TAG">コライダーのタグ</param>
	/// <param name="COLL_RADIUS">コライダーの半径</param>
	/// <param name="COLL_LOCAL_POS">コライダーの相対座標</param>
	/// <param name="playerTrans">追従対象</param>
	PlayerCollOperator(
		COLLIDER_TAG COLL_TAG,
		float COLL_RADIUS,

		const Vector3& COLL_LOCAL_POS,

		const Transform& playerTrans
	);
	~PlayerCollOperator()override = default;

	void Load(void)override;

	// 判定発生
	void On(void);
	// 判定消去
	void Off(void);

private:

	// 攻撃の判定を発生させる座標（プレイヤー座標からの相対座標）
	const Vector3 COLL_LOCAL_POS;

	// プレイヤーのモデル制御情報の参照
	const Transform& playerTrans;

	// 生成するコライダー
	SphereCollider* attackColl;

	// 更新処理
	void SubUpdate(void)override;
};