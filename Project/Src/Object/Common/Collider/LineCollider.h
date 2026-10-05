#pragma once
#include"ColliderBase.h"

class LineCollider : public ColliderBase
{
public:
	/// <summary>
	/// コンストラクト
	/// </summary>
	/// <param name="type">当たり判定タイプ</param>
	/// <param name="localStartPos">線分の開始点</param>
	/// <param name="localEndPos">線分の終了点</param>
	/// <param name="pos">相対座標（引数省略で{0.0f,0.0f,0.0f}）</param>
	LineCollider(
		COLLIDER_TAG tag,
		const Vector3& startPos,
		const Vector3& endPos,
		const Vector3& pos = Vector3(),
		const Quaternion& rotation = Quaternion()
	) :
		ColliderBase(tag, pos, rotation),
		startPos(startPos),
		endPos(endPos)
	{
		SetShape(COLLIDER_SHAPE::Line);
	}
	~LineCollider()override {}

	// 線分の長さ
	float GetLen(void)const { return (startPos - endPos).Length(); }

	// 線分の半分の長さ
	float GetHalfLen(void)const { return (startPos - endPos).Length() / 2; }

	// 線分の始点
	Vector3 GetStartPos(void)const { return GetPos() + VTrans(startPos); }
	// 線分の終点
	Vector3 GetEndPos(void)const { return GetPos() + VTrans(endPos); }

	// 押し出しの方向
	Vector3 GetDirection(void)const { return GetStartPos() - GetEndPos(); }

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

	Vector3 ClosestPointAABB(const Vector3& bmin, const Vector3& bmax) const {
		// AABB 内部の基準点として clamp する
		Vector3 p = ClosestPoint(
			Vector3(
				std::clamp(GetPos().x, bmin.x, bmax.x),
				std::clamp(GetPos().y, bmin.y, bmax.y),
				std::clamp(GetPos().z, bmin.z, bmax.z)
			)
		);

		// さらに AABB 側の clamp
		return {
			std::clamp(p.x, bmin.x, bmax.x),
			std::clamp(p.y, bmin.y, bmax.y),
			std::clamp(p.z, bmin.z, bmax.z)
		};
	}

	AABB GetAABB(void) const override {
		Vector3 s = GetStartPos();
		Vector3 e = GetEndPos();

		return {
			{
				(std::min)(s.x, e.x),
				(std::min)(s.y, e.y),
				(std::min)(s.z, e.z)
			},
			{
				(std::max)(s.x, e.x),
				(std::max)(s.y, e.y),
				(std::max)(s.z, e.z)
			}
		};
	}

	void DrawDebug(unsigned int color = 0xffffff)override {
		DrawLine3D(GetStartPos().ToVECTOR(), GetEndPos().ToVECTOR(), color);
	}
private:
	// 線分の開始点
	Vector3 startPos;
	// 線分の終了点
	Vector3 endPos;
};