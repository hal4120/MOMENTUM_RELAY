#include "MeshCollider.h"

#include <algorithm>
#include <cfloat>
#include <numeric>

MeshCollider::Triangle::Triangle(void) :
	p0(),
	p1(),
	p2(),
	normal(),
	aabb(),
	center()
{
}

MeshCollider::Triangle::Triangle(
	const Vector3& p0,
	const Vector3& p1,
	const Vector3& p2
) :
	p0(p0),
	p1(p1),
	p2(p2),
	normal(),
	aabb(),
	center()
{
	const Vector3 edge0 = p1 - p0;
	const Vector3 edge1 = p2 - p0;

	normal = edge0.Cross(edge1);
	if (normal.LengthSq() > 0.000001f)
	{
		normal.Normalize();
	}

	aabb.min = Vector3(
		(std::min)({ p0.x,p1.x,p2.x }),
		(std::min)({ p0.y,p1.y,p2.y }),
		(std::min)({ p0.z,p1.z,p2.z })
	);

	aabb.max = Vector3(
		(std::max)({ p0.x,p1.x,p2.x }),
		(std::max)({ p0.y,p1.y,p2.y }),
		(std::max)({ p0.z,p1.z,p2.z })
	);

	center = (p0 + p1 + p2) / 3.0f;
}

MeshCollider::BVHNode::BVHNode(void) :
	aabb(),
	left(-1),
	right(-1),
	triangleStart(0),
	triangleCount(0)
{
}

bool MeshCollider::BVHNode::IsLeaf(void)const
{
	return left < 0 && right < 0;
}

MeshCollider::MeshCollider(COLLIDER_TAG type, const Vector3& pos, const Quaternion& rotation) :
	ColliderBase(type, pos, rotation),
	triangles(),
	triangleIndices(),
	bvhNodes(),
	localAABB()
{
	SetShape(COLLIDER_SHAPE::Mesh);
}

MeshCollider::MeshCollider(COLLIDER_TAG type, int modelHandle, int frameIndex, const Vector3& pos, const Quaternion& rotation) :
	MeshCollider(type, pos, rotation)
{
	SetupFromModel(modelHandle, frameIndex);
}

bool MeshCollider::SetupFromModel(int modelHandle, int frameIndex)
{
	Clear();

	if (modelHandle < 0) { return false; }

	/*
		参照メッシュはローカル座標で取得する。
		三角形を毎フレームワールド変換せず、
		判定時にBVHから得た候補だけをワールド座標へ変換する。
	*/
	const MV1_REF_POLYGONLIST referenceMesh = MV1GetReferenceMesh(modelHandle, frameIndex, true, true);

	if (referenceMesh.PolygonNum <= 0 || referenceMesh.Polygons == nullptr) { return false; }

	triangles.reserve(static_cast<size_t>(referenceMesh.PolygonNum));

	for (int i = 0; i < referenceMesh.PolygonNum; i++) {
		const MV1_REF_POLYGON& polygon = referenceMesh.Polygons[i];

		triangles.emplace_back(
			Vector3(referenceMesh.Vertexs[polygon.VIndex[0]].Position),
			Vector3(referenceMesh.Vertexs[polygon.VIndex[1]].Position),
			Vector3(referenceMesh.Vertexs[polygon.VIndex[2]].Position)
		);
	}

	BuildBVH();

	return !triangles.empty();
}

bool MeshCollider::Setup(
	const std::vector<Vector3>& vertices,
	const std::vector<unsigned int>& indices
)
{
	Clear();

	if (
		vertices.empty() ||
		indices.size() < 3 ||
		indices.size() % 3 != 0
	)
	{
		return false;
	}

	triangles.reserve(indices.size() / 3);

	for (
		size_t i = 0;
		i < indices.size();
		i += 3
	)
	{
		const unsigned int i0 = indices[i + 0];
		const unsigned int i1 = indices[i + 1];
		const unsigned int i2 = indices[i + 2];

		if (
			i0 >= vertices.size() ||
			i1 >= vertices.size() ||
			i2 >= vertices.size()
		)
		{
			Clear();
			return false;
		}

		triangles.emplace_back(
			vertices[i0],
			vertices[i1],
			vertices[i2]
		);
	}

	BuildBVH();

	return !triangles.empty();
}

bool MeshCollider::Setup(
	const std::vector<Triangle>& triangles
)
{
	Clear();

	this->triangles = triangles;

	if (this->triangles.empty())
	{
		return false;
	}

	BuildBVH();

	return true;
}

void MeshCollider::Clear(void)
{
	triangles.clear();
	triangleIndices.clear();
	bvhNodes.clear();

	localAABB = AABB();
}

ColliderBase::AABB MeshCollider::GetAABB(void)const
{
	if (triangles.empty())
	{
		const Vector3 pos = GetPos();
		return AABB(pos, pos);
	}

	const Vector3 localCorners[8] = {
		Vector3(localAABB.min.x, localAABB.min.y, localAABB.min.z),
		Vector3(localAABB.max.x, localAABB.min.y, localAABB.min.z),
		Vector3(localAABB.min.x, localAABB.max.y, localAABB.min.z),
		Vector3(localAABB.max.x, localAABB.max.y, localAABB.min.z),
		Vector3(localAABB.min.x, localAABB.min.y, localAABB.max.z),
		Vector3(localAABB.max.x, localAABB.min.y, localAABB.max.z),
		Vector3(localAABB.min.x, localAABB.max.y, localAABB.max.z),
		Vector3(localAABB.max.x, localAABB.max.y, localAABB.max.z),
	};

	Vector3 minPos = ToWorldPoint(localCorners[0]);
	Vector3 maxPos = minPos;

	for (int i = 1; i < 8; ++i)
	{
		const Vector3 worldPos =
			ToWorldPoint(localCorners[i]);

		minPos.x = (std::min)(minPos.x, worldPos.x);
		minPos.y = (std::min)(minPos.y, worldPos.y);
		minPos.z = (std::min)(minPos.z, worldPos.z);

		maxPos.x = (std::max)(maxPos.x, worldPos.x);
		maxPos.y = (std::max)(maxPos.y, worldPos.y);
		maxPos.z = (std::max)(maxPos.z, worldPos.z);
	}

	return AABB(minPos, maxPos);
}

void MeshCollider::DrawDebug(unsigned int color)
{
	for (
		unsigned int i = 0;
		i < static_cast<unsigned int>(triangles.size());
		++i
	)
	{
		Vector3 p0, p1, p2, normal;
		GetWorldTriangle(i, p0, p1, p2, normal);

		DrawLine3D(
			p0.ToVECTOR(),
			p1.ToVECTOR(),
			color
		);

		DrawLine3D(
			p1.ToVECTOR(),
			p2.ToVECTOR(),
			color
		);

		DrawLine3D(
			p2.ToVECTOR(),
			p0.ToVECTOR(),
			color
		);
	}
}

void MeshCollider::QueryTriangles(
	const AABB& worldAABB,
	std::vector<unsigned int>& outIndices
)const
{
	outIndices.clear();

	if (bvhNodes.empty()) { return; }

	const AABB localQueryAABB =
		WorldAABBToLocalAABB(worldAABB);

	QueryBVH(
		0,
		localQueryAABB,
		outIndices
	);
}

const MeshCollider::Triangle&
	MeshCollider::GetTriangle(
		unsigned int triangleIndex
	)const
{
	return triangles.at(triangleIndex);
}

unsigned int MeshCollider::GetTriangleNum(void)const
{
	return static_cast<unsigned int>(
		triangles.size()
	);
}

Vector3 MeshCollider::ToWorldPoint(
	const Vector3& localPoint
)const
{
	const Transform& transform = GetTransform();

	Vector3 scaled = localPoint * transform.scale;
	Vector3 rotated = scaled.TransMat(GetRotationMat());

	return GetPos() + rotated;
}

Vector3 MeshCollider::ToLocalPoint(
	const Vector3& worldPoint
)const
{
	const Transform& transform = GetTransform();

	const MATRIX inverseAngle =
		MInverse(GetRotationMat());

	Vector3 local =
		(worldPoint - GetPos()).TransMat(
			inverseAngle
		);

	constexpr float epsilon = 0.000001f;

	local.x =
		(std::abs(transform.scale.x) > epsilon) ?
		local.x / transform.scale.x :
		0.0f;

	local.y =
		(std::abs(transform.scale.y) > epsilon) ?
		local.y / transform.scale.y :
		0.0f;

	local.z =
		(std::abs(transform.scale.z) > epsilon) ?
		local.z / transform.scale.z :
		0.0f;

	return local;
}

Vector3 MeshCollider::ToWorldDirection(
	const Vector3& localDirection
)const
{
	Vector3 direction =
		localDirection.TransMat(GetRotationMat());

	if (direction.LengthSq() > 0.000001f)
	{
		direction.Normalize();
	}

	return direction;
}

Vector3 MeshCollider::ToLocalDirection(
	const Vector3& worldDirection
)const
{
	Vector3 direction =
		worldDirection.TransMat(
			MInverse(GetRotationMat())
		);

	if (direction.LengthSq() > 0.000001f)
	{
		direction.Normalize();
	}

	return direction;
}

void MeshCollider::GetWorldTriangle(
	unsigned int triangleIndex,
	Vector3& p0,
	Vector3& p1,
	Vector3& p2,
	Vector3& normal
)const
{
	const Triangle& triangle =
		GetTriangle(triangleIndex);

	p0 = ToWorldPoint(triangle.p0);
	p1 = ToWorldPoint(triangle.p1);
	p2 = ToWorldPoint(triangle.p2);

	normal = (p1 - p0).Cross(p2 - p0);

	if (normal.LengthSq() > 0.000001f)
	{
		normal.Normalize();
	}
}

void MeshCollider::BuildBVH(void)
{
	triangleIndices.resize(triangles.size());

	std::iota(
		triangleIndices.begin(),
		triangleIndices.end(),
		0u
	);

	bvhNodes.clear();
	bvhNodes.reserve(
		triangles.size() * 2
	);

	localAABB =
		CalculateTriangleRangeAABB(
			0,
			static_cast<unsigned int>(
				triangleIndices.size()
			)
		);

	BuildBVHNode(
		0,
		static_cast<unsigned int>(
			triangleIndices.size()
		)
	);
}

int MeshCollider::BuildBVHNode(
	unsigned int start,
	unsigned int count
)
{
	const int nodeIndex =
		static_cast<int>(bvhNodes.size());

	bvhNodes.emplace_back();

	bvhNodes[nodeIndex].aabb =
		CalculateTriangleRangeAABB(
			start,
			count
		);

	if (count <= LEAF_TRIANGLE_COUNT)
	{
		bvhNodes[nodeIndex].triangleStart = start;
		bvhNodes[nodeIndex].triangleCount = count;

		return nodeIndex;
	}

	const Vector3 size =
		bvhNodes[nodeIndex].aabb.max -
		bvhNodes[nodeIndex].aabb.min;

	int splitAxis = 0;

	if (
		size.y > size.x &&
		size.y >= size.z
	)
	{
		splitAxis = 1;
	}
	else if (
		size.z > size.x &&
		size.z > size.y
	)
	{
		splitAxis = 2;
	}

	const unsigned int center =
		start + count / 2;

	std::nth_element(
		triangleIndices.begin() + start,
		triangleIndices.begin() + center,
		triangleIndices.begin() + start + count,
		[this, splitAxis](
			unsigned int a,
			unsigned int b
		)
		{
			const Vector3& aCenter =
				triangles[a].center;

			const Vector3& bCenter =
				triangles[b].center;

			if (splitAxis == 0)
			{
				return aCenter.x < bCenter.x;
			}

			if (splitAxis == 1)
			{
				return aCenter.y < bCenter.y;
			}

			return aCenter.z < bCenter.z;
		}
	);

	const unsigned int leftCount =
		center - start;

	const unsigned int rightCount =
		count - leftCount;

	const int left =
		BuildBVHNode(
			start,
			leftCount
		);

	const int right =
		BuildBVHNode(
			center,
			rightCount
		);

	bvhNodes[nodeIndex].left = left;
	bvhNodes[nodeIndex].right = right;

	return nodeIndex;
}

void MeshCollider::QueryBVH(
	int nodeIndex,
	const AABB& localQueryAABB,
	std::vector<unsigned int>& outIndices
)const
{
	const BVHNode& node =
		bvhNodes[nodeIndex];

	if (
		!IsAABBOverlap(
			node.aabb,
			localQueryAABB
		)
	)
	{
		return;
	}

	if (node.IsLeaf())
	{
		for (
			unsigned int i = 0;
			i < node.triangleCount;
			++i
		)
		{
			const unsigned int triangleIndex =
				triangleIndices[
					node.triangleStart + i
				];

			if (
				IsAABBOverlap(
					triangles[triangleIndex].aabb,
					localQueryAABB
				)
			)
			{
				outIndices.emplace_back(
					triangleIndex
				);
			}
		}

		return;
	}

	QueryBVH(
		node.left,
		localQueryAABB,
		outIndices
	);

	QueryBVH(
		node.right,
		localQueryAABB,
		outIndices
	);
}

ColliderBase::AABB
	MeshCollider::CalculateTriangleRangeAABB(
		unsigned int start,
		unsigned int count
	)const
{
	AABB result;

	result.min =
		Vector3(FLT_MAX);

	result.max =
		Vector3(-FLT_MAX);

	for (
		unsigned int i = 0;
		i < count;
		++i
	)
	{
		const Triangle& triangle =
			triangles[
				triangleIndices[start + i]
			];

		result.min.x =
			(std::min)(
				result.min.x,
				triangle.aabb.min.x
			);

		result.min.y =
			(std::min)(
				result.min.y,
				triangle.aabb.min.y
			);

		result.min.z =
			(std::min)(
				result.min.z,
				triangle.aabb.min.z
			);

		result.max.x =
			(std::max)(
				result.max.x,
				triangle.aabb.max.x
			);

		result.max.y =
			(std::max)(
				result.max.y,
				triangle.aabb.max.y
			);

		result.max.z =
			(std::max)(
				result.max.z,
				triangle.aabb.max.z
			);
	}

	return result;
}

ColliderBase::AABB
	MeshCollider::WorldAABBToLocalAABB(
		const AABB& worldAABB
	)const
{
	const Vector3 worldCorners[8] = {
		Vector3(worldAABB.min.x, worldAABB.min.y, worldAABB.min.z),
		Vector3(worldAABB.max.x, worldAABB.min.y, worldAABB.min.z),
		Vector3(worldAABB.min.x, worldAABB.max.y, worldAABB.min.z),
		Vector3(worldAABB.max.x, worldAABB.max.y, worldAABB.min.z),
		Vector3(worldAABB.min.x, worldAABB.min.y, worldAABB.max.z),
		Vector3(worldAABB.max.x, worldAABB.min.y, worldAABB.max.z),
		Vector3(worldAABB.min.x, worldAABB.max.y, worldAABB.max.z),
		Vector3(worldAABB.max.x, worldAABB.max.y, worldAABB.max.z),
	};

	Vector3 minPos =
		ToLocalPoint(worldCorners[0]);

	Vector3 maxPos = minPos;

	for (int i = 1; i < 8; ++i)
	{
		const Vector3 localPos =
			ToLocalPoint(worldCorners[i]);

		minPos.x =
			(std::min)(minPos.x, localPos.x);

		minPos.y =
			(std::min)(minPos.y, localPos.y);

		minPos.z =
			(std::min)(minPos.z, localPos.z);

		maxPos.x =
			(std::max)(maxPos.x, localPos.x);

		maxPos.y =
			(std::max)(maxPos.y, localPos.y);

		maxPos.z =
			(std::max)(maxPos.z, localPos.z);
	}

	return AABB(minPos, maxPos);
}

bool MeshCollider::IsAABBOverlap(
	const AABB& a,
	const AABB& b
)
{
	return !(
		a.max.x < b.min.x ||
		a.min.x > b.max.x ||
		a.max.y < b.min.y ||
		a.min.y > b.max.y ||
		a.max.z < b.min.z ||
		a.min.z > b.max.z
	);
}
