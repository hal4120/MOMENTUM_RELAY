#pragma once

#include"ColliderBase.h"

#include <array>
#include <algorithm>
#include <cmath>

class BoxCollider : public ColliderBase
{
public:
	BoxCollider(
		COLLIDER_TAG tag,
		const Vector3& size,
		const Vector3& pos = Vector3(),
		const Quaternion& rotation = Quaternion()
	) :
		ColliderBase(tag, pos, rotation),
		size(size)
	{
		SetShape(COLLIDER_SHAPE::Box);
	}
	~BoxCollider()override {}

	const Vector3& GetSize(void)const { return size; }

	// Še²‚Ì”¼ƒTƒCƒY
	Vector3 GetHalfSize(void) const { return size * 0.5f; }

	// ‰ñ“]Œã‚Ìƒ{ƒbƒNƒX‚ÌŠe²‚ğæ“¾i0:X 1:Y 2:Zj
	std::array<Vector3, 3> GetAxes(void) const{
		const MATRIX rotationMat = GetRotationMat();

		return{
			Vector3::Xonly(1.0f).TransMat(rotationMat).Normalized(),
			Vector3::Yonly(1.0f).TransMat(rotationMat).Normalized(),
			Vector3::Zonly(1.0f).TransMat(rotationMat).Normalized()
		};
	}
	// w’è‚µ‚½²‚Éƒ{ƒbƒNƒX‚ğ“Š‰e‚µ‚½‚Æ‚«‚Ì”¼Œa
	float GetProjectionRadius(const Vector3& axis) const
	{
		const Vector3 halfSize = GetHalfSize();
		const std::array<Vector3, 3> axes = GetAxes();

		return
			std::abs(axis.Dot(axes[0])) * halfSize.x +
			std::abs(axis.Dot(axes[1])) * halfSize.y +
			std::abs(axis.Dot(axes[2])) * halfSize.z;
	}

	// w’è•ûŒü‚ÉÅ‚à‰“‚¢ƒ{ƒbƒNƒXã‚Ì“_‚ğæ“¾
	Vector3 GetSupportPoint(const Vector3& direction) const {

		// ƒ{ƒbƒNƒX”¼ƒTƒCƒY‚ğæ“¾
		const Vector3 halfSize = GetHalfSize();
		// ƒ{ƒbƒNƒX‚ÌŠe²‚ğæ“¾
		const std::array<Vector3, 3> axes = GetAxes();

		// ƒ{ƒbƒNƒX‚Ì’†SÀ•W‚ğæ“¾
		Vector3 point = GetPos();

		// Še²‚Ì³•‰•ûŒü‚É‰‚¶‚ÄAƒ{ƒbƒNƒX‚Ì’¸“_‚ğ‘I‘ğ
		point += axes[0] * (direction.Dot(axes[0]) >= 0.0f ? halfSize.x : -halfSize.x);
		point += axes[1] * (direction.Dot(axes[1]) >= 0.0f ? halfSize.y : -halfSize.y);
		point += axes[2] * (direction.Dot(axes[2]) >= 0.0f ? halfSize.z : -halfSize.z);

		return point;
	}

	// ‰ñ“]Œã‚Ìƒ{ƒbƒNƒX8’¸“_‚ğæ“¾
	std::array<Vector3, 8> GetVertices(void) const {

		// ƒ{ƒbƒNƒX‚Ì’†SÀ•W‚ğæ“¾
		const Vector3 center = GetPos();
		// ƒ{ƒbƒNƒX‚Ì”¼ƒTƒCƒY‚ğæ“¾
		const Vector3 halfSize = GetHalfSize();

		// ƒ{ƒbƒNƒX‚ÌŠe²‚ğæ“¾
		const std::array<Vector3, 3> axes = GetAxes();

		// Še²‚Ì”¼ƒTƒCƒY‚ğŒvZ
		const Vector3 xAxis = axes[0] * halfSize.x;
		const Vector3 yAxis = axes[1] * halfSize.y;
		const Vector3 zAxis = axes[2] * halfSize.z;

		// 8’¸“_‚ğŒvZ‚µ‚Ä•Ô‚·
		return {
			center - xAxis - yAxis - zAxis,
			center + xAxis - yAxis - zAxis,
			center - xAxis + yAxis - zAxis,
			center + xAxis + yAxis - zAxis,

			center - xAxis - yAxis + zAxis,
			center + xAxis - yAxis + zAxis,
			center - xAxis + yAxis + zAxis,
			center + xAxis + yAxis + zAxis
		};
	}

    AABB GetAABB(void) const override {
		// ‰ñ“]Œã‚Ìƒ{ƒbƒNƒX‚Ì8’¸“_‚ğæ“¾
		const std::array<Vector3, 8> vertices = GetVertices();

		// AABB‚ÌÅ¬À•W‚ÆÅ‘åÀ•W‚ğŒvZ
		Vector3 minPos = vertices[0];
		Vector3 maxPos = vertices[0];

		// 8’¸“_‚ğƒ‹[ƒv‚µ‚ÄÅ¬À•W‚ÆÅ‘åÀ•W‚ğXV
		for (int i = 1; i < 8; ++i) {
			minPos.x = (std::min)(minPos.x, vertices[i].x);

			minPos.y = (std::min)(minPos.y, vertices[i].y);

			minPos.z = (std::min)(minPos.z, vertices[i].z);

			maxPos.x = (std::max)(maxPos.x, vertices[i].x);

			maxPos.y = (std::max)(maxPos.y, vertices[i].y);

			maxPos.z = (std::max)(maxPos.z, vertices[i].z);
		}

		// AABB‚ğ•Ô‚·
		return AABB(minPos, maxPos);
	}

	void DrawDebug(unsigned int color = 0xffffff)override {

		// ‰ñ“]Œã‚Ìƒ{ƒbƒNƒX‚Ì8’¸“_‚ğæ“¾
		const std::array<Vector3, 8> vertices = GetVertices();

		// ‘O–Ê````````````````````````````````
		DrawLine3D(vertices[0].ToVECTOR(), vertices[1].ToVECTOR(), color);

		DrawLine3D(vertices[1].ToVECTOR(), vertices[3].ToVECTOR(), color);

		DrawLine3D(vertices[3].ToVECTOR(), vertices[2].ToVECTOR(), color);

		DrawLine3D(vertices[2].ToVECTOR(), vertices[0].ToVECTOR(), color);
		// ````````````````````````````````‘O–Ê

		// ”w–Ê````````````````````````````````
		DrawLine3D(vertices[4].ToVECTOR(), vertices[5].ToVECTOR(), color);

		DrawLine3D(vertices[5].ToVECTOR(), vertices[7].ToVECTOR(), color);

		DrawLine3D(vertices[7].ToVECTOR(), vertices[6].ToVECTOR(), color);

		DrawLine3D(vertices[6].ToVECTOR(), vertices[4].ToVECTOR(), color);
		// ````````````````````````````````”w–Ê

		// ‘O–Ê‚Æ”w–Ê‚ğÚ‘±``````````````````````````
		DrawLine3D(vertices[0].ToVECTOR(), vertices[4].ToVECTOR(), color);

		DrawLine3D(vertices[1].ToVECTOR(), vertices[5].ToVECTOR(), color);

		DrawLine3D(vertices[2].ToVECTOR(), vertices[6].ToVECTOR(), color);

		DrawLine3D(vertices[3].ToVECTOR(), vertices[7].ToVECTOR(), color);
		// ``````````````````````````‘O–Ê‚Æ”w–Ê‚ğÚ‘±

		DrawCube3D(vertices[0].ToVECTOR(), vertices[7].ToVECTOR(), 0xffff00, 0xddddff, true);
	}
private:
	Vector3 size;
};