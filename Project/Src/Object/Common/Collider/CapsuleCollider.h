#pragma once

#include"ColliderBase.h"

class CapsuleCollider : public ColliderBase
{
public:
	CapsuleCollider(
		COLLIDER_TAG tag,
		const Vector3& startPos,
		const Vector3& endPos,
		float radius,
		const Vector3& pos = Vector3(),
		const Quaternion& rotation = Quaternion()
	) :
		ColliderBase(tag, pos, rotation),
		startPos(startPos),
		endPos(endPos),
		radius(radius)
	{
		SetShape(COLLIDER_SHAPE::Capsule);
	}
	~CapsuleCollider()override {}

#pragma region 各ゲット関数
	// 線分の長さ
	float GetLen(void)const { return (startPos - endPos).Length(); }
	// 線分の半分の長さ
	float GetHalfLen(void)const { return (startPos - endPos).Length() / 2; }
	// 半径
	float GetRadius(void)const { return radius; }

	// カプセル線分の始点
	Vector3 GetStartPos(void)const { return GetPos() + VTrans(startPos); }
	// カプセル線分の終点
	Vector3 GetEndPos(void)const { return GetPos() + VTrans(endPos); }

	// 指定した座標から線分の中で一番近い座標を取得する
	Vector3 ClosestPoint(const Vector3& point) const {
		// 始点と終点
		Vector3 s = GetStartPos();
		Vector3 e = GetEndPos();

		Vector3 se = e - s;
		Vector3 sp = point - s;

		float lenSq = se.LengthSq();
		if (lenSq < 1e-6f) {
			// 始点 = 終点 の場合、始点を返す
			return s;
		}

		// 点を線分に射影するパラメータt
		float t = sp.Dot(se) / lenSq;

		// 線分範囲に clamp
		t = std::clamp(t, 0.0f, 1.0f);

		// 最近点
		return s + se * t;
	}

	AABB GetAABB(void) const override {
		Vector3 s = GetStartPos();
		Vector3 e = GetEndPos();

		return AABB(
			Vector3(
				(std::min)(s.x, e.x) - radius,
				(std::min)(s.y, e.y) - radius,
				(std::min)(s.z, e.z) - radius
			),
			Vector3(
				(std::max)(s.x, e.x) + radius,
				(std::max)(s.y, e.y) + radius,
				(std::max)(s.z, e.z) + radius
			)
		);
	}
#pragma endregion

	void DrawDebug(unsigned int color = 0xffffff)override {
		DrawCapsule3D(GetStartPos().ToVECTOR(), GetEndPos().ToVECTOR(), radius, 12, color, color, true);
	}
private:

	// 線分の開始点
	Vector3 startPos;
	// 線分の終了点
	Vector3 endPos;
	
	// 半径
	float radius;
};