#pragma once

#include "ColliderBase.h"

class XZCircleCollider : public ColliderBase
{
public:
    XZCircleCollider(COLLIDER_TAG tag, float radius, float yLength, const Vector3& pos = Vector3()) :
        ColliderBase(tag, pos),

        radius(radius),
        yLength(yLength)
    {
        SetShape(COLLIDER_SHAPE::XzCircle);
    }
	~XZCircleCollider()override = default;

	float GetRadius(void)const { return radius; }

	float GetYLength(void)const { return yLength; }

	void SetRadius(float radius) { this->radius = radius; }

	AABB GetAABB(void) const override {
		Vector3 p = GetPos();

        return AABB(
            Vector3(p.x - radius, p.y - yLength, p.z - radius),
            Vector3(p.x + radius, p.y + yLength, p.z + radius)
        );
	}

	void DrawDebug(unsigned int color = 0xffffff)override {

        // 円の分割数
        constexpr int DIV_NUM = 32;

		// 中心座標
        const Vector3 center = GetPos();

		// 上面・下面のY座標
        const float topY = center.y + yLength;
        const float bottomY = center.y - yLength;

        // 上面・下面
        for (int i = 0; i < DIV_NUM; i++) {
            float angle1 = DX_PI_F * 2.0f * i / DIV_NUM;
            float angle2 = DX_PI_F * 2.0f * (i + 1) / DIV_NUM;

            Vector3 top1 = Vector3(center.x + cosf(angle1) * radius, topY, center.z + sinf(angle1) * radius);

            Vector3 top2 = Vector3(center.x + cosf(angle2) * radius, topY, center.z + sinf(angle2) * radius);

            Vector3 bottom1 = Vector3(top1.x, bottomY, top1.z);

            Vector3 bottom2 = Vector3(top2.x, bottomY, top2.z);

            // 上面
            DrawLine3D(top1.ToVECTOR(), top2.ToVECTOR(), color);
            // 下面
            DrawLine3D(bottom1.ToVECTOR(), bottom2.ToVECTOR(), color);
            // 側面
            DrawLine3D(top1.ToVECTOR(), bottom1.ToVECTOR(), color);

        }
    }

private:
	// 半径
	float radius;

	// Y方向の長さ
	float yLength;
};