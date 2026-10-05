#pragma once

#include <vector>

#include "ColliderBase.h"

class MeshCollider : public ColliderBase
{
public:

	// メッシュを構成する三角形
	struct Triangle {
		// ローカル座標
		Vector3 p0, p1, p2;

		// ローカル法線
		Vector3 normal;

		// ローカルAABB
		AABB aabb;

		// 三角形中心
		Vector3 center;

		Triangle(void);
		Triangle(const Vector3& p0, const Vector3& p1, const Vector3& p2);
	};

private:

	// BVHノード
	struct BVHNode {
		AABB aabb;

		// 子ノードインデックス
		int left;
		int right;

		// 葉ノードが参照する三角形インデックス範囲
		unsigned int triangleStart;
		unsigned int triangleCount;

		BVHNode(void);

		bool IsLeaf(void)const;
	};

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	MeshCollider(COLLIDER_TAG type, const Vector3& pos = Vector3(), const Quaternion& rotation = Quaternion());

	/// <summary>
	/// モデルハンドルから三角形情報を構築するコンストラクタ
	/// </summary>
	MeshCollider(COLLIDER_TAG type, int modelHandle, int frameIndex = -1, const Vector3& pos = Vector3(), const Quaternion& rotation = Quaternion());

	~MeshCollider()override = default;

	/// <summary>
	/// DxLibモデルの参照メッシュから三角形情報とBVHを構築
	/// </summary>
	/// <param name="modelHandle">MV1モデルハンドル</param>
	/// <param name="frameIndex">対象フレーム。-1でモデル全体</param>
	/// <returns>構築成功でtrue</returns>
	bool SetupFromModel(int modelHandle, int frameIndex = -1);

	/// <summary>
	/// 頂点配列とインデックス配列から構築
	/// </summary>
	bool Setup(const std::vector<Vector3>& vertices, const std::vector<unsigned int>& indices);

	/// <summary>
	/// 三角形配列から構築
	/// </summary>
	bool Setup(const std::vector<Triangle>& triangles);

	// データ解放
	void Clear(void);

	// 自分が占有しているワールドAABB
	AABB GetAABB(void)const override;

	// デバッグ描画
	void DrawDebug(unsigned int color = 0xffffff)override;

	// 指定ワールドAABBと重なる候補三角形インデックスを取得
	void QueryTriangles(const AABB& worldAABB, std::vector<unsigned int>& outIndices)const;

	// 三角形取得
	const Triangle& GetTriangle(unsigned int triangleIndex)const;

	// 三角形数
	unsigned int GetTriangleNum(void)const;

	// ローカル座標からワールド座標へ変換
	Vector3 ToWorldPoint(const Vector3& localPoint)const;

	// ワールド座標からローカル座標へ変換
	Vector3 ToLocalPoint(const Vector3& worldPoint)const;

	// ローカル方向からワールド方向へ変換
	Vector3 ToWorldDirection(const Vector3& localDirection)const;

	// ワールド方向からローカル方向へ変換
	Vector3 ToLocalDirection(const Vector3& worldDirection)const;

	// ローカル三角形をワールド座標で取得
	void GetWorldTriangle(unsigned int triangleIndex, Vector3& p0, Vector3& p1, Vector3& p2, Vector3& normal)const;

private:

	// 1葉ノードに格納する最大三角形数
	static constexpr unsigned int LEAF_TRIANGLE_COUNT = 8;

	// 三角形一覧
	std::vector<Triangle> triangles;

	// BVH内で並び替えた三角形インデックス
	std::vector<unsigned int> triangleIndices;

	// 配列形式のBVH
	std::vector<BVHNode> bvhNodes;

	// メッシュ全体のローカルAABB
	AABB localAABB;

	// BVH構築
	void BuildBVH(void);
	int BuildBVHNode(unsigned int start, unsigned int count);

	// BVH検索
	void QueryBVH(int nodeIndex, const AABB& localQueryAABB, std::vector<unsigned int>& outIndices)const;

	// 指定範囲の三角形AABBを計算
	AABB CalculateTriangleRangeAABB(unsigned int start, unsigned int count)const;

	// ワールドAABBをメッシュローカルAABBへ変換
	AABB WorldAABBToLocalAABB(const AABB& worldAABB)const;

	// AABB重なり判定
	static bool IsAABBOverlap(const AABB& a, const AABB& b);
};
