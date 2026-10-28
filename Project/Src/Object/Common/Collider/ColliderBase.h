#pragma once

#include "../../../pch.h"

#include <algorithm>
#include <functional>
#include <vector>

class ActorBase;
struct Transform;

#include "../../../Common/Vector3.h"
#include "../../../Common/Quaternion.h"

#include "ColliderTagDefine.h"

// 衝突結果構造体
struct CollisionResult
{
	// 衝突点
	Vector3 point;

	// BからAへ向かう衝突法線ベクトル
	Vector3 normal;

	// めり込み量
	float penetration;

	CollisionResult(void);
};


class ColliderBase
{
public:

	// 最小/最大 座標 構造体
	struct AABB
	{
		Vector3 min;
		Vector3 max;

		AABB(void) = default;

		AABB(const Vector3& min, const Vector3& max)
		{
			this->min = min;
			this->max = max;
		}
	};


	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="type">当たり判定タイプ</param>
	/// <param name="pos">相対座標</param>
	/// <param name="rotation">相対回転</param>
	ColliderBase(
		COLLIDER_TAG type,
		const Vector3& pos = Vector3(),
		const Quaternion& rotation = Quaternion()
	);

	virtual ~ColliderBase() = default;

	// 初期化
	virtual void Init(void) {}

	// デバッグ描画
	virtual void DrawDebug(unsigned int color = 0xffffff) = 0;


#pragma region 初期設定

	// 参照アクターをセット
	void SetActorPtr(ActorBase* ptr);

	// 当たり判定通知用関数セット
	void SetOnCollisionFunc(std::function<void(COLLIDER_TAG, const ColliderBase&, const CollisionResult&)> OnCollisionFunc);

	// 接地判定通知用関数セット
	void SetOnGroundedFunc(std::function<void(COLLIDER_TAG, const ColliderBase&, const Vector3&)> OnGroundedFunc);

#pragma endregion


#pragma region ゲット関数

	// アクター取得
	ActorBase* GetActor(void)const;

	// コライダー座標
	Vector3 GetPos(void) const;

	// 1フレーム前のコライダー座標
	Vector3 GetPrevPos(void) const;

	// コライダー回転
	Quaternion GetRotation(void) const;

	// コライダー回転行列
	MATRIX GetRotationMat(void) const;

	// コライダー回転を適用したベクトルを取得
	Vector3 VTrans(const Vector3& v) const;

	// 当たり判定フラグ
	bool GetJudgeFlg(void) const;

	// 当たり判定のタイプ
	COLLIDER_TAG GetTag(void) const;

	// 当たり判定の形状
	COLLIDER_SHAPE GetShape(void) const;

	// 判定通知の呼び出し
	void CallOnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result);

	// 接地判定通知の呼び出し
	void CallOnGrounded(COLLIDER_TAG ownTag, const ColliderBase& other, const Vector3& pushVec);

	// 自分が占有している範囲
	virtual AABB GetAABB(void) const = 0;

#pragma endregion


#pragma region セット関数

	// モデル制御情報の座標情報を書き換える
	void SetTransformPos(const Vector3& pos);
	void SetTransformPosAdd(const Vector3& vec);

	// 当たり判定フラグセット
	void SetJudgeFlg(bool flg);

#pragma endregion


private:

	// 参照アクター
	ActorBase* owner;

	// 相対座標
	Vector3 pos;

	// 相対回転
	Quaternion rotation;

	// 当たり判定フラグ
	bool judgeFlg;

	// 当たり判定タイプ
	COLLIDER_TAG tag;

	// 当たり判定形状
	COLLIDER_SHAPE shape;

	// 当たったときに呼び出す関数
	std::function<void(COLLIDER_TAG, const ColliderBase&, const CollisionResult&)> OnCollision;

	// 接地したときに呼び出す関数
	std::function<void(COLLIDER_TAG, const ColliderBase&, const Vector3&)> OnGrounded;

protected:

	void SetShape(COLLIDER_SHAPE s);
};


/// <summary>
/// 特定のコライダーを探す
/// </summary>
/// <typeparam name="T">探したいクラス</typeparam>
/// <param name="tag">タグ種類</param>
/// <returns></returns>
template<typename T = ColliderBase>
std::vector<T*> ColliderSerch(std::vector<ColliderBase*> collider, COLLIDER_TAG tag = COLLIDER_TAG::None)
{
	std::vector<T*> out;

	out.reserve(collider.size());

	for (auto c : collider) {

		if (!c) { continue; }

		if (auto* ptr = dynamic_cast<T*>(c)) {
			if (c->GetTag() == tag || tag == COLLIDER_TAG::None) { out.push_back(ptr); }
		}
	}

	return out;
}