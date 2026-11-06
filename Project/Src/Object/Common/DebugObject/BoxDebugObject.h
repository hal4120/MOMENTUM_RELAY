#pragma once

#include "DebugObjectBase.h"

#include "../Collider/BoxCollider.h"

class BoxDebugObject : public DebugObjectBase
{
public:
	BoxDebugObject(
		const Vector3& size,

		const Vector3& pos = Vector3(),

		bool dynamicFlg = true,
		bool isGravity = true,
		bool pushFlg = true,
		bool isOperator = false
	) :
		DebugObjectBase(
			pos,
			dynamicFlg,
			isGravity,
			pushFlg,
			isOperator
		),
		size(size)
	{
	}
	~BoxDebugObject()override = default;

	void Load(void)override {

		// 動的オブジェクトとして生成された場合
		if ((GetColliderMobility() == COLLIDER_MOBILITY::Movable)) {

			// 物理挙動生成
			EnablePhysics();

			// 取得
			PhysicsBody* body = GetPhysicsBody();

			// 質量
			body->SetMass(10.0f);
			// 接触時物理応答を有効にする
			body->SetRotationEnabled(true);

			// 箱の大きさから回転慣性を近似
			body->SetShapeInertiaBox(size);

			// 反発係数
			body->SetRestitution(0.2f);

			body->SetFriction(0.6f);
			body->SetAngularDragPerFrame(0.01f);
		}

		boxCollider = new BoxCollider(COLLIDER_TAG::DebugObject, size);
		AddCollider(boxCollider);
	}

private:
	Vector3 size;

	// ActorBaseが所有・解放するコライダーへの参照（ここではdeleteしない）
	BoxCollider* boxCollider = nullptr;

	void SubDraw(void)override {
		if (boxCollider == nullptr) { return; }

		// コライダーの回転済み8頂点をそのまま描画に使用する。
		// これによりTransformの回転と見た目の回転が一致する。
		const auto vertices = boxCollider->GetVertices();

		// 各面の4頂点。6面をそれぞれ2三角形で描画する。
		static constexpr int faces[6][4] = {
			{ 0, 2, 3, 1 }, // -Z
			{ 4, 5, 7, 6 }, // +Z
			{ 0, 4, 6, 2 }, // -X
			{ 1, 3, 7, 5 }, // +X
			{ 0, 1, 5, 4 }, // -Y
			{ 2, 6, 7, 3 }  // +Y
		};

		for (const auto& face : faces) {
			DrawTriangle3D(vertices[face[0]].ToVECTOR(), vertices[face[1]].ToVECTOR(), vertices[face[2]].ToVECTOR(), 0x555555, true);
			DrawTriangle3D(vertices[face[0]].ToVECTOR(), vertices[face[2]].ToVECTOR(), vertices[face[3]].ToVECTOR(), 0x555555, true);
		}

		// 回転が見やすいよう、辺を黒い線で描画する。
		static constexpr int edges[12][2] = {
			{0,1}, {1,3}, {3,2}, {2,0},
			{4,5}, {5,7}, {7,6}, {6,4},
			{0,4}, {1,5}, {2,6}, {3,7}
		};
		for (const auto& edge : edges) {
			DrawLine3D(vertices[edge[0]].ToVECTOR(), vertices[edge[1]].ToVECTOR(), 0x000000);
		}
	}
};