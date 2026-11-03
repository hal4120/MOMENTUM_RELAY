#include <chrono>
#include <cassert>
#include"CollisionManager.h"
#include "ContactSolver.h"

#include <algorithm>
#include <array>
#include <cfloat>
#include <cmath>
#include <vector>

#include "../../Application/Application.h"

#include "../../Common/Vector2.h"
#include "../../Common/Vector2I.h"

#include "../../Object/Common/ActorBase/ActorBase.h"
#include "../../Object/Common/PhysicsBody/PhysicsBody.h"

#include"../../Object/Common/Collider/LineCollider.h"
#include"../../Object/Common/Collider/SphereCollider.h"
#include"../../Object/Common/Collider/CapsuleCollider.h"
#include"../../Object/Common/Collider/BoxCollider.h"
#include"../../Object/Common/Collider/MeshCollider.h"
#include"../../Object/Common/Collider/XZCircleCollider.h"

namespace
{
// 形状と派生クラスの一致をDebugで検証し、Releaseでは高速に変換
template <typename T>
T* ColliderCast(ColliderBase* collider)
{
#if defined(_DEBUG) && defined(ENABLE_COLLIDER_CAST_CHECK)

    // 型の整合性を厳密に確認する
    T* result = dynamic_cast<T*>(collider);
    assert(result != nullptr);
    return result;

#else
    // 通常は高速なキャストを使用
    return static_cast<T*>(collider);
#endif
}

// BoxとSphere・Capsuleの組み合わせか判定
bool IsSweptBoxPair(COLLIDER_SHAPE a, COLLIDER_SHAPE b)
{
    return (a == COLLIDER_SHAPE::Box &&
        (b == COLLIDER_SHAPE::Sphere || b == COLLIDER_SHAPE::Capsule)) ||
        (b == COLLIDER_SHAPE::Box &&
        (a == COLLIDER_SHAPE::Sphere || a == COLLIDER_SHAPE::Capsule));
}

// 1フレーム前の位置も含むAABBに拡張
void ExpandSweptAABB(ColliderBase* collider, ColliderBase::AABB& bounds)
{
    if (collider->GetMobility() != COLLIDER_MOBILITY::Movable) { return; }

    const Vector3 delta = collider->GetPos() - collider->GetPrevPos();
    const Vector3 oldMin = bounds.min - delta;
    const Vector3 oldMax = bounds.max - delta;

    bounds.min.x = (std::min)(bounds.min.x, oldMin.x);
    bounds.min.y = (std::min)(bounds.min.y, oldMin.y);
    bounds.min.z = (std::min)(bounds.min.z, oldMin.z);
    bounds.max.x = (std::max)(bounds.max.x, oldMax.x);
    bounds.max.y = (std::max)(bounds.max.y, oldMax.y);
    bounds.max.z = (std::max)(bounds.max.z, oldMax.z);
}

// 2つのAABBが重なっているか判定
bool OverlapsAABB(const ColliderBase::AABB& a, const ColliderBase::AABB& b)
{
    return a.max.x >= b.min.x && a.min.x <= b.max.x &&
        a.max.y >= b.min.y && a.min.y <= b.max.y &&
        a.max.z >= b.min.z && a.min.z <= b.max.z;
}

}

namespace
{
// 単一接触点を共通ソルバーへ登録する形状の組み合わせか判定
// Box同士は複数接触点を専用処理で登録する
// Line・XZCircleは専用の位置補正を使用する
bool UsesSingleContactManifold(COLLIDER_SHAPE a, COLLIDER_SHAPE b)
{
    if (a == COLLIDER_SHAPE::Box && b == COLLIDER_SHAPE::Box) { return false; }
    if (a == COLLIDER_SHAPE::Line || b == COLLIDER_SHAPE::Line) { return false; }
    if (a == COLLIDER_SHAPE::XzCircle || b == COLLIDER_SHAPE::XzCircle) { return false; }
    return true;
}

}

namespace
{
// 衝突結果の法線とめり込み量を補完
// Lineは体積を持たないため、めり込み量を0のままとする
void CompleteCollisionResult(ColliderBase* a, ColliderBase* b, CollisionResult& result)
{
	// 法線が求まらなかった組み合わせ用の安全処理
	if (result.normal.LengthSq() <= 0.000001f) {
		result.normal = a->GetPos() - b->GetPos();

		if (result.normal.LengthSq() > 0.000001f) { result.normal.Normalize(); }
		else { result.normal = Vector3::Yonly(1.0f); }
	}

	result.penetration = (std::max)(result.penetration, 0.0f);

	// 判定関数側でめり込み量を明示できなかった場合の補完
	// Line系は体積を持たないため0のままとする
	if (result.penetration <= 0.0f && a->GetShape() != COLLIDER_SHAPE::Line && b->GetShape() != COLLIDER_SHAPE::Line) {
		const ColliderBase::AABB aAABB = a->GetAABB();

		const ColliderBase::AABB bAABB = b->GetAABB();

		const float overlapX = (std::min)(aAABB.max.x, bAABB.max.x) - (std::max)(aAABB.min.x, bAABB.min.x);

		const float overlapY = (std::min)(aAABB.max.y, bAABB.max.y) - (std::max)(aAABB.min.y, bAABB.min.y);

		const float overlapZ = (std::min)(aAABB.max.z, bAABB.max.z) - (std::max)(aAABB.min.z, bAABB.min.z);

		result.penetration = (std::max)(0.0f, (std::min)({ overlapX,overlapY,overlapZ }));
	}
}

}

namespace
{
// 単一接触点を共通ソルバーへ登録
// Box同士は形状判定側で複数接触点を登録する
void RegisterSingleContact(ColliderBase* a, ColliderBase* b, const CollisionResult& result)
{
    if (!UsesSingleContactManifold(a->GetShape(), b->GetShape())) { return; }

    ContactManifold contact;
    contact.a = a;
    contact.b = b;
    contact.normal = result.normal;
    contact.penetration = result.penetration;
    contact.AddPoint(result.point);
    ContactSolver::AddContact(contact);
}

// AとBへそれぞれの向きの衝突法線を通知
void NotifyCollisionPair(ColliderBase* a, ColliderBase* b, const CollisionResult& result)
{
    CollisionResult aResult = result;
    CollisionResult bResult = result;

    // Bへ通知する法線だけ反転する
    bResult.normal = -result.normal;

    a->CallOnCollision(a->GetTag(), *b, aResult);
    b->CallOnCollision(b->GetTag(), *a, bResult);
}
}

// 衝突法線とめり込み量を設定
// 衝突点は形状ごとの判定処理で設定する
void SetContactNormalAndDepth(CollisionResult& result, const Vector3& normal, float penetration)
{
    result.normal = normal;
    result.penetration = penetration;
}

// Sphere × Box CCDの前方宣言（定義はファイル後半）。
namespace { bool SweptSphereBoxTOI(SphereCollider* sphere, BoxCollider* box, CollisionResult& result); }

// 回転後のBoxの指定方向側の面中心を求める。
// 支持点（角）ではなく面の中心を返す。
namespace
{
    Vector3 BoxFaceCenter(const BoxCollider* box, const Vector3& direction)
    {
        if (box == nullptr) { return Vector3(0.0f); }

        const auto axes = box->GetAxes();
        const Vector3 half = box->GetHalfSize();
        const float lengths[3] = { half.x, half.y, half.z };

        int faceAxis = 0;
        float best = std::abs(direction.Dot(axes[0]));
        for (int i = 1; i < 3; ++i) {
            const float alignment = std::abs(direction.Dot(axes[i]));
            if (alignment > best) {
                best = alignment;
                faceAxis = i;
            }
        }
        const float sign = direction.Dot(axes[faceAxis]) >= 0.0f ? 1.0f : -1.0f;
        return box->GetPos() + axes[faceAxis] * (lengths[faceAxis] * sign);
    }
}

// 衝突判定のための補助関数群をまとめた名前空間
namespace
{
	// 衝突判定の計算誤差を吸収するための微小値
	constexpr float COLLISION_EPSILON = 0.000001f;

	// 三角形上の最近接点を求める
	Vector3 ClosestPointOnTriangle(const Vector3& point, const Vector3& a, const Vector3& b, const Vector3& c) {
		const Vector3 ab = b - a;
		const Vector3 ac = c - a;
		const Vector3 ap = point - a;

		const float d1 = ab.Dot(ap);
		const float d2 = ac.Dot(ap);

		if (d1 <= 0.0f && d2 <= 0.0f) { return a; }

		const Vector3 bp = point - b;
		const float d3 = ab.Dot(bp);
		const float d4 = ac.Dot(bp);

		if (d3 >= 0.0f && d4 <= d3) { return b; }

		const float vc = d1 * d4 - d3 * d2;

		if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
			const float v = d1 / (d1 - d3);
			return a + ab * v;
		}

		const Vector3 cp = point - c;
		const float d5 = ab.Dot(cp);
		const float d6 = ac.Dot(cp);

		if (d6 >= 0.0f && d5 <= d6) { return c; }

		const float vb = d5 * d2 - d1 * d6;

		if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
			const float w = d2 / (d2 - d6);
			return a + ac * w;
		}

		const float va = d3 * d6 - d5 * d4;

		if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
			const Vector3 bc = c - b;

			const float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));

			return b + bc * w;
		}

		const float denominator = 1.0f / (va + vb + vc);

		const float v = vb * denominator;
		const float w = vc * denominator;

		return a + ab * v + ac * w;
	}

	// 2つの線分の最近接点を求める
	void ClosestPointsBetweenSegments(
		const Vector3& p0, const Vector3& p1,
		const Vector3& q0, const Vector3& q1,
		Vector3& resultP,
		Vector3& resultQ
	)
	{
		const Vector3 d0 = p1 - p0;
		const Vector3 d1 = q1 - q0;
		const Vector3 r = p0 - q0;

		const float a = d0.Dot(d0);
		const float e = d1.Dot(d1);
		const float f = d1.Dot(r);

		float s = 0.0f;
		float t = 0.0f;

		if (a <= COLLISION_EPSILON && e <= COLLISION_EPSILON) {
			resultP = p0;
			resultQ = q0;
			return;
		}

		if (a <= COLLISION_EPSILON) {
			t = std::clamp(f / e, 0.0f, 1.0f);
		}
		else {
			const float c = d0.Dot(r);

			if (e <= COLLISION_EPSILON) {
				s = std::clamp(-c / a, 0.0f, 1.0f);
			}
			else {
				const float b = d0.Dot(d1);
				const float denominator = a * e - b * b;

				if (std::abs(denominator) > COLLISION_EPSILON) {
					s = std::clamp((b * f - c * e) / denominator, 0.0f, 1.0f);
				}

				t = (b * s + f) / e;

				if (t < 0.0f) {
					t = 0.0f;
					s = std::clamp(-c / a, 0.0f, 1.0f);
				}
				else if (t > 1.0f) {
					t = 1.0f;
					s = std::clamp((b - c) / a, 0.0f, 1.0f);
				}
			}
		}

		resultP = p0 + d0 * s;
		resultQ = q0 + d1 * t;
	}

	// 線分と三角形の交差判定を行う
	bool SegmentToTriangle(
		const Vector3& start, const Vector3& end,
		const Vector3& p0, const Vector3& p1, const Vector3& p2,
		float& hitRate,
		Vector3& hitPoint,
		Vector3& triangleNormal
	)
	{
		const Vector3 direction = end - start;
		const Vector3 edge0 = p1 - p0;
		const Vector3 edge1 = p2 - p0;

		const Vector3 p = direction.Cross(edge1);
		const float determinant = edge0.Dot(p);

		if (std::abs(determinant) <= COLLISION_EPSILON) { return false; }

		const float inverseDeterminant = 1.0f / determinant;

		const Vector3 t = start - p0;

		const float u = t.Dot(p) * inverseDeterminant;

		if (u < 0.0f || u > 1.0f) { return false; }

		const Vector3 q = t.Cross(edge0);

		const float v = direction.Dot(q) * inverseDeterminant;

		if (v < 0.0f || u + v > 1.0f) { return false; }

		const float rate = edge1.Dot(q) * inverseDeterminant;

		if (rate < 0.0f || rate > 1.0f) { return false; }

		hitRate = rate;
		hitPoint = start + direction * rate;

		triangleNormal = edge0.Cross(edge1);

		if (triangleNormal.LengthSq() > COLLISION_EPSILON) { triangleNormal.Normalize(); }

		return true;
	}

	// 線分と三角形の最近接点を求める
	float SegmentToTriangleDistanceSq(
		const Vector3& segmentStart, const Vector3& segmentEnd,
		const Vector3& p0, const Vector3& p1, const Vector3& p2,
		Vector3& segmentPoint,
		Vector3& trianglePoint
	)
	{
		float hitRate = 0.0f;
		Vector3 hitPoint;
		Vector3 hitNormal;

		if (SegmentToTriangle(segmentStart, segmentEnd, p0, p1, p2, hitRate, hitPoint, hitNormal)) {
			segmentPoint = hitPoint;
			trianglePoint = hitPoint;
			return 0.0f;
		}

		float minimumDistanceSq = FLT_MAX;

		auto Update = [&](const Vector3& a, const Vector3& b) {
			const float distanceSq = (a - b).LengthSq();

			if (distanceSq < minimumDistanceSq) {
				minimumDistanceSq = distanceSq;

				segmentPoint = a;
				trianglePoint = b;
			}
			};

		Update(segmentStart, ClosestPointOnTriangle(segmentStart, p0, p1, p2));

		Update(segmentEnd, ClosestPointOnTriangle(segmentEnd, p0, p1, p2));

		const Vector3 edgeStarts[3] = { p0, p1, p2 };

		const Vector3 edgeEnds[3] = { p1, p2, p0 };

		for (int i = 0; i < 3; ++i) {
			Vector3 pointOnSegment;
			Vector3 pointOnEdge;

			ClosestPointsBetweenSegments(segmentStart, segmentEnd, edgeStarts[i], edgeEnds[i], pointOnSegment, pointOnEdge);

			Update(pointOnSegment, pointOnEdge);
		}

		return minimumDistanceSq;
	}

	// 三角形とOBBの交差判定を行う
	bool TriangleToOBB(
		const Vector3& worldP0, const Vector3& worldP1, const Vector3& worldP2,
		const BoxCollider& box,
		Vector3& normal,
		float& penetration
	)
	{
		const Vector3 boxCenter = box.GetPos();
		const std::array<Vector3, 3> boxAxes = box.GetAxes();

		const Vector3 halfSize = box.GetHalfSize();

		auto ToBoxLocal = [&](const Vector3& point) {
			const Vector3 diff = point - boxCenter;

			return Vector3(diff.Dot(boxAxes[0]), diff.Dot(boxAxes[1]), diff.Dot(boxAxes[2]));
			};

		const Vector3 p0 = ToBoxLocal(worldP0);
		const Vector3 p1 = ToBoxLocal(worldP1);
		const Vector3 p2 = ToBoxLocal(worldP2);

		const Vector3 edges[3] = { p1 - p0, p2 - p1, p0 - p2 };

		std::vector<Vector3> axes;
		axes.reserve(13);

		axes.emplace_back(Vector3::Xonly(1.0f));

		axes.emplace_back(Vector3::Yonly(1.0f));

		axes.emplace_back(Vector3::Zonly(1.0f));

		axes.emplace_back(edges[0].Cross(edges[1]));

		const Vector3 boxLocalAxes[3] = { Vector3::Xonly(1.0f), Vector3::Yonly(1.0f), Vector3::Zonly(1.0f) };

		for (const Vector3& edge : edges) {
			for (const Vector3& boxAxis : boxLocalAxes) {
				axes.emplace_back(edge.Cross(boxAxis));
			}
		}

		float minimumOverlap = FLT_MAX;
		Vector3 minimumAxis = Vector3::Yonly(1.0f);

		for (Vector3 axis : axes) {
			if (axis.LengthSq() <= COLLISION_EPSILON) { continue; }

			axis.Normalize();

			const float p0Projection = p0.Dot(axis);

			const float p1Projection = p1.Dot(axis);

			const float p2Projection = p2.Dot(axis);

			const float triangleMin = (std::min)({ p0Projection, p1Projection, p2Projection });

			const float triangleMax = (std::max)({ p0Projection, p1Projection, p2Projection });

			const float boxRadius = std::abs(axis.x) * halfSize.x + std::abs(axis.y) * halfSize.y + std::abs(axis.z) * halfSize.z;

			// Boxはローカル原点中心なので、投影区間は[-boxRadius, boxRadius]
			const float boxMin = -boxRadius;
			const float boxMax = boxRadius;

			// 分離軸が存在する場合は未衝突
			if (triangleMax < boxMin || triangleMin > boxMax) { return false; }

			// 単純な交差区間長を使うと、厚み0の三角形法線軸では
			// 衝突中でも0になってしまう。
			// そのためBoxを正負どちらへ動かせば分離できるかを求め、
			// 短い方をこの軸の押し出し量として採用する。
			const float movePositive = triangleMax - boxMin;
			const float moveNegative = boxMax - triangleMin;

			float axisPenetration = 0.0f;
			Vector3 pushAxis;

			if (movePositive < moveNegative) {
				axisPenetration = movePositive;
				pushAxis = axis;
			}
			else {
				axisPenetration = moveNegative;
				pushAxis = -axis;
			}

			if (axisPenetration < minimumOverlap) {
				minimumOverlap = axisPenetration;
				minimumAxis = pushAxis;
			}
		}

		normal = boxAxes[0] * minimumAxis.x + boxAxes[1] * minimumAxis.y + boxAxes[2] * minimumAxis.z;

		if (normal.LengthSq() > COLLISION_EPSILON) { normal.Normalize(); }

		// 数値誤差で同じ面へ再度めり込みやすいため、ごく小さい余裕を加える
		penetration = minimumOverlap + COLLISION_EPSILON;

		return true;
	}

	// 三角形のAABBを作成する
	ColliderBase::AABB MakeTriangleAABB(const Vector3& p0, const Vector3& p1, const Vector3& p2) {
		return ColliderBase::AABB(
			Vector3((std::min)({ p0.x,p1.x,p2.x }), (std::min)({ p0.y,p1.y,p2.y }), (std::min)({ p0.z,p1.z,p2.z })),
			Vector3((std::max)({ p0.x,p1.x,p2.x }), (std::max)({ p0.y,p1.y,p2.y }), (std::max)({ p0.z,p1.z,p2.z }))
		);
	}
}

#pragma region 初期化系

// コライダーを追加（単体）
void CollisionManager::Add(ColliderBase* collider)
{
	// 安全処理
	if (!collider) { return; }

	// グループを一時変数として定義（「その他」で初期化）
	COLLIDER_GROUP group = COLLIDER_GROUP::Other;

	// グループリストに記述が存在すれば、取得
	if (TAG_TO_GROUP_LIST.contains(collider->GetTag())) {
		group = TAG_TO_GROUP_LIST.at(collider->GetTag());
	}

	// 最終的なグループで格納
	groupColliders[(int)group].colliders.emplace_back(collider);
	groupColliders[(int)group].staticCacheDirty = true;
}

// コライダーを追加（複数）
void CollisionManager::Add(std::vector<ColliderBase*> collider)
{
	// 指定されたコライダーを順番に単体追加関数を使用して追加していく
	for (ColliderBase*& c : collider) { Add(c); }
}

// 現状抱えているコライダーすべてのチャンク分けを実行
void CollisionManager::InitBuildChunks(void)
{
    // 明示的な再構築要求。Staticの座標や形状を変更した場合にも呼び出す
    for (ColliderGroupData& group : groupColliders) {
        group.staticCacheDirty = true;
    }
    BuildChunks();
}

#pragma endregion

#pragma region 判定実行

void CollisionManager::Check(void)
{
	using Clock = std::chrono::steady_clock;
	const auto toMs = [](Clock::duration duration) {
		return std::chrono::duration<double, std::milli>(duration).count();
	};

	// フレームごとの判定統計を初期化
	checkStats = CheckStats{};

	ContactSolver::BeginFrame();
	const auto buildStart = Clock::now();
	// チャンク分け
	BuildChunks();
	checkStats.buildChunksMs = toMs(Clock::now() - buildStart);

	// 二重判定防止の判定記録配列をリセット
	checkedPairs.clear();
	const auto matchingStart = Clock::now();

	// ①プレイヤー系 × ステージ系
	Matching(COLLIDER_GROUP::Player, COLLIDER_GROUP::Stage);

	// ②エネミー系 × ステージ系
	Matching(COLLIDER_GROUP::Enemy, COLLIDER_GROUP::Stage);


	// ③プレイヤー系 × エネミー系
	Matching(COLLIDER_GROUP::Player, COLLIDER_GROUP::Enemy);


	// ④プレイヤー系 × プレイヤー系にだけ当たるコライダー
	Matching(COLLIDER_GROUP::Player, COLLIDER_GROUP::PlayerOnly);

	// ⑤エネミー系 × エネミー系にだけ当たるコライダー
	Matching(COLLIDER_GROUP::Enemy, COLLIDER_GROUP::EnemyOnly);

	// ⑥ステージ系 × ステージ系にだけ当たるコライダー
	Matching(COLLIDER_GROUP::Stage, COLLIDER_GROUP::StageOnly);


	// ⑦その他 × ステージ系
	Matching(COLLIDER_GROUP::Other, COLLIDER_GROUP::Stage);

	// ⑧その他 × プレイヤー系
	Matching(COLLIDER_GROUP::Other, COLLIDER_GROUP::Player);

	// ⑨その他 × エネミー系
	Matching(COLLIDER_GROUP::Other, COLLIDER_GROUP::Enemy);

	// ⑩その他 × その他
	Matching(COLLIDER_GROUP::Other);
	checkStats.matchingMs = toMs(Clock::now() - matchingStart);

	// 接触解決にかかった時間を記録
	const auto solverStart = Clock::now();
	ContactSolver::SolveAll();
	ContactSolver::EndFrame();
	checkStats.solverMs = toMs(Clock::now() - solverStart);
}

#pragma endregion

#pragma region 判定振り分け

void CollisionManager::BuildChunks(void)
{
    for (ColliderGroupData& group : groupColliders) {
        // Movable用チャンクの登録をリセット
        // マップと配列のメモリは維持して次の登録に再利用する
        for (auto& chunk : group.dynamicChunks) {
            chunk.second.colliders.clear();
        }

        // Staticの登録順と構成が変わったか判定
        // Staticの座標・形状変更はInitBuildChunksで再構築する
        bool staticChanged = group.staticCacheDirty;
        size_t staticIndex = 0;

        for (ColliderBase* collider : group.colliders) {
            if (!collider) { continue; }

            // 移動するコライダーは毎フレーム登録
            if (collider->GetMobility() == COLLIDER_MOBILITY::Movable) {
                RegisterToChunks(group.dynamicChunks, collider);
                continue;
            }

            // 前回のStatic一覧と順序を比較
            if (staticIndex >= group.staticCache.size() ||
                group.staticCache[staticIndex] != collider) {
                staticChanged = true;
            }
            staticIndex++;
        }

        // Staticの数が変わった場合も再構築
        if (staticIndex != group.staticCache.size()) {
            staticChanged = true;
        }

        // 今回使用しなかったMovable用チャンクを削除
        // 移動先の変化によって空のチャンクが増え続けるのを防ぐ
        for (auto it = group.dynamicChunks.begin(); it != group.dynamicChunks.end();) {
            if (it->second.colliders.empty()) {
                it = group.dynamicChunks.erase(it);
            }
            else {
                ++it;
            }
        }

        // Staticの構成に変更がない場合はキャッシュを再利用
        if (!staticChanged) { continue; }

        group.staticChunks.clear();
        group.staticCache.clear();
        group.staticCache.reserve(staticIndex);

        for (ColliderBase* collider : group.colliders) {
            if (!collider || collider->GetMobility() == COLLIDER_MOBILITY::Movable) {
                continue;
            }

            group.staticCache.emplace_back(collider);
            RegisterToChunks(group.staticChunks, collider);
        }

        group.staticCacheDirty = false;
    }
}

// 指定のグループ同士を判定(2グループ指定)
void CollisionManager::Matching(COLLIDER_GROUP groupA, COLLIDER_GROUP groupB)
{
	ColliderGroupData& a = groupColliders[(int)groupA];
	ColliderGroupData& b = groupColliders[(int)groupB];

	// 動的A × 動的B
	MatchingChunks(a.dynamicChunks, b.dynamicChunks);

	// 動的A × 静的B（3Dチャンク）
	MatchingChunks(a.dynamicChunks, b.staticChunks);

	// 静的A × 動的B（3Dチャンク）
	MatchingChunks(a.staticChunks, b.dynamicChunks);

	// 静的A × 静的Bは基本不要
}

// 指定のグループを総当たりで判定(1グループ指定)
void CollisionManager::Matching(COLLIDER_GROUP group)
{
	// グループを取得
	ColliderGroupData& g = groupColliders[(int)group];

	// 動的同士
	MatchingChunks(g.dynamicChunks);

	// 動的 × 静的
	MatchingChunks(g.dynamicChunks, g.staticChunks);

	// 静的同士は基本不要
}

// 異なるチャンクマップ同士で、同じ座標のコライダーを判定
void CollisionManager::MatchingChunks(ChunkMap& aChunks, ChunkMap& bChunks)
{
	// 判定対象が存在しない場合は終了
	if (aChunks.empty() || bChunks.empty()) { return; }

	// 1つ目のチャンクマップを基準に、同じ座標のチャンクを探す
	for (const auto& aChunk : aChunks) {

		// 2つ目のチャンクマップに同じ座標がなければ次へ
		auto bChunk = bChunks.find(aChunk.first);
		if (bChunk == bChunks.end()) { continue; }

		// コライダーがないチャンクは判定しない
		const auto& aColliders = aChunk.second.colliders;
		const auto& bColliders = bChunk->second.colliders;
		if (aColliders.empty() || bColliders.empty()) { continue; }

		// 同じ座標に登録されたコライダー同士を判定
		for (ColliderBase* a : aColliders) {
			for (ColliderBase* b : bColliders) {
				CheckPairOnce(a, b);
			}
		}
	}
}

// 同じチャンクマップ内で、コライダーの組み合わせを判定
void CollisionManager::MatchingChunks(ChunkMap& chunks)
{
	// 判定対象が存在しない場合は終了
	if (chunks.empty()) { return; }

	for (const auto& chunk : chunks) {

		// チャンク内のコライダーを取得
		const auto& colliders = chunk.second.colliders;

		// 2つ以上存在する場合のみ組み合わせを判定
		if (colliders.size() < 2) { continue; }

		for (size_t i = 0; i < colliders.size(); i++) {
			for (size_t j = i + 1; j < colliders.size(); j++) {
				CheckPairOnce(colliders[i], colliders[j]);
			}
		}
	}
}


// 重複判定チェック
void CollisionManager::CheckPairOnce(ColliderBase* a, ColliderBase* b)
{
	// 安全処理
	if (!a || !b) { return; }

	// 同じもの同士は判定しない
	if (a == b) { return; }

	// 同じアクターに紐づいているコライダー同士も判定しない
	if (a->GetActor() == b->GetActor()) { return; }

	// 判定候補の数を記録（同一ペアの重複を含む）
	checkStats.pairCandidates++;

	// このフレーム中に同じ組み合わせを判定済みなら終了
	// insertの結果を利用し、検索と登録を一度に行う
	const ColliderPairKey key(a, b);
	if (!checkedPairs.insert(key).second) { return; }
	checkStats.uniquePairs++;

	// 衝突情報
	CollisionResult result;

	// 判定を実行する
	if (IsHit(a, b, result)) {
		checkStats.hitPairs++;

		// 衝突情報を補完
		CompleteCollisionResult(a, b, result);

		// 共通ソルバーへ接触を登録
		if (NeedContactResponse(a, b)) {
			RegisterSingleContact(a, b, result);
		}

		// 双方のアクターへ衝突を通知
		NotifyCollisionPair(a, b, result);
	}
}

// 形状の振り分け
bool CollisionManager::IsHit(ColliderBase* a, ColliderBase* b, CollisionResult& result)
{
	// 当たり判定フラグを確認
	if (!a->GetJudgeFlg() || !b->GetJudgeFlg()) { return false; }

	const COLLIDER_SHAPE aShape = a->GetShape();
	const COLLIDER_SHAPE bShape = b->GetShape();

	if (aShape == COLLIDER_SHAPE::None || bShape == COLLIDER_SHAPE::None) { return false; }

	// 同一グループに属する同一タグ同士は判定しない
	if (a->GetTag() == b->GetTag()) {
		if (TAG_TO_GROUP_LIST.contains(a->GetTag())) { return false; }
	}

	// AABBによる広域判定
	ColliderBase::AABB aAABB = a->GetAABB();
	ColliderBase::AABB bAABB = b->GetAABB();

	// Sphere・Capsule×Boxは、移動前の範囲も含めて判定する
	// 現在位置だけで除外すると、高速移動時に薄い壁をすり抜ける
	if (IsSweptBoxPair(aShape, bShape)) {
		ExpandSweptAABB(a, aAABB);
		ExpandSweptAABB(b, bAABB);
	}

	// AABBが重なっていなければ判定しない
	if (!OverlapsAABB(aAABB, bAABB)) { return false; }

	// 衝突情報を初期化
	result = CollisionResult();

	// 法線の向きを反転させるラムダ関数
	auto ReverseNormal = [&result](bool isHit) -> bool {
		if (isHit) { result.normal = -result.normal; }
		return isHit;
		};

#pragma region 形状を判別して適切な関数にて判定を行う

	// 同じ形状同士は形状の種類だけで振り分ける
	if (aShape == bShape) {
		switch (aShape) {
		case COLLIDER_SHAPE::Line:
			return LineToLine(ColliderCast<LineCollider>(a), ColliderCast<LineCollider>(b), result);
		case COLLIDER_SHAPE::Sphere:
			return SphereToSphere(ColliderCast<SphereCollider>(a), ColliderCast<SphereCollider>(b), result);
		case COLLIDER_SHAPE::Capsule:
			return CapsuleToCapsule(ColliderCast<CapsuleCollider>(a), ColliderCast<CapsuleCollider>(b), result);
		case COLLIDER_SHAPE::Box:
			return BoxToBox(ColliderCast<BoxCollider>(a), ColliderCast<BoxCollider>(b), result);
		case COLLIDER_SHAPE::Mesh:
			return MeshToMesh(ColliderCast<MeshCollider>(a), ColliderCast<MeshCollider>(b), result);
		case COLLIDER_SHAPE::XzCircle:
			return XZCircleToXZCircle(ColliderCast<XZCircleCollider>(a), ColliderCast<XZCircleCollider>(b), result);
		default:
			return false;
		}
	}

	// 線分×球体（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Line && bShape == COLLIDER_SHAPE::Sphere) ||
		(aShape == COLLIDER_SHAPE::Sphere && bShape == COLLIDER_SHAPE::Line)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::Sphere);
		auto* first = ColliderCast<LineCollider>(reversed ? b : a);
		auto* second = ColliderCast<SphereCollider>(reversed ? a : b);
		const bool isHit = LineToSphere(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// 線分×カプセル（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Line && bShape == COLLIDER_SHAPE::Capsule) ||
		(aShape == COLLIDER_SHAPE::Capsule && bShape == COLLIDER_SHAPE::Line)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::Capsule);
		auto* first = ColliderCast<LineCollider>(reversed ? b : a);
		auto* second = ColliderCast<CapsuleCollider>(reversed ? a : b);
		const bool isHit = LineToCapsule(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// 線分×ボックス（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Line && bShape == COLLIDER_SHAPE::Box) ||
		(aShape == COLLIDER_SHAPE::Box && bShape == COLLIDER_SHAPE::Line)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::Box);
		auto* first = ColliderCast<LineCollider>(reversed ? b : a);
		auto* second = ColliderCast<BoxCollider>(reversed ? a : b);
		const bool isHit = LineToBox(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// 線分×メッシュ（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Line && bShape == COLLIDER_SHAPE::Mesh) ||
		(aShape == COLLIDER_SHAPE::Mesh && bShape == COLLIDER_SHAPE::Line)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::Mesh);
		auto* first = ColliderCast<LineCollider>(reversed ? b : a);
		auto* second = ColliderCast<MeshCollider>(reversed ? a : b);
		const bool isHit = LineToMesh(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// 球体×カプセル（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Sphere && bShape == COLLIDER_SHAPE::Capsule) ||
		(aShape == COLLIDER_SHAPE::Capsule && bShape == COLLIDER_SHAPE::Sphere)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::Capsule);
		auto* first = ColliderCast<SphereCollider>(reversed ? b : a);
		auto* second = ColliderCast<CapsuleCollider>(reversed ? a : b);
		const bool isHit = SphereToCapsule(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// 球体×ボックス（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Sphere && bShape == COLLIDER_SHAPE::Box) ||
		(aShape == COLLIDER_SHAPE::Box && bShape == COLLIDER_SHAPE::Sphere)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::Box);
		auto* first = ColliderCast<SphereCollider>(reversed ? b : a);
		auto* second = ColliderCast<BoxCollider>(reversed ? a : b);
		const bool isHit = SphereToBox(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// 球体×メッシュ（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Sphere && bShape == COLLIDER_SHAPE::Mesh) ||
		(aShape == COLLIDER_SHAPE::Mesh && bShape == COLLIDER_SHAPE::Sphere)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::Mesh);
		auto* first = ColliderCast<SphereCollider>(reversed ? b : a);
		auto* second = ColliderCast<MeshCollider>(reversed ? a : b);
		const bool isHit = SphereToMesh(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// 球体×XZ円（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Sphere && bShape == COLLIDER_SHAPE::XzCircle) ||
		(aShape == COLLIDER_SHAPE::XzCircle && bShape == COLLIDER_SHAPE::Sphere)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::XzCircle);
		auto* first = ColliderCast<SphereCollider>(reversed ? b : a);
		auto* second = ColliderCast<XZCircleCollider>(reversed ? a : b);
		const bool isHit = SphereToXZCircle(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// カプセル×ボックス（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Capsule && bShape == COLLIDER_SHAPE::Box) ||
		(aShape == COLLIDER_SHAPE::Box && bShape == COLLIDER_SHAPE::Capsule)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::Box);
		auto* first = ColliderCast<CapsuleCollider>(reversed ? b : a);
		auto* second = ColliderCast<BoxCollider>(reversed ? a : b);
		const bool isHit = CapsuleToBox(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// カプセル×メッシュ（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Capsule && bShape == COLLIDER_SHAPE::Mesh) ||
		(aShape == COLLIDER_SHAPE::Mesh && bShape == COLLIDER_SHAPE::Capsule)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::Mesh);
		auto* first = ColliderCast<CapsuleCollider>(reversed ? b : a);
		auto* second = ColliderCast<MeshCollider>(reversed ? a : b);
		const bool isHit = CapsuleToMesh(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// カプセル×XZ円（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Capsule && bShape == COLLIDER_SHAPE::XzCircle) ||
		(aShape == COLLIDER_SHAPE::XzCircle && bShape == COLLIDER_SHAPE::Capsule)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::XzCircle);
		auto* first = ColliderCast<CapsuleCollider>(reversed ? b : a);
		auto* second = ColliderCast<XZCircleCollider>(reversed ? a : b);
		const bool isHit = CapsuleToXZCircle(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

	// ボックス×メッシュ（逆順の場合は法線を反転）
	if ((aShape == COLLIDER_SHAPE::Box && bShape == COLLIDER_SHAPE::Mesh) ||
		(aShape == COLLIDER_SHAPE::Mesh && bShape == COLLIDER_SHAPE::Box)) {
		const bool reversed = (aShape == COLLIDER_SHAPE::Mesh);
		auto* first = ColliderCast<BoxCollider>(reversed ? b : a);
		auto* second = ColliderCast<MeshCollider>(reversed ? a : b);
		const bool isHit = BoxToMesh(first, second, result);
		return reversed ? ReverseNormal(isHit) : isHit;
	}

#pragma endregion

	return false;
}


#pragma region 各形状の実判定

// 線分×線分
bool CollisionManager::LineToLine(LineCollider* a, LineCollider* b, CollisionResult& result)
{
	return false;
}

// 球体×球体
bool CollisionManager::SphereToSphere(SphereCollider* a, SphereCollider* b, CollisionResult& result)
{
#pragma region 必要情報を求める

	// 接触点の計算に使用する中心座標を取得
	const Vector3 aPos = a->GetPos();
	const Vector3 bPos = b->GetPos();

	// bからaへ向かうベクトル
	Vector3 normal = aPos - bPos;

	// 中心間距離の2乗
	const float distanceSq = normal.LengthSq();

	// 半径
	const float aRadius = a->GetRadius();
	const float bRadius = b->GetRadius();

	// 半径の合計
	const float radiusSum = aRadius + bRadius;

#pragma endregion

#pragma region 衝突判定

	// 中心間距離が半径の合計より大きければ衝突していない
	if (distanceSq > radiusSum * radiusSum) { return false; }

#pragma endregion

#pragma region 衝突方向を求める

	// 球の中心が完全に一致している場合は、
	// 中心座標から衝突方向を求められないため代替方向を使用する
	if (distanceSq <= 0.000001f) {

		// 第一候補：Aの移動方向
		const Vector3 velocity = a->GetActor()->GetTrans().Velocity();

		// 第一候補がゼロベクトルなら、第二候補：Y軸方向を使用する
		normal = (velocity.LengthSq() > 0.000001f) ? velocity.Normalized() : Vector3::Yonly(1.0f);
	}
	else { normal.Normalize(); }

#pragma endregion

#pragma region 衝突点を求める

	// Aの表面上で、Bに最も近い点
	const Vector3 pointOnA = aPos - normal * aRadius;

	// Bの表面上で、Aに最も近い点
	const Vector3 pointOnB = bPos + normal * bRadius;

	// 2つの表面点の中間を衝突点とする
	result.point = (pointOnA + pointOnB) * 0.5f;

#pragma endregion

#pragma region 衝突情報を設定

	const float distance =
		std::sqrt((std::max)(distanceSq, 0.0f));

	const float overlap =
		radiusSum - distance;

	SetContactNormalAndDepth(result, normal, overlap);


#pragma endregion

	return true;
}

// カプセル×カプセル
bool CollisionManager::CapsuleToCapsule(CapsuleCollider* a, CapsuleCollider* b, CollisionResult& result)
{
#pragma region 必要情報を取得
	// 線分の 始点/終点 
	const Vector3 aStartPos = a->GetStartPos(), aEndPos = a->GetEndPos();
	const Vector3 bStartPos = b->GetStartPos(), bEndPos = b->GetEndPos();

	// 半径
	const float   aRadius = a->GetRadius(), bRadius = b->GetRadius();
#pragma endregion

#pragma region 衝突判定（お互いの線分上における最近点を求めてその２点間の距離をはかって 未衝突なら終了）
	// 線分同士の最近接点を求める ～～～～～～～～～～～

	// ここに最近接点が入る
	Vector3 pa = {}, pb = {};

	// Aの方向ベクトル
	Vector3 u = aEndPos - aStartPos;
	// Bの方向ベクトル
	Vector3 v = bEndPos - bStartPos;

	// Bの始点からAの始点までのベクトル
	Vector3 w = aStartPos - bStartPos;

	float aLen = u.LengthSq();
	float bLen = v.LengthSq();
	float ab = u.Dot(v);
	float aw = u.Dot(w);
	float bw = v.Dot(w);

	float denom = aLen * bLen - ab * ab;
	float s, t;

	if (denom < 1e-6f) {
		// 線分がほぼ平行 → 片方に合わせて計算
		s = 0.0f;
		t = bw / bLen;
	}
	else {
		s = (ab * bw - bLen * aw) / denom;
		t = (aLen * bw - ab * aw) / denom;
	}

	// 線分内に clamp
	s = std::clamp(s, 0.0f, 1.0f);
	t = std::clamp(t, 0.0f, 1.0f);

	pa = aStartPos + u * s;  // A線分上の最近点
	pb = bStartPos + v * t;  // B線分上の最近点

	// 距離計算
	Vector3 normal = pa - pb;
	float distSq = normal.LengthSq();
	float radSum = aRadius + bRadius;

	// 
	if (distSq >= radSum * radSum) { return false; }

#pragma endregion

#pragma region 衝突方向を求める

	// 距離
	float dist = std::sqrt(distSq);

	// 衝突方向
	Vector3 collisionNormal;

	// B側からA側へ向かう方向
	if (dist > 1e-6f) { collisionNormal = normal / dist; }
	// 最近点が完全に同じ位置だった場合
	else {
		// 第一候補：Aの移動方向
		Vector3 velocity = -a->GetActor()->GetTrans().Velocity();

		// 第一候補がゼロベクトルなら、第二候補：Y軸方向を使用する
		collisionNormal = (velocity.LengthSq() > 1e-6f) ? velocity.Normalized() : Vector3::Yonly(1.0f);
	}

#pragma endregion

#pragma region 衝突点を求める

	// Aカプセル表面の、B側にある点
	const Vector3 pointOnA = pa - collisionNormal * aRadius;

	// Bカプセル表面の、A側にある点
	const Vector3 pointOnB = pb + collisionNormal * bRadius;

	// 両方の表面点の中間を衝突座標とする
	result.point = (pointOnA + pointOnB) * 0.5f;

#pragma endregion

#pragma region 衝突情報を設定
	const float overlap = radSum - dist;
	SetContactNormalAndDepth(result, collisionNormal, overlap);

#pragma endregion

	// 当たった
	return true;
}

// ボックス×ボックス

// Box同士の接触面をクリッピングして最大4接触点を求める。
// 複数の接触点を求め、接触応答の回転計算に使用する。
namespace
{
    struct BoxContactPoint
    {
        Vector3 position;
		float depth = 0.0f;
        float normalImpulse = 0.0f;
        float tangentImpulse = 0.0f;
    };

    struct BoxContactManifold
    {
        std::array<BoxContactPoint, 4> points;
        int count = 0;
    };

	struct BoxFace
	{
		Vector3 center, normal, u, v;
		float hu = 0.0f, hv = 0.0f;
	};

    BoxFace MakeBoxFace(const BoxCollider* box, const Vector3& direction)
    {
        const auto axes = box->GetAxes();
        const Vector3 h = box->GetHalfSize();
        const float half[3] = { h.x, h.y, h.z };
        int axis = 0;
        for (int i = 1; i < 3; ++i) {
            if (std::abs(direction.Dot(axes[i])) > std::abs(direction.Dot(axes[axis]))) { axis = i; }
        }
        const float sign = direction.Dot(axes[axis]) >= 0.0f ? 1.0f : -1.0f;
        const int iu = (axis + 1) % 3, iv = (axis + 2) % 3;
        BoxFace face;
        face.normal = axes[axis] * sign;
        face.center = box->GetPos() + face.normal * half[axis];
        face.u = axes[iu]; face.v = axes[iv];
        face.hu = half[iu]; face.hv = half[iv];
        return face;
    }

    // dot(p - origin, axis) <= limit の半空間で多角形を切り取る。
    void ClipBoxPolygon(std::vector<Vector3>& polygon, const Vector3& origin,
        const Vector3& axis, float limit)
    {
        if (polygon.empty()) { return; }
        std::vector<Vector3> clipped;
        clipped.reserve(polygon.size() + 2);
        Vector3 prev = polygon.back();
        float dp = (prev - origin).Dot(axis) - limit;
        for (const Vector3& cur : polygon) {
            const float dc = (cur - origin).Dot(axis) - limit;
            if ((dp <= 0.0f) != (dc <= 0.0f)) {
                const float t = dp / (dp - dc);
                clipped.push_back(prev + (cur - prev) * t);
            }
            if (dc <= 0.0f) { clipped.push_back(cur); }
            prev = cur; dp = dc;
        }
        polygon.swap(clipped);
    }

    BoxContactManifold MakeBoxManifold(const BoxCollider* a, const BoxCollider* b,
        const Vector3& normal, float penetration)
    {
        BoxContactManifold manifold;
        const auto axA = a->GetAxes(), axB = b->GetAxes();
        float alignmentA = 0.0f, alignmentB = 0.0f;
        for (int i = 0; i < 3; ++i) {
            alignmentA = (std::max)(alignmentA, std::abs(normal.Dot(axA[i])));
            alignmentB = (std::max)(alignmentB, std::abs(normal.Dot(axB[i])));
        }
        // SATの最小軸が辺×辺の軸なら、面のクリッピングは適切でない。
        if ((std::max)(alignmentA, alignmentB) < 0.94f) {
            manifold.points[0].position = (a->GetSupportPoint(-normal) + b->GetSupportPoint(normal)) * 0.5f;
            manifold.points[0].depth = penetration;
            manifold.count = 1;
            return manifold;
        }
        const bool refIsA = alignmentA >= alignmentB;
        const BoxCollider* reference = refIsA ? a : b;
        const BoxCollider* incident = refIsA ? b : a;
        const Vector3 referenceDirection = refIsA ? -normal : normal;
        const BoxFace ref = MakeBoxFace(reference, referenceDirection);
        const BoxFace inc = MakeBoxFace(incident, -ref.normal);
        std::vector<Vector3> polygon = {
            inc.center + inc.u * inc.hu + inc.v * inc.hv,
            inc.center - inc.u * inc.hu + inc.v * inc.hv,
            inc.center - inc.u * inc.hu - inc.v * inc.hv,
            inc.center + inc.u * inc.hu - inc.v * inc.hv
        };
        ClipBoxPolygon(polygon, ref.center, ref.u, ref.hu);
        ClipBoxPolygon(polygon, ref.center, -ref.u, ref.hu);
        ClipBoxPolygon(polygon, ref.center, ref.v, ref.hv);
        ClipBoxPolygon(polygon, ref.center, -ref.v, ref.hv);
        for (const Vector3& point : polygon) {
            const float depth = -(point - ref.center).Dot(ref.normal);
            if (depth < -0.02f) { continue; }
            if (manifold.count >= 4) { break; }
            BoxContactPoint& cp = manifold.points[manifold.count++];
            cp.position = point + ref.normal * (depth * 0.5f);
            cp.depth = (std::max)(0.0f, depth);
        }
        if (manifold.count == 0) {
            manifold.points[0].position = (a->GetSupportPoint(-normal) + b->GetSupportPoint(normal)) * 0.5f;
            manifold.points[0].depth = penetration;
            manifold.count = 1;
        }
        return manifold;
    }

}

bool CollisionManager::BoxToBox(BoxCollider* a, BoxCollider* b, CollisionResult& result)
{
#pragma region 必要情報を取得

	// Bの中心からAの中心へ向かうベクトル
	const Vector3 centerDifference = a->GetPos() - b->GetPos();

	// 各ボックスの回転後の3軸
	const std::array<Vector3, 3> aAxes = a->GetAxes();

	const std::array<Vector3, 3> bAxes = b->GetAxes();

	// 最小めり込み量
	float minimumOverlap = FLT_MAX;

	// 最小めり込み軸
	Vector3 minimumAxis(0.0f, 1.0f, 0.0f);

#pragma endregion

#pragma region 衝突判定

	// 1軸分のSAT判定をするラムダ関数
	auto CheckAxis = [&](const Vector3& testAxis) -> bool {
		const float axisLengthSq = testAxis.LengthSq();

		// 外積がほぼゼロの場合、分離軸として使えないので無視
		if (axisLengthSq < 1e-8f) { return true; }

		// SAT判定用に正規化
		const Vector3 axis = testAxis / std::sqrt(axisLengthSq);

		// 各ボックスをこの軸に投影した半径
		const float aProjectionRadius = a->GetProjectionRadius(axis);

		const float bProjectionRadius = b->GetProjectionRadius(axis);

		// 軸上での中心間距離
		const float centerDistance = std::abs(centerDifference.Dot(axis));

		// めり込み量
		const float overlap = aProjectionRadius + bProjectionRadius - centerDistance;

		// この軸で離れているなら衝突していない
		if (overlap < 0.0f) { return false; }

		// 最もめり込みが浅い軸を保存
		if (overlap < minimumOverlap) {
			minimumOverlap = overlap;
			minimumAxis = axis;

			// BからAへ向く方向に統一
			if (centerDifference.Dot(minimumAxis) < 0.0f) { minimumAxis = -minimumAxis; }
		}

		return true;
		};


	// A側の3軸を判定
	for (const Vector3& axis : aAxes) {
		if (!CheckAxis(axis)) { return false; }
	}

	// B側の3軸を判定
	for (const Vector3& axis : bAxes) {
		if (!CheckAxis(axis)) { return false; }
	}

	// 各軸の外積9本を判定
	for (const Vector3& aAxis : aAxes) {
		for (const Vector3& bAxis : bAxes) {
			if (!CheckAxis(aAxis.Cross(bAxis))) { return false; }
		}
	}
#pragma endregion

#pragma region 衝突点を計算

	// Aの表面上でB側にある点
	const Vector3 pointOnA = a->GetSupportPoint(-minimumAxis);

	// Bの表面上でA側にある点
	const Vector3 pointOnB = b->GetSupportPoint(minimumAxis);

	// 両方の接触候補点の中間を衝突点とする
	result.point = (pointOnA + pointOnB) * 0.5f;

#pragma endregion

	SetContactNormalAndDepth(result, minimumAxis, minimumOverlap);

	// 接触応答が有効なら接触点を登録
	if (NeedContactResponse(a, b)) {
        // 接触点をBox面中心同士の中点で近似し、双方に回転を与える。

        BoxContactManifold manifold = MakeBoxManifold(a, b, minimumAxis, minimumOverlap);
        if (manifold.count > 0) { result.point = manifold.points[0].position; }
        ContactManifold contact;
        contact.a = a;
        contact.b = b;
        contact.normal = minimumAxis;
        contact.penetration = minimumOverlap;
        for (int i = 0; i < manifold.count; ++i) {
            contact.AddPoint(manifold.points[i].position, manifold.points[i].depth);
        }
        // 速度拘束と位置拘束はAddContactで一括登録する。
        ContactSolver::AddContact(contact);
	}

	return true;
}

// メッシュ×メッシュ
bool CollisionManager::MeshToMesh(MeshCollider* a, MeshCollider* b, CollisionResult& result)
{
	// 静的メッシュ同士はCollisionManagerの組み合わせ段階で通常判定されない。
	// 動的メッシュ同士が必要な場合だけ候補三角形をBVHで絞り込む。
	for (unsigned int aIndex = 0; aIndex < a->GetTriangleNum(); aIndex++) {
		Vector3 a0, a1, a2, aNormal;

		a->GetWorldTriangle(aIndex, a0, a1, a2, aNormal);

		const ColliderBase::AABB aTriangleAABB = MakeTriangleAABB(a0, a1, a2);

		b->QueryTriangles(aTriangleAABB, meshQueryBufferA);

		for (unsigned int bIndex : meshQueryBufferA)
		{
			Vector3 b0, b1, b2, bNormal;

			b->GetWorldTriangle(bIndex, b0, b1, b2, bNormal);

			const Vector3 aStarts[3] = { a0,a1,a2 };

			const Vector3 aEnds[3] = { a1,a2,a0 };

			for (int i = 0; i < 3; ++i) {
				float hitRate = 0.0f;
				Vector3 hitPoint;
				Vector3 hitNormal;

				if (SegmentToTriangle(aStarts[i], aEnds[i], b0, b1, b2, hitRate, hitPoint, hitNormal))
				{
					result.point = hitPoint;
					result.normal = hitNormal;

					if ((a->GetPos() - b->GetPos()).Dot(result.normal) < 0.0f) { result.normal = -result.normal; }

					result.penetration = 0.0f;
					return true;
				}
			}

			const Vector3 bStarts[3] = { b0,b1,b2 };

			const Vector3 bEnds[3] = { b1,b2,b0 };

			for (int i = 0; i < 3; ++i) {
				float hitRate = 0.0f;
				Vector3 hitPoint;
				Vector3 hitNormal;

				if (SegmentToTriangle(bStarts[i], bEnds[i], a0, a1, a2, hitRate, hitPoint, hitNormal)) {
					result.point = hitPoint;
					result.normal = aNormal;

					if ((a->GetPos() - b->GetPos()).Dot(result.normal) < 0.0f) { result.normal = -result.normal; }

					result.penetration = 0.0f;

					return true;
				}
			}
		}
	}

	return false;
}

// XZ平面上の円形×XZ平面上の円形
bool CollisionManager::XZCircleToXZCircle(XZCircleCollider* a, XZCircleCollider* b, CollisionResult& result)
{
#pragma region 必要情報を取得
	// XZ平面上のベクトルを取得
	Vector2 vec = a->GetPos().ToVector2XZ() - b->GetPos().ToVector2XZ();

	// 半径の合計
	float radius = a->GetRadius() + b->GetRadius();

	// Y方向の長さを取得
	float aYLength = a->GetYLength(), bYLength = b->GetYLength();
#pragma endregion

#pragma region 衝突判定

	// Y方向の判定
	if (abs(a->GetPos().y - b->GetPos().y) > (aYLength + bYLength) * 0.5f) { return false; }

	// XZ平面上の判定
	if (vec.LengthSq() > radius * radius) { return false; }

#pragma endregion

#pragma region 衝突確定：必要なら位置補正
	if (NeedContactResponse(a, b)) {
		// めり込み量
		float overlap = radius - vec.Length();

		// ベクトルを正規化
		vec.Normalize();
		
		// 位置補正を登録
		QueuePositionCorrection(a, b, Vector3::XZonly(vec.x, vec.y), overlap);
	}
#pragma endregion

	return true;
}

// 線分×球体
bool CollisionManager::LineToSphere(LineCollider* line, SphereCollider* sphere, CollisionResult& result)
{
#pragma region 必要情報を取得

	// sphere（球体）
	const Vector3 spherePos = sphere->GetPos();
	const float radius = sphere->GetRadius();

#pragma endregion

#pragma region 衝突判定（Sphere中心から線分最近点までの距離）

	// 球体の中心に最も近い線分上の点
	const Vector3 closestPoint = line->ClosestPoint(spherePos);

	// 最近点から球体中心までのベクトル
	const Vector3 difference = spherePos - closestPoint;

	const float distanceSq = difference.LengthSq();

	if (distanceSq > radius * radius) { return false; }

#pragma endregion

#pragma region 衝突点を設定

	// 線分上にある、球体中心との最近点
	result.point = closestPoint;

	const float distance = std::sqrt((std::max)(distanceSq, 0.0f));
	SetContactNormalAndDepth(result, (distance > 0.000001f) ? -difference / distance : -line->GetDirection().Normalized(), radius - distance);

#pragma endregion

#pragma region 衝突確定：必要に応じて位置補正を登録

	if (NeedContactResponse(line, sphere))
	{
		// 現在も衝突しているか調べる
		auto IsColliding = [&]() -> bool {
			const Vector3 currentSpherePos = sphere->GetPos();

			const Vector3 currentClosestPoint = line->ClosestPoint(currentSpherePos);

			const float currentDistanceSq = (currentSpherePos - currentClosestPoint).LengthSq();

			return currentDistanceSq <= radius * radius;
			};

		// 1回の押し出し量
		const float onePush = 5.0f;

		// 線分の進行方向に押し出す
		const Vector3 pushVector = line->GetDirection().Normalized() * onePush;

		// 当たらなくなるまで少しずつ押し出す
		// 無限ループ防止のため最大50回
		for (unsigned char i = 0; i < 50; ++i) {
			QueueOneSidePositionCorrection(line, sphere, pushVector);

			if (!IsColliding()) { break; }
		}
	}

#pragma endregion

	return true;
}

// 線分×カプセル
bool CollisionManager::LineToCapsule(LineCollider* line, CapsuleCollider* capsule, CollisionResult& result)
{
#pragma region 必要情報を取得

	// 判定対象の線分
	const Vector3 lineStart = line->GetStartPos();
	const Vector3 lineEnd = line->GetEndPos();

	// カプセルの中心軸
	const Vector3 capsuleStart = capsule->GetStartPos();
	const Vector3 capsuleEnd = capsule->GetEndPos();

	const float radius = capsule->GetRadius();

#pragma endregion

#pragma region 2本の線分間の最近点を取得

	// 線分1と線分2の最近点ペアを取得する
	auto ClosestPointsBetweenSegments = [](const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, Vector3& closest1, Vector3& closest2) {
		constexpr float epsilon = 1e-8f;

		const Vector3 d1 = q1 - p1;
		const Vector3 d2 = q2 - p2;
		const Vector3 r = p1 - p2;

		const float a = d1.Dot(d1);
		const float e = d2.Dot(d2);
		const float f = d2.Dot(r);

		float s = 0.0f;
		float t = 0.0f;

		// 両方とも長さがほぼ0
		if (a <= epsilon && e <= epsilon) { closest1 = p1; closest2 = p2; return; }

		// line側だけ長さがほぼ0
		if (a <= epsilon) { s = 0.0f; t = std::clamp(f / e, 0.0f, 1.0f); }
		else {
			const float c = d1.Dot(r);

			// capsule中心軸側だけ長さがほぼ0
			if (e <= epsilon) {
				t = 0.0f;
				s = std::clamp(-c / a, 0.0f, 1.0f);
			}
			else {
				const float b = d1.Dot(d2);
				const float denominator = a * e - b * b;

				// 平行でなければline側の割合を計算
				if (std::abs(denominator) > epsilon) { s = std::clamp((b * f - c * e) / denominator, 0.0f, 1.0f); }
				// ほぼ平行
				else { s = 0.0f; }

				// capsule中心軸側の割合
				t = (b * s + f) / e;

				// capsule側の範囲から外れた場合、
				// 端点に固定してline側を再計算
				if (t < 0.0f) {
					t = 0.0f;
					s = std::clamp(-c / a, 0.0f, 1.0f);
				}
				else if (t > 1.0f) {
					t = 1.0f;
					s = std::clamp((b - c) / a, 0.0f, 1.0f);
				}
			}
		}

		closest1 = p1 + d1 * s;
		closest2 = p2 + d2 * t;
		};

	Vector3 lineClosest;
	Vector3 capsuleAxisClosest;

	ClosestPointsBetweenSegments(lineStart, lineEnd, capsuleStart, capsuleEnd, lineClosest, capsuleAxisClosest);

#pragma endregion

#pragma region 衝突判定

	const Vector3 difference = lineClosest - capsuleAxisClosest;

	const float distanceSq = difference.LengthSq();

	if (distanceSq > radius * radius) { return false; }

#pragma endregion

#pragma region 衝突位置を設定

	// 線分上にある、カプセル中心軸との最近点
	result.point = lineClosest;

	const float distance = std::sqrt((std::max)(distanceSq, 0.0f));
	SetContactNormalAndDepth(result, (distance > 0.000001f) ? difference / distance : -line->GetDirection().Normalized(), radius - distance);

#pragma endregion

#pragma region 衝突確定：必要に応じて位置補正を登録

	if (NeedContactResponse(line, capsule))	{

		// 現在も衝突しているか判定
		auto IsColliding = [&]() -> bool {
			Vector3 currentLineClosest;
			Vector3 currentCapsuleClosest;

			ClosestPointsBetweenSegments(line->GetStartPos(), line->GetEndPos(), capsule->GetStartPos(), capsule->GetEndPos(), currentLineClosest, currentCapsuleClosest);

			return (currentLineClosest - currentCapsuleClosest).LengthSq() <= radius * radius;
			};

		// 1回で押し出す量
		const float step = 5.0f;

		// 線分の進行方向へ押し出す
		const Vector3 pushVector = line->GetDirection().Normalized() * step;

		// 当たらなくなるまで細かく押し出す
		for (int i = 0; i < 50; ++i) {
			QueueOneSidePositionCorrection(line, capsule, pushVector);

			if (!IsColliding()) { break; }
		}
	}

#pragma endregion

	return true;
}

// 線分×ボックス
bool CollisionManager::LineToBox(LineCollider* line, BoxCollider* box, CollisionResult& result)
{
#pragma region 必要情報を取得

	const Vector3 lineStart = line->GetStartPos();
	const Vector3 lineEnd = line->GetEndPos();

	// 線分ベクトル
	const Vector3 lineVector = lineEnd - lineStart;

	const Vector3 pushDir = line->GetDirection().Normalized();

	const Vector3 boxPos = box->GetPos();

	const Vector3 half = box->GetHalfSize();

	// 回転後のBoxの3軸
	const std::array<Vector3, 3> boxAxes = box->GetAxes();

#pragma endregion

#pragma region 線分をBoxのローカル座標へ変換

	// Box中心から見た線分開始・終了位置
	const Vector3 startDifference = lineStart - boxPos;

	const Vector3 endDifference = lineEnd - boxPos;

	// Boxの各軸との内積を取ることで、
	// 回転していないBoxの座標系へ変換する
	const Vector3 localStart(
		startDifference.Dot(boxAxes[0]),
		startDifference.Dot(boxAxes[1]),
		startDifference.Dot(boxAxes[2])
	);

	const Vector3 localEnd(
		endDifference.Dot(boxAxes[0]),
		endDifference.Dot(boxAxes[1]),
		endDifference.Dot(boxAxes[2])
	);

	const Vector3 localDirection = localEnd - localStart;

#pragma endregion

#pragma region 線分とローカルAABBの交差判定

	// 線分上の位置
	// 0.0f = 開始点
	// 1.0f = 終了点
	float enterTime = 0.0f;
	float exitTime = 1.0f;

	// 衝突したBox面の法線
	Vector3 localHitNormal;

	auto CheckAxis =
		[&](float start, float direction, float minValue, float maxValue, const Vector3& negativeNormal, const Vector3& positiveNormal)		-> bool
		{
			constexpr float epsilon = 1e-8f;

			// 線分がこの軸方向へほぼ移動していない
			if (std::abs(direction) < epsilon) {
				// この軸のBox範囲外にいるなら交差しない
				if (start < minValue || start > maxValue) { return false; }

				return true;
			}

			float time1 = (minValue - start) / direction;

			float time2 = (maxValue - start) / direction;

			Vector3 normal1 = negativeNormal;
			Vector3 normal2 = positiveNormal;

			// 入る時刻と出る時刻を正しい順番にする
			if (time1 > time2) { std::swap(time1, time2); std::swap(normal1, normal2); }

			// より遅い侵入時刻を採用
			if (time1 > enterTime) { enterTime = time1; localHitNormal = normal1; }

			// より早い退出時刻を採用
			exitTime = (std::min)(exitTime, time2);

			// 入る時刻が出る時刻を越えたら交差しない
			if (enterTime > exitTime) { return false; }

			return true;
		};

	// X軸
	if (!CheckAxis(localStart.x, localDirection.x, -half.x, half.x, Vector3::Xonly(-1.0f), Vector3::Xonly(1.0f))) { return false; }

	// Y軸
	if (!CheckAxis(localStart.y, localDirection.y, -half.y, half.y, Vector3::Yonly(-1.0f), Vector3::Yonly(1.0f))) { return false; }

	// Z軸
	if (!CheckAxis(localStart.z, localDirection.z, -half.z, half.z, Vector3::Zonly(-1.0f), Vector3::Zonly(1.0f))) { return false; }

	// 線分の範囲外で交差している
	if (exitTime < 0.0f || enterTime > 1.0f) { return false; }

#pragma endregion

#pragma region 衝突点を計算

	// 線分がBoxへ入った位置
	const float hitTime = (std::max)(enterTime, 0.0f);

	result.point = lineStart + lineVector * hitTime;

	// 衝突面のローカル法線をワールド空間へ変換
	Vector3 worldHitNormal = boxAxes[0] * localHitNormal.x + boxAxes[1] * localHitNormal.y + boxAxes[2] * localHitNormal.z;
	worldHitNormal.Normalize();

	const float startDepth = -(lineStart - result.point).Dot(worldHitNormal);
	const float endDepth = -(lineEnd - result.point).Dot(worldHitNormal);
	float overlap = (std::max)(startDepth, endDepth);

	if (overlap < 0.0f) { overlap = 0.0f; }
	overlap += 0.001f;

	SetContactNormalAndDepth(result, worldHitNormal, overlap);

#pragma endregion

#pragma region 衝突確定：必要に応じて位置補正を登録

	if (NeedContactResponse(line, box)) {

		// Boxの接触面法線方向へ位置補正を登録
		QueueOneSidePositionCorrection(line, box, worldHitNormal * overlap);
	}

#pragma endregion

	return true;
}

// 線分×メッシュ
bool CollisionManager::LineToMesh(LineCollider* line, MeshCollider* mesh, CollisionResult& result)
{
	const Vector3 start = line->GetStartPos();
	const Vector3 end = line->GetEndPos();

	mesh->QueryTriangles(line->GetAABB(), meshQueryBufferA);

	float nearestRate = FLT_MAX;
	bool isHit = false;

	for (unsigned int triangleIndex : meshQueryBufferA) {
		Vector3 p0, p1, p2, triangleNormal;

		mesh->GetWorldTriangle(triangleIndex, p0, p1, p2, triangleNormal);

		float hitRate = 0.0f;
		Vector3 hitPoint;
		Vector3 hitNormal;

		if (!SegmentToTriangle(start, end, p0, p1, p2, hitRate, hitPoint, hitNormal)) { continue; }

		if (hitRate >= nearestRate) { continue; }

		nearestRate = hitRate;
		result.point = hitPoint;
		SetContactNormalAndDepth(result, -hitNormal, 0.0f);

		if (line->GetDirection().Dot(result.normal) > 0.0f) { result.normal = -result.normal; }

		isHit = true;
	}

	return isHit;
}

// 球体×カプセル
bool CollisionManager::SphereToCapsule(SphereCollider* sphere, CapsuleCollider* capsule, CollisionResult& result)
{
#pragma region 必要情報を取得

	// sphere（球体）
	const Vector3 C = sphere->GetPos();
	const float rS = sphere->GetRadius();

	// capsule（カプセル）
	const Vector3 A = capsule->GetStartPos();
	const Vector3 B = capsule->GetEndPos();
	const float rC = capsule->GetRadius();

#pragma endregion

#pragma region カプセル中心軸上の最近点を取得

	const Vector3 AB = B - A;
	const Vector3 AC = C - A;

	const float abLenSq = AB.LengthSq();

	float t = 0.0f;

	if (abLenSq > 1e-6f) {
		t = AC.Dot(AB) / abLenSq;
		t = std::clamp(t, 0.0f, 1.0f);
	}

	// 球中心から最も近いカプセル中心軸上の点
	const Vector3 Q = A + AB * t;

#pragma endregion

#pragma region 衝突判定

	// カプセル中心軸から球中心へ向かうベクトル
	Vector3 normal = C - Q;

	const float distSq = normal.LengthSq();

	const float radiusSum = rS + rC;

	if (distSq >= radiusSum * radiusSum) { return false; }

#pragma endregion

#pragma region 衝突点を計算

	constexpr float epsilon = 1e-6f;

	if (distSq > epsilon * epsilon) {
		// カプセル中心軸から球体側へ向かう単位ベクトル
		const Vector3 normalN = normal / std::sqrt(distSq);

		// カプセル表面上の接触候補点
		const Vector3 pointOnCapsule = Q + normalN * rC;

		// 球体表面上の接触候補点
		const Vector3 pointOnSphere = C - normalN * rS;

		// めり込み中でも偏りが出にくいよう、
		// 両表面点の中間を代表衝突点にする
		result.point = (pointOnCapsule + pointOnSphere) * 0.5f;
	}
	else {
		// 球中心がカプセル中心軸と完全に一致している場合、
		// 接触方向を一意に決められない
		Vector3 fallbackNormal = -sphere->GetActor()->GetTrans().Velocity();

		if (fallbackNormal.LengthSq() <= epsilon * epsilon) { fallbackNormal = Vector3::Yonly(1.0f); }

		fallbackNormal.Normalize();

		result.point = Q + fallbackNormal * rC;

		normal = fallbackNormal;
	}

#pragma endregion

#pragma region 衝突確定：必要なら位置補正

	float dist = std::sqrt((std::max)(distSq, 0.0f));

	if (dist < epsilon) {
			// 完全一致時は先ほど決めた予備方向を使用
			if (normal.LengthSq() <= epsilon * epsilon) {
				normal = -sphere->GetActor()->GetTrans().Velocity();

				if (normal.LengthSq() <= epsilon * epsilon) { normal = Vector3::Yonly(1.0f); }
			}

			dist = 0.0f;
		}

	const float overlap = radiusSum - dist;

	SetContactNormalAndDepth(result, normal.Normalized(), overlap);


#pragma endregion

	return true;
}

// 球体×ボックス
bool CollisionManager::SphereToBox(SphereCollider* sphere, BoxCollider* box, CollisionResult& result)
{
#pragma region 必要情報を取得

	const Vector3 spherePos = sphere->GetPos();

	const float radius = sphere->GetRadius();

	const Vector3 boxPos = box->GetPos();

	const Vector3 half = box->GetHalfSize();

	// 回転後のBoxのローカル3軸
	const std::array<Vector3, 3> boxAxes = box->GetAxes();

#pragma endregion

#pragma region 球中心をBoxローカル座標へ変換

	const Vector3 centerDiff = spherePos - boxPos;

	const Vector3 localSpherePos(
		centerDiff.Dot(boxAxes[0]),
		centerDiff.Dot(boxAxes[1]),
		centerDiff.Dot(boxAxes[2])
	);

#pragma endregion

#pragma region Box上の最近点を取得

	// Boxローカル空間での最近点
	Vector3 localNearest;

	localNearest.x = std::clamp(localSpherePos.x, -half.x, half.x);

	localNearest.y = std::clamp(localSpherePos.y, -half.y, half.y);

	localNearest.z = std::clamp(localSpherePos.z, -half.z, half.z);

	// 最近点をワールド座標へ戻す
	Vector3 nearest = boxPos + boxAxes[0] * localNearest.x + boxAxes[1] * localNearest.y + boxAxes[2] * localNearest.z;

#pragma endregion

#pragma region 衝突判定

	// Box表面の最近点から球中心へ向かうベクトル
	Vector3 normal = spherePos - nearest;

	const float distSq = normal.LengthSq();

	// 現在位置で貫通していても、移動経路上の最初の接触を優先する。
	if (SweptSphereBoxTOI(sphere, box, result)) { return true; }

	if (distSq > radius * radius) { return false; }

#pragma endregion

#pragma region 接触応答・位置補正方向・めり込み量を取得

	constexpr float epsilon = 1e-6f;

	Vector3 pushNormal;
	float overlap = 0.0f;

	if (distSq > epsilon * epsilon) {
		// 球中心がBoxの外側にある通常ケース
		const float dist = std::sqrt(distSq);

		pushNormal = normal / dist;

		overlap = radius - dist;

		// Box表面上の最近点を衝突点にする
		result.point = nearest;
	}
	else
	{
		// 球中心がBox内部、またはBox表面と完全一致しているケース

		// 各面までの距離
		const float distanceToXFace = half.x - std::abs(localSpherePos.x);

		const float distanceToYFace = half.y - std::abs(localSpherePos.y);

		const float distanceToZFace = half.z - std::abs(localSpherePos.z);

		Vector3 localPushNormal;
		Vector3 localSurfacePoint = localSpherePos;

		float distanceToSurface = 0.0f;

		// 最も近い面を選ぶ
		if (distanceToXFace <= distanceToYFace && distanceToXFace <= distanceToZFace) {

			const float sign = localSpherePos.x >= 0.0f ? 1.0f : -1.0f;

			localPushNormal = Vector3(sign, 0.0f, 0.0f);

			localSurfacePoint.x = sign * half.x;

			distanceToSurface = distanceToXFace;
		}
		else if (distanceToYFace <= distanceToXFace && distanceToYFace <= distanceToZFace) {

			const float sign = localSpherePos.y >= 0.0f ? 1.0f : -1.0f;

			localPushNormal = Vector3(0.0f, sign, 0.0f);

			localSurfacePoint.y = sign * half.y;

			distanceToSurface = distanceToYFace;
		}
		else
		{
			const float sign = localSpherePos.z >= 0.0f ? 1.0f : -1.0f;

			localPushNormal = Vector3(0.0f, 0.0f, sign);

			localSurfacePoint.z = sign * half.z;

			distanceToSurface = distanceToZFace;
		}

		// ローカル法線をワールド法線へ変換
		pushNormal = boxAxes[0] * localPushNormal.x + boxAxes[1] * localPushNormal.y + boxAxes[2] * localPushNormal.z;

		pushNormal.Normalize();

		// Box内部から外へ出す距離＋球の半径
		overlap = distanceToSurface + radius;

		// 選択した最寄り面上の点をワールド座標へ変換
		result.point = boxPos + boxAxes[0] * localSurfacePoint.x + boxAxes[1] * localSurfacePoint.y + boxAxes[2] * localSurfacePoint.z;
	}

#pragma endregion

#pragma region 衝突確定：必要に応じて位置補正を登録

	SetContactNormalAndDepth(result, pushNormal, (std::max)(overlap, 0.0f));


#pragma endregion

	return true;
}

// 球体×メッシュ
bool CollisionManager::SphereToMesh(SphereCollider* sphere, MeshCollider* mesh, CollisionResult& result)
{
	mesh->QueryTriangles(sphere->GetAABB(), meshQueryBufferA);

	const Vector3 spherePos = sphere->GetPos();

	const float radius = sphere->GetRadius();

	const float radiusSq = radius * radius;

	bool isHit = false;
	float maximumPenetration = -FLT_MAX;

	for (unsigned int triangleIndex : meshQueryBufferA) {
		Vector3 p0, p1, p2, triangleNormal;

		mesh->GetWorldTriangle(triangleIndex, p0, p1, p2, triangleNormal);

		const Vector3 closestPoint = ClosestPointOnTriangle(spherePos, p0, p1, p2);

		// ポリゴン法線方向のめり込み量を求める（面の表側を衝突可能側とする）
		if (triangleNormal.LengthSq() <= COLLISION_EPSILON) { continue; }
		const Vector3 normal = triangleNormal.Normalized();
		const float signedDistance = (spherePos - p0).Dot(normal);
		const float penetration = radius - signedDistance;
		if (penetration <= 0.0f) { continue; }

		if (penetration <= maximumPenetration) { continue; }

		maximumPenetration = penetration;
		result.point = closestPoint;
		SetContactNormalAndDepth(result, normal, penetration);
		isHit = true;
	}


	return isHit;
}

// 球体×XZ平面上の円形
bool CollisionManager::SphereToXZCircle(SphereCollider* sphere, XZCircleCollider* xzcircle, CollisionResult& result)
{
#pragma region 必要情報を取得

	// 球体の中心座標
	const Vector3 spherePos = sphere->GetPos();
	// 球体の半径
	const float sphereRadius = sphere->GetRadius();

	// XZCircleColliderの中心座標
	const Vector3 circlePos = xzcircle->GetPos();
	// XZCircleColliderの半径
	const float circleRadius = xzcircle->GetRadius();
	// XZCircleColliderのY方向の長さ
	const float yLength = xzcircle->GetYLength();


	// XZ平面上で、円の中心から球の中心へ向かうベクトル
	Vector2 vec = spherePos.ToVector2XZ() - circlePos.ToVector2XZ();

	// 半径の合計
	const float radiusSum = sphereRadius + circleRadius;

#pragma endregion

#pragma region Y方向の早期リターン

	// XZCircleColliderのY方向範囲
	const float circleMinY = circlePos.y - yLength;

	const float circleMaxY = circlePos.y + yLength;

	// SphereのY方向範囲
	const float sphereMinY = spherePos.y - sphereRadius;

	const float sphereMaxY = spherePos.y + sphereRadius;

	// Y方向で完全に離れている
	if (sphereMaxY < circleMinY || sphereMinY > circleMaxY) { return false; }

#pragma endregion

#pragma region XZ平面上で衝突判定

	const float distanceSq = vec.LengthSq();

	if (distanceSq > radiusSum * radiusSum) { return false; }

#pragma endregion

#pragma region XZ方向の法線を取得

	constexpr float epsilon = 1e-6f;

	Vector3 pushNormal;

	float distance = 0.0f;

	if (distanceSq > epsilon * epsilon) {
		distance = std::sqrt(distanceSq);

		pushNormal = Vector3::XZonly(vec.x / distance, vec.y / distance);
	}
	else {
		/*
			XZ座標が完全一致している場合は
			押し出す方向を計算できない。

			そのため、Sphereの移動方向の逆方向を使う。
		*/

		Vector3 fallbackNormal = -sphere->GetActor()->GetTrans().Velocity();

		// Y方向は使用しない
		fallbackNormal.y = 0.0f;

		if (fallbackNormal.LengthSq() > epsilon * epsilon) { pushNormal = fallbackNormal.Normalized(); }
		// 移動していない場合は固定方向
		else { pushNormal = Vector3::Xonly(1.0f); }

		distance = 0.0f;
	}

#pragma endregion

#pragma region 衝突点を計算

	/*
		衝突点のY座標。

		Sphereの中心Yを、XZCircleColliderの
		Y範囲内に収めた位置にする。
	*/
	const float collisionY = std::clamp(spherePos.y, circleMinY, circleMaxY);

	// XZCircleCollider表面上の点
	const Vector3 pointOnCircle(circlePos.x + pushNormal.x * circleRadius, collisionY, circlePos.z + pushNormal.z * circleRadius);

	// Sphere表面上の点
	const Vector3 pointOnSphere(spherePos.x - pushNormal.x * sphereRadius, collisionY, spherePos.z - pushNormal.z * sphereRadius);

	// 両方の表面点の中間を代表衝突点にする
	result.point = (pointOnCircle + pointOnSphere) * 0.5f;

#pragma endregion

#pragma region 衝突確定：必要ならXZ方向へ押し出し

	const float overlap = radiusSum - distance;

	SetContactNormalAndDepth(result, pushNormal, overlap);

	if (NeedContactResponse(sphere, xzcircle)) {
		QueuePositionCorrection(sphere, xzcircle, pushNormal, overlap);
	}

#pragma endregion

	return true;
}


// Capsule×Boxの並進CCD。回転は現在姿勢で近似する。
// カプセル中心軸上の複数の球をBoxローカル空間で掃引し、最初の接触時刻を探す。
// 拡張AABBへのRaycastで候補を得た後、実際の球とBoxの距離で偽陽性を除外する。
namespace
{
    bool SweptCapsuleBoxTOI(CapsuleCollider* capsule, BoxCollider* box, CollisionResult& result)
    {
        if (!capsule || !box) return false;
        const Vector3 capsuleDelta = capsule->GetPos() - capsule->GetPrevPos();
        const Vector3 boxDelta = box->GetPos() - box->GetPrevPos();
        const Vector3 relativeDelta = capsuleDelta - boxDelta;
        if (relativeDelta.LengthSq() < 1e-8f) return false;

        const Vector3 boxCenter = box->GetPos();
        const auto axes = box->GetAxes();
        const Vector3 half = box->GetHalfSize();
        const float radius = capsule->GetRadius();
        const Vector3 expanded(half.x + radius, half.y + radius, half.z + radius);
        const Vector3 axisStart = capsule->GetStartPos();
        const Vector3 axisEnd = capsule->GetEndPos();
        const Vector3 axisVector = axisEnd - axisStart;
        const Vector3 localMove(relativeDelta.Dot(axes[0]), relativeDelta.Dot(axes[1]), relativeDelta.Dot(axes[2]));

        float bestTime = 2.0f;
        Vector3 bestNormal(0.0f);
        Vector3 bestPoint(0.0f);
        // 中心軸の離散サンプル。長いCapsuleや回転中のCCDは今後の対象。
        constexpr int kAxisSamples = 9;
        for (int sample = 0; sample < kAxisSamples; ++sample) {
            const float fraction = float(sample) / float(kAxisSamples - 1);
            const Vector3 worldCenter = axisStart + axisVector * fraction;
            const Vector3 diff = worldCenter - boxCenter;
            const Vector3 localCurrent(diff.Dot(axes[0]), diff.Dot(axes[1]), diff.Dot(axes[2]));
            const Vector3 localPrevious = localCurrent - localMove;
            const float oldCoords[3] = { localPrevious.x,localPrevious.y,localPrevious.z };
            const float velocities[3] = { localMove.x,localMove.y,localMove.z };
            const float extents[3] = { expanded.x,expanded.y,expanded.z };
            float entry = 0.0f, exit = 1.0f;
            int hitAxis = -1;
            float hitSign = 0.0f;
            bool valid = true;
            for (int axis = 0; axis < 3; ++axis) {
                const float v = velocities[axis];
                if (std::abs(v) < 1e-7f) {
                    if (oldCoords[axis] < -extents[axis] || oldCoords[axis] > extents[axis]) { valid = false; break; }
                    continue;
                }
                float t1 = (-extents[axis] - oldCoords[axis]) / v;
                float t2 = ( extents[axis] - oldCoords[axis]) / v;
                const float sign = v > 0.0f ? -1.0f : 1.0f;
                if (t1 > t2) std::swap(t1, t2);
                if (t1 > entry) { entry = t1; hitAxis = axis; hitSign = sign; }
                exit = (std::min)(exit, t2);
                if (entry > exit) { valid = false; break; }
            }
            // 拡張AABBは接触候補区間にすぎない。角・稜線では入口時点で
            // 球とBoxがまだ接触していないため、区間内の最短距離を調べる。
            if (!valid || entry > exit || entry > 1.0f || exit < 0.0f || entry >= bestTime) continue;
            const float intervalStart = (std::max)(0.0f, entry);
            const float intervalEnd = (std::min)(1.0f, (std::min)(exit, bestTime));
            if (intervalStart > intervalEnd) continue;

            const auto distanceSqAt = [&](float time) -> float {
                const Vector3 p = localPrevious + localMove * time;
                const Vector3 q(
                    std::clamp(p.x, -half.x, half.x),
                    std::clamp(p.y, -half.y, half.y),
                    std::clamp(p.z, -half.z, half.z));
                return (p - q).LengthSq();
            };
            const float radiusSq = radius * radius;
            const float tolerance = 1e-4f;
            // 既に接触していたサンプルは新規TOIとして扱わない。
            // 通常の離散接触判定に任せ、開始時の重なりによる誤った巻き戻しを防ぐ。
            if (distanceSqAt(0.0f) <= radiusSq + tolerance) continue;
            float contactTime = intervalStart;
            if (distanceSqAt(intervalStart) > radiusSq + tolerance) {
                // 並進する点と凸Boxの距離二乗は時間について凸関数。
                // 三分探索で区間内の最接近時刻を求める。
                float lo = intervalStart;
                float hi = intervalEnd;
                for (int i = 0; i < 28; ++i) {
                    const float m1 = lo + (hi - lo) / 3.0f;
                    const float m2 = hi - (hi - lo) / 3.0f;
                    if (distanceSqAt(m1) < distanceSqAt(m2)) hi = m2;
                    else lo = m1;
                }
                const float closestTime = (lo + hi) * 0.5f;
                if (distanceSqAt(closestTime) > radiusSq + tolerance) continue;

                // 最接近時刻までの最初の接触を二分探索する。
                lo = intervalStart;
                hi = closestTime;
                for (int i = 0; i < 26; ++i) {
                    const float mid = (lo + hi) * 0.5f;
                    if (distanceSqAt(mid) <= radiusSq + tolerance) hi = mid;
                    else lo = mid;
                }
                contactTime = hi;
            }
            if (contactTime >= bestTime) continue;

            const Vector3 localHit = localPrevious + localMove * contactTime;
            const Vector3 closest(
                std::clamp(localHit.x, -half.x, half.x),
                std::clamp(localHit.y, -half.y, half.y),
                std::clamp(localHit.z, -half.z, half.z));
            const Vector3 separation = localHit - closest;
            Vector3 localNormal(0.0f);
            if (separation.LengthSq() > 1e-10f) {
                localNormal = separation.Normalized();
            }
            else {
                // 開始時に重なっている場合など、距離から法線を決められないとき。
                // 移動に逆らう方向を優先する。
                const float ax = std::abs(localMove.x);
                const float ay = std::abs(localMove.y);
                const float az = std::abs(localMove.z);
                if (ax >= ay && ax >= az) localNormal.x = localMove.x >= 0.0f ? -1.0f : 1.0f;
                else if (ay >= az) localNormal.y = localMove.y >= 0.0f ? -1.0f : 1.0f;
                else localNormal.z = localMove.z >= 0.0f ? -1.0f : 1.0f;
            }
            Vector3 worldNormal = axes[0] * localNormal.x + axes[1] * localNormal.y + axes[2] * localNormal.z;
            worldNormal.Normalize();
            // 接触面へ近づいている場合のみ、新規衝突として採用する。
            // 既に接触面から離れているケースで逆向きの補正を発生させない。
            if (relativeDelta.Dot(worldNormal) >= -1.0e-5f) continue;
            // 接触時刻のBox表面位置（Boxの並進も考慮）。
            const Vector3 boxAtHit = boxCenter - boxDelta * (1.0f - contactTime);
            const Vector3 worldPoint = boxAtHit + axes[0] * closest.x + axes[1] * closest.y + axes[2] * closest.z;
            bestTime = contactTime;
            bestNormal = worldNormal;
            bestPoint = worldPoint;
        }
        if (bestTime > 1.0f) return false;
        result.normal = bestNormal;
        result.point = bestPoint;
        // 最初の接触時刻以降の相対移動を法線へ投影して位置拘束へ渡す。
        // 接線方向は維持する（スライド可能）。厳密な全位置巻き戻しではない。
        result.penetration = (std::max)(0.0f, -relativeDelta.Dot(bestNormal) * (1.0f - bestTime)) + 0.02f;
        return true;
    }

    // Sphere × Box: Capsuleの安定版TOI探索と同じ接触時刻探索を利用する。
    bool SweptSphereBoxTOI(SphereCollider* sphere, BoxCollider* box, CollisionResult& result)
    {
        if (!sphere || !box) return false;
        const Vector3 capsuleDelta = sphere->GetPos() - sphere->GetPrevPos();
        const Vector3 boxDelta = box->GetPos() - box->GetPrevPos();
        const Vector3 relativeDelta = capsuleDelta - boxDelta;
        if (relativeDelta.LengthSq() < 1e-8f) return false;

        const Vector3 boxCenter = box->GetPos();
        const auto axes = box->GetAxes();
        const Vector3 half = box->GetHalfSize();
        const float radius = sphere->GetRadius();
        const Vector3 expanded(half.x + radius, half.y + radius, half.z + radius);
        const Vector3 axisStart = sphere->GetPos();
        const Vector3 axisEnd = sphere->GetPos();
        const Vector3 axisVector = axisEnd - axisStart;
        const Vector3 localMove(relativeDelta.Dot(axes[0]), relativeDelta.Dot(axes[1]), relativeDelta.Dot(axes[2]));

        float bestTime = 2.0f;
        Vector3 bestNormal(0.0f);
        Vector3 bestPoint(0.0f);
        // 球体の中心点を掃引する。Boxの回転はフレーム中一定と近似。
        constexpr int kAxisSamples = 1;
        for (int sample = 0; sample < kAxisSamples; ++sample) {
            const float fraction = 0.0f;
            const Vector3 worldCenter = axisStart + axisVector * fraction;
            const Vector3 diff = worldCenter - boxCenter;
            const Vector3 localCurrent(diff.Dot(axes[0]), diff.Dot(axes[1]), diff.Dot(axes[2]));
            const Vector3 localPrevious = localCurrent - localMove;
            const float oldCoords[3] = { localPrevious.x,localPrevious.y,localPrevious.z };
            const float velocities[3] = { localMove.x,localMove.y,localMove.z };
            const float extents[3] = { expanded.x,expanded.y,expanded.z };
            float entry = 0.0f, exit = 1.0f;
            int hitAxis = -1;
            float hitSign = 0.0f;
            bool valid = true;
            for (int axis = 0; axis < 3; ++axis) {
                const float v = velocities[axis];
                if (std::abs(v) < 1e-7f) {
                    if (oldCoords[axis] < -extents[axis] || oldCoords[axis] > extents[axis]) { valid = false; break; }
                    continue;
                }
                float t1 = (-extents[axis] - oldCoords[axis]) / v;
                float t2 = ( extents[axis] - oldCoords[axis]) / v;
                const float sign = v > 0.0f ? -1.0f : 1.0f;
                if (t1 > t2) std::swap(t1, t2);
                if (t1 > entry) { entry = t1; hitAxis = axis; hitSign = sign; }
                exit = (std::min)(exit, t2);
                if (entry > exit) { valid = false; break; }
            }
            // 拡張AABBは接触候補区間にすぎない。角・稜線では入口時点で
            // 球とBoxがまだ接触していないため、区間内の最短距離を調べる。
            if (!valid || entry > exit || entry > 1.0f || exit < 0.0f || entry >= bestTime) continue;
            const float intervalStart = (std::max)(0.0f, entry);
            const float intervalEnd = (std::min)(1.0f, (std::min)(exit, bestTime));
            if (intervalStart > intervalEnd) continue;

            const auto distanceSqAt = [&](float time) -> float {
                const Vector3 p = localPrevious + localMove * time;
                const Vector3 q(
                    std::clamp(p.x, -half.x, half.x),
                    std::clamp(p.y, -half.y, half.y),
                    std::clamp(p.z, -half.z, half.z));
                return (p - q).LengthSq();
            };
            const float radiusSq = radius * radius;
            const float tolerance = 1e-4f;
            // 既に接触していたサンプルは新規TOIとして扱わない。
            // 通常の離散接触判定に任せ、開始時の重なりによる誤った巻き戻しを防ぐ。
            if (distanceSqAt(0.0f) <= radiusSq + tolerance) continue;
            float contactTime = intervalStart;
            if (distanceSqAt(intervalStart) > radiusSq + tolerance) {
                // 並進する点と凸Boxの距離二乗は時間について凸関数。
                // 三分探索で区間内の最接近時刻を求める。
                float lo = intervalStart;
                float hi = intervalEnd;
                for (int i = 0; i < 28; ++i) {
                    const float m1 = lo + (hi - lo) / 3.0f;
                    const float m2 = hi - (hi - lo) / 3.0f;
                    if (distanceSqAt(m1) < distanceSqAt(m2)) hi = m2;
                    else lo = m1;
                }
                const float closestTime = (lo + hi) * 0.5f;
                if (distanceSqAt(closestTime) > radiusSq + tolerance) continue;

                // 最接近時刻までの最初の接触を二分探索する。
                lo = intervalStart;
                hi = closestTime;
                for (int i = 0; i < 26; ++i) {
                    const float mid = (lo + hi) * 0.5f;
                    if (distanceSqAt(mid) <= radiusSq + tolerance) hi = mid;
                    else lo = mid;
                }
                contactTime = hi;
            }
            if (contactTime >= bestTime) continue;

            const Vector3 localHit = localPrevious + localMove * contactTime;
            const Vector3 closest(
                std::clamp(localHit.x, -half.x, half.x),
                std::clamp(localHit.y, -half.y, half.y),
                std::clamp(localHit.z, -half.z, half.z));
            const Vector3 separation = localHit - closest;
            Vector3 localNormal(0.0f);
            if (separation.LengthSq() > 1e-10f) {
                localNormal = separation.Normalized();
            }
            else {
                // 開始時に重なっている場合など、距離から法線を決められないとき。
                // 移動に逆らう方向を優先する。
                const float ax = std::abs(localMove.x);
                const float ay = std::abs(localMove.y);
                const float az = std::abs(localMove.z);
                if (ax >= ay && ax >= az) localNormal.x = localMove.x >= 0.0f ? -1.0f : 1.0f;
                else if (ay >= az) localNormal.y = localMove.y >= 0.0f ? -1.0f : 1.0f;
                else localNormal.z = localMove.z >= 0.0f ? -1.0f : 1.0f;
            }
            Vector3 worldNormal = axes[0] * localNormal.x + axes[1] * localNormal.y + axes[2] * localNormal.z;
            worldNormal.Normalize();
            // 接触面へ近づいている場合のみ、新規衝突として採用する。
            // 既に接触面から離れているケースで逆向きの補正を発生させない。
            if (relativeDelta.Dot(worldNormal) >= -1.0e-5f) continue;
            // 接触時刻のBox表面位置（Boxの並進も考慮）。
            const Vector3 boxAtHit = boxCenter - boxDelta * (1.0f - contactTime);
            const Vector3 worldPoint = boxAtHit + axes[0] * closest.x + axes[1] * closest.y + axes[2] * closest.z;
            bestTime = contactTime;
            bestNormal = worldNormal;
            bestPoint = worldPoint;
        }
        if (bestTime > 1.0f) return false;
        result.normal = bestNormal;
        result.point = bestPoint;
        // 最初の接触時刻以降の相対移動を法線へ投影して位置拘束へ渡す。
        // 接線方向は維持する（スライド可能）。厳密な全位置巻き戻しではない。
        result.penetration = (std::max)(0.0f, -relativeDelta.Dot(bestNormal) * (1.0f - bestTime)) + 0.02f;
        return true;
    }
}

// カプセル×ボックス
bool CollisionManager::CapsuleToBox(CapsuleCollider* capsule, BoxCollider* box, CollisionResult& result)
{
#pragma region 必要情報の取得

	// カプセル中心軸
	const Vector3 capsuleStart = capsule->GetStartPos();
	const Vector3 capsuleEnd = capsule->GetEndPos();
	const float capsuleRadius = capsule->GetRadius();

	// Box
	const Vector3 boxPos = box->GetPos();
	const Vector3 half = box->GetHalfSize();

	// 回転後のBoxの3軸
	const std::array<Vector3, 3> boxAxes = box->GetAxes();

#pragma endregion

#pragma region カプセル中心軸をBoxローカル座標へ変換

	const Vector3 startDifference = capsuleStart - boxPos;

	const Vector3 endDifference = capsuleEnd - boxPos;

	const Vector3 localStart(startDifference.Dot(boxAxes[0]), startDifference.Dot(boxAxes[1]), startDifference.Dot(boxAxes[2]));

	const Vector3 localEnd(endDifference.Dot(boxAxes[0]), endDifference.Dot(boxAxes[1]), endDifference.Dot(boxAxes[2]));

#pragma endregion

#pragma region 線分とAABBの最近点ペアを求める

	// Boxローカル空間でのカプセル中心軸線分
	const Vector3 localSegment = localEnd - localStart;

	// 線分上の割合
	float t = 0.5f;

	// 
	Vector3 localCapsuleClosest;
	Vector3 localBoxClosest;

	constexpr int iterationCount = 8;

	for (int i = 0; i < iterationCount; ++i) {
		// 現在の線分上の点
		localCapsuleClosest = localStart + localSegment * t;

		// その点からBoxへの最近点
		localBoxClosest.x = std::clamp(localCapsuleClosest.x, -half.x, half.x);

		localBoxClosest.y = std::clamp(localCapsuleClosest.y, -half.y, half.y);

		localBoxClosest.z = std::clamp(localCapsuleClosest.z, -half.z, half.z);

		// Box最近点を線分へ再投影
		const float segmentLengthSq = localSegment.LengthSq();

		if (segmentLengthSq <= 1e-8f) { t = 0.0f;			break; }

		t = (localBoxClosest - localStart).Dot(localSegment) / segmentLengthSq;
		t = std::clamp(t, 0.0f, 1.0f);
	}

	// 最終結果を再計算
	localCapsuleClosest = localStart + localSegment * t;

	localBoxClosest.x = std::clamp(localCapsuleClosest.x, -half.x, half.x);
	localBoxClosest.y = std::clamp(localCapsuleClosest.y, -half.y, half.y);
	localBoxClosest.z = std::clamp(localCapsuleClosest.z, -half.z, half.z);

#pragma endregion

#pragma region 最近点をワールド座標へ変換

	const Vector3 capsuleClosest = boxPos + boxAxes[0] * localCapsuleClosest.x + boxAxes[1] * localCapsuleClosest.y + boxAxes[2] * localCapsuleClosest.z;
	const Vector3 boxClosest = boxPos + boxAxes[0] * localBoxClosest.x + boxAxes[1] * localBoxClosest.y + boxAxes[2] * localBoxClosest.z;

#pragma endregion

#pragma region 衝突判定

	Vector3 normal = capsuleClosest - boxClosest;

	const float distSq = normal.LengthSq();

    // 高速移動時は現在位置の貫通判定より、移動経路上の最初の接触を優先。
    // 現在位置で既にBoxの裏側へ深く入っていても、入口側の法線を保持する。
    // 開始時から接触していた場合は掃引側で除外し、通常判定に任せる。
    if (SweptCapsuleBoxTOI(capsule, box, result)) return true;
    if (distSq > capsuleRadius * capsuleRadius) return false;

#pragma endregion

#pragma region 衝突方向・めり込み量・衝突点を計算

	constexpr float epsilon = 1e-6f;

	Vector3 pushNormal;
	float overlap = 0.0f;

	if (distSq > epsilon * epsilon) {
		// カプセル中心軸がBox外側にある通常ケース
		const float distance = std::sqrt(distSq);

		pushNormal = normal / distance;

		overlap = capsuleRadius - distance;

		// カプセル表面上の点
		const Vector3 pointOnCapsule = capsuleClosest - pushNormal * capsuleRadius;

		// Box表面上の最近点との中間
		result.point = (pointOnCapsule + boxClosest) * 0.5f;
	}
	else {

		const float distanceToXFace = half.x - std::abs(localCapsuleClosest.x);
		const float distanceToYFace = half.y - std::abs(localCapsuleClosest.y);
		const float distanceToZFace = half.z - std::abs(localCapsuleClosest.z);

		Vector3 localPushNormal;
		Vector3 localSurfacePoint = localCapsuleClosest;

		float distanceToSurface = 0.0f;

		if (distanceToXFace <= distanceToYFace && distanceToXFace <= distanceToZFace) {
			float sign = 1.0f;

			if (localCapsuleClosest.x < 0.0f) { sign = -1.0f; }

			localPushNormal = Vector3::Xonly(sign);

			localSurfacePoint.x = half.x * sign;

			distanceToSurface = distanceToXFace;
		}
		else if (distanceToYFace <= distanceToXFace && distanceToYFace <= distanceToZFace) {
			float sign = 1.0f;

			if (localCapsuleClosest.y < 0.0f) { sign = -1.0f; }

			localPushNormal = Vector3::Yonly(sign);

			localSurfacePoint.y = half.y * sign;

			distanceToSurface = distanceToYFace;
		}
		else {
			float sign = 1.0f;

			if (localCapsuleClosest.z < 0.0f) { sign = -1.0f; }

			localPushNormal = Vector3::Zonly(sign);

			localSurfacePoint.z = half.z * sign;

			distanceToSurface = distanceToZFace;
		}

		// ローカル法線をワールド空間へ変換
		pushNormal = boxAxes[0] * localPushNormal.x + boxAxes[1] * localPushNormal.y + boxAxes[2] * localPushNormal.z;
		pushNormal.Normalize();

		// Box内部から表面までの距離＋カプセル半径
		overlap = distanceToSurface + capsuleRadius;

		// 最寄り面上の点をワールド座標へ変換
		result.point = boxPos + boxAxes[0] * localSurfacePoint.x + boxAxes[1] * localSurfacePoint.y + boxAxes[2] * localSurfacePoint.z;
	}

#pragma endregion

#pragma region 衝突確定：必要なら位置補正

	SetContactNormalAndDepth(result, pushNormal, (std::max)(overlap, 0.0f));


#pragma endregion

	return true;
}

// カプセル×メッシュ
bool CollisionManager::CapsuleToMesh(CapsuleCollider* capsule, MeshCollider* mesh, CollisionResult& result)
{
	mesh->QueryTriangles(capsule->GetAABB(), meshQueryBufferA);

	const Vector3 capsuleStart = capsule->GetStartPos();

	const Vector3 capsuleEnd = capsule->GetEndPos();

	const float radius = capsule->GetRadius();

	const float radiusSq = radius * radius;

	bool isHit = false;
	float maximumPenetration = -FLT_MAX;

	for (unsigned int triangleIndex : meshQueryBufferA) {
		Vector3 p0, p1, p2, triangleNormal;

		mesh->GetWorldTriangle(triangleIndex, p0, p1, p2, triangleNormal);

		Vector3 capsulePoint;
		Vector3 trianglePoint;

		const float distanceSq = SegmentToTriangleDistanceSq(capsuleStart, capsuleEnd, p0, p1, p2, capsulePoint, trianglePoint);

		if (distanceSq > radiusSq) { continue; }

		const float distance = std::sqrt((std::max)(distanceSq, 0.0f));

		// ポリゴン法線方向のめり込み量を求める（面の表側を衝突可能側とする）
		if (triangleNormal.LengthSq() <= COLLISION_EPSILON) { continue; }
		const Vector3 normal = triangleNormal.Normalized();
		const float signedDistance = (capsulePoint - p0).Dot(normal);
		const float penetration = radius - signedDistance;
		if (penetration <= 0.0f) { continue; }

		if (penetration <= maximumPenetration) { continue; }

		maximumPenetration = penetration;
		result.point = trianglePoint;
		SetContactNormalAndDepth(result, normal, penetration);
		isHit = true;
	}


	return isHit;
}

// カプセル×XZ平面上の円形
bool CollisionManager::CapsuleToXZCircle(CapsuleCollider* capsule, XZCircleCollider* xzcircle, CollisionResult& result)
{
#pragma region 必要情報を取得

	const Vector3 capsuleStart = capsule->GetStartPos();

	const Vector3 capsuleEnd = capsule->GetEndPos();

	const float capsuleRadius = capsule->GetRadius();

	const Vector3 circlePos = xzcircle->GetPos();

	const float circleRadius = xzcircle->GetRadius();

	const float circleYLength = xzcircle->GetYLength();

	const float radiusSum = capsuleRadius + circleRadius;

#pragma endregion

#pragma region Y方向の早期リターン

	// カプセル中心軸のY方向範囲
	const float capsuleAxisMinY = (std::min)(capsuleStart.y, capsuleEnd.y);

	const float capsuleAxisMaxY = (std::max)(capsuleStart.y, capsuleEnd.y);

	// カプセル全体のY方向範囲
	const float capsuleMinY = capsuleAxisMinY - capsuleRadius;

	const float capsuleMaxY = capsuleAxisMaxY + capsuleRadius;

	// XZCircleColliderのY方向範囲
	const float circleMinY = circlePos.y - circleYLength;

	const float circleMaxY = circlePos.y + circleYLength;

	// Y方向で完全に離れている
	if (capsuleMaxY < circleMinY || capsuleMinY > circleMaxY) { return false; }

#pragma endregion

#pragma region XZ平面へ投影

	Vector3 startXZ = capsuleStart;

	Vector3 endXZ = capsuleEnd;

	Vector3 circleXZ = circlePos;

	startXZ.y = 0.0f;
	endXZ.y = 0.0f;
	circleXZ.y = 0.0f;

#pragma endregion

#pragma region カプセル中心軸上の最近点を取得

	// XZ平面上のカプセル中心軸
	const Vector3 segmentXZ = endXZ - startXZ;

	const Vector3 startToCircle = circleXZ - startXZ;

	const float segmentLengthSq = segmentXZ.LengthSq();

	float t = 0.0f;

	if (segmentLengthSq > 1e-8f) {
		t = segmentXZ.Dot(startToCircle) / segmentLengthSq;

		t = std::clamp(t, 0.0f, 1.0f);
	}

	// XZ平面上の最近点
	const Vector3 closestXZ = startXZ + segmentXZ * t;

	// XZ平面上で求めたtを使って、実際の3Dカプセル中心軸上の点も求める
	const Vector3 capsuleAxis = capsuleEnd - capsuleStart;
	const Vector3 closestOnCapsuleAxis = capsuleStart + capsuleAxis * t;

#pragma endregion

#pragma region XZ方向の衝突判定

	const Vector3 differenceXZ = closestXZ - circleXZ;

	const float distanceSq = differenceXZ.LengthSq();

	if (distanceSq > radiusSum * radiusSum) { return false; }

#pragma endregion

#pragma region XZ方向の法線を取得

	constexpr float epsilon = 1e-6f;

	Vector3 pushNormal;

	float distance = 0.0f;

	if (distanceSq > epsilon * epsilon) {
		distance = std::sqrt(distanceSq);

		pushNormal = Vector3::XZonly(differenceXZ.x / distance, differenceXZ.z / distance);
	}
	// XZ座標が完全に一致している場合
	else {

		// 移動方向の逆をXZ平面へ投影して使用する。
		Vector3 fallbackNormal = -capsule->GetActor()->GetTrans().Velocity();
		fallbackNormal.y = 0.0f;

		if (fallbackNormal.LengthSq() > epsilon * epsilon) { pushNormal = fallbackNormal.Normalized(); }
		// 移動していなければ固定方向
		else { pushNormal = Vector3::Xonly(1.0f); }

		distance = 0.0f;
	}

#pragma endregion

#pragma region 衝突点を計算

	/*
		衝突点のY座標は、
		カプセル中心軸上の最近点Yを
		XZCircleColliderの高さ範囲に収める。
	*/
	const float collisionY = std::clamp(closestOnCapsuleAxis.y, circleMinY, circleMaxY);

	// XZCircleCollider表面上の点
	const Vector3 pointOnCircle(circlePos.x + pushNormal.x * circleRadius, collisionY, circlePos.z + pushNormal.z * circleRadius);

	// カプセル表面上の点
	const Vector3 pointOnCapsule(closestOnCapsuleAxis.x - pushNormal.x * capsuleRadius, collisionY, closestOnCapsuleAxis.z - pushNormal.z * capsuleRadius);

	// 両形状の表面点の中間
	result.point = (pointOnCircle + pointOnCapsule) * 0.5f;

#pragma endregion

#pragma region 衝突確定：必要ならXZ方向へ押し出し

	const float overlap = radiusSum - distance;

	SetContactNormalAndDepth(result, pushNormal, overlap);

	if (NeedContactResponse(capsule, xzcircle)) {
		QueuePositionCorrection(capsule, xzcircle, pushNormal, overlap);
	}

#pragma endregion

	return true;
}

// ボックス×メッシュ
bool CollisionManager::BoxToMesh(BoxCollider* box, MeshCollider* mesh, CollisionResult& result)
{
	mesh->QueryTriangles(box->GetAABB(), meshQueryBufferA);

	bool isHit = false;
	float minimumPenetration = FLT_MAX;

	for (unsigned int triangleIndex : meshQueryBufferA) {
		Vector3 p0, p1, p2, triangleNormal;

		mesh->GetWorldTriangle(triangleIndex, p0, p1, p2, triangleNormal);

		Vector3 normal;
		float penetration = 0.0f;

		if (!TriangleToOBB(p0, p1, p2, *box, normal, penetration)) { continue; }

		// SATで衝突を判定し、接触法線はポリゴン法線に固定
		if (triangleNormal.LengthSq() <= COLLISION_EPSILON) { continue; }
		normal = triangleNormal.Normalized();
		const auto axes = box->GetAxes();
		const Vector3 half = box->GetHalfSize();
		const float radius =
			std::abs(normal.Dot(axes[0])) * half.x +
			std::abs(normal.Dot(axes[1])) * half.y +
			std::abs(normal.Dot(axes[2])) * half.z;
		penetration = radius - (box->GetPos() - p0).Dot(normal) + COLLISION_EPSILON;
		if (penetration <= 0.0f) { continue; }
		if (penetration >= minimumPenetration) { continue; }

		minimumPenetration = penetration;
		SetContactNormalAndDepth(result, normal, penetration);


		result.point = ClosestPointOnTriangle(BoxFaceCenter(box, -normal), p0, p1, p2);

		isHit = true;
	}


	return isHit;
}

#pragma endregion

#pragma region 接触応答・位置補正

// 接触応答の設定とColliderの移動区分から、共通ソルバーへの登録を判定する。
bool CollisionManager::NeedContactResponse(ColliderBase* a, ColliderBase* b)const
{
    if (!a || !b) { return false; }
    ActorBase* actorA = a->GetActor();
    ActorBase* actorB = b->GetActor();
    if (!actorA || !actorB) { return false; }
    if (!actorA->IsContactResponseEnabled() || !actorB->IsContactResponseEnabled()) { return false; }
    return a->GetMobility() == COLLIDER_MOBILITY::Movable ||
           b->GetMobility() == COLLIDER_MOBILITY::Movable;
}

// Line・XZCircle用の位置補正を登録する。
// Sphere・Capsule・Box・Meshの接触応答はContactSolverで処理する。
void CollisionManager::QueuePositionCorrection(ColliderBase* a, ColliderBase* b, const Vector3& normal, float overlap)const
{
    QueuePositionCorrection(a, b, normal * overlap);
}

void CollisionManager::QueuePositionCorrection(ColliderBase* a, ColliderBase* b, const Vector3& overlapVec)const
{
    if (!a || !b) return;

    // 体積形状は共通ソルバーで処理するため、ここでは補正しない。
    const COLLIDER_SHAPE shapeA = a->GetShape();
    const COLLIDER_SHAPE shapeB = b->GetShape();
    const bool specialA = shapeA == COLLIDER_SHAPE::Line || shapeA == COLLIDER_SHAPE::XzCircle;
    const bool specialB = shapeB == COLLIDER_SHAPE::Line || shapeB == COLLIDER_SHAPE::XzCircle;
    if (!specialA && !specialB) return;

    // Line・XZCircleのみ専用の位置補正を登録する。
    ContactSolver::AddPositionCorrection(a, b, overlapVec);
}

void CollisionManager::QueueOneSidePositionCorrection(ColliderBase* dynamicColl, ColliderBase* staticColl, const Vector3& overlapVec)const
{
    ContactSolver::AddOneSideCorrection(dynamicColl, staticColl, overlapVec);
}

#pragma endregion

#pragma region レイキャスト

// レイと登録済みコライダーの交差判定。最短距離のヒットのみ返す。
RaycastHit CollisionManager::Raycast(const Vector3& origin, const Vector3& direction, float maxDistance)
{
    RaycastHit hit;
    Raycast(origin, direction, maxDistance, hit);
    return hit;
}

bool CollisionManager::Raycast(const Vector3& origin, const Vector3& direction, float maxDistance, RaycastHit& result)
{
    result = RaycastHit{};
    constexpr float eps = 1.0e-6f;
    if (maxDistance <= 0.0f || direction.LengthSq() <= eps) { return false; }

    const Vector3 dir = direction.Normalized();
    float nearest = maxDistance;

    // 1つの候補が命中した場合、これまでの最短距離より近ければ更新
    auto Accept = [&](ColliderBase* collider, float t, const Vector3& normal) {
        if (t < 0.0f || t > nearest) { return; }
        nearest = t;
        result.collider = collider;
        result.distance = t;
        result.point = origin + dir * t;
        result.normal = normal;
        if (result.normal.LengthSq() > eps) { result.normal.Normalize(); }
        else { result.normal = -dir; }
    };

    // 球（カプセルの両端にも利用）
    auto TestSphere = [&](ColliderBase* collider, const Vector3& center, float radius) {
        const Vector3 oc = origin - center;
        const float b = oc.Dot(dir);
        const float d = b * b - (oc.Dot(oc) - radius * radius);
        if (d < 0.0f) { return; }
        const float root = std::sqrt(d);
        float t = -b - root;
        if (t < 0.0f) { t = -b + root; }
        if (t >= 0.0f && t <= nearest) {
            const Vector3 p = origin + dir * t;
            Accept(collider, t, p - center);
        }
    };

	for (int i = 0; i < (int)COLLIDER_GROUP::Max; i++) {

		if (i == (int)COLLIDER_GROUP::Player) { continue; }

        for (ColliderBase* collider : groupColliders[i].colliders) {
            if (!collider || !collider->GetJudgeFlg()) { continue; }
			if (
				collider->GetTag() == COLLIDER_TAG::CameraLine ||
				collider->GetTag() == COLLIDER_TAG::CameraPoint
				) {
				continue;
			}

            switch (collider->GetShape()) {
            case COLLIDER_SHAPE::Sphere: {
                auto* sphere = static_cast<SphereCollider*>(collider);
                TestSphere(collider, sphere->GetPos(), sphere->GetRadius());
                break;
            }
            case COLLIDER_SHAPE::Capsule: {
                auto* capsule = static_cast<CapsuleCollider*>(collider);
                const Vector3 a = capsule->GetStartPos();
                const Vector3 b = capsule->GetEndPos();
                const float radius = capsule->GetRadius();
                const Vector3 axis = b - a;
                const float length = axis.Length();
                if (length <= eps) { TestSphere(collider, a, radius); break; }
                const Vector3 n = axis / length;
                const Vector3 m = origin - a;
                const float md = m.Dot(n);
                const float nd = dir.Dot(n);
                const Vector3 perpM = m - n * md;
                const Vector3 perpD = dir - n * nd;
                const float A = perpD.Dot(perpD);
                const float B = 2.0f * perpM.Dot(perpD);
                const float C = perpM.Dot(perpM) - radius * radius;
                if (A > eps) {
                    const float disc = B * B - 4.0f * A * C;
                    if (disc >= 0.0f) {
                        const float root = std::sqrt(disc);
                        const float candidates[2] = { (-B - root) / (2.0f * A), (-B + root) / (2.0f * A) };
                        for (float t : candidates) {
                            const float h = md + nd * t;
                            if (t >= 0.0f && h >= 0.0f && h <= length) {
                                Accept(collider, t, origin + dir * t - (a + n * h));
                            }
                        }
                    }
                }
                // 球部分は各端点の外側半球だけを採用する
                auto TestCap = [&](const Vector3& center, bool first) {
                    const Vector3 oc = origin - center;
                    const float v = oc.Dot(dir);
                    const float disc = v * v - (oc.Dot(oc) - radius * radius);
                    if (disc < 0.0f) { return; }
                    const float root = std::sqrt(disc);
                    const float candidates[2] = { -v - root, -v + root };
                    for (float t : candidates) {
                        if (t < 0.0f || t > nearest) { continue; }
                        const Vector3 p = origin + dir * t;
                        const float axial = (p - a).Dot(n);
                        if ((first && axial <= 0.0f) || (!first && axial >= length)) {
                            Accept(collider, t, p - center);
                        }
                    }
                };
                TestCap(a, true);
                TestCap(b, false);
                break;
            }
            case COLLIDER_SHAPE::Box: {
                auto* box = static_cast<BoxCollider*>(collider);
                const auto axes = box->GetAxes();
                const Vector3 half = box->GetHalfSize();
                const float extent[3] = { half.x, half.y, half.z };
                const Vector3 relative = origin - box->GetPos();
                float enter = -FLT_MAX, leave = FLT_MAX;
                Vector3 enterNormal, leaveNormal;
                bool valid = true;
                for (int i = 0; i < 3; ++i) {
                    const float o = relative.Dot(axes[i]);
                    const float d = dir.Dot(axes[i]);
                    if (std::abs(d) <= eps) {
                        if (o < -extent[i] || o > extent[i]) { valid = false; break; }
                        continue;
                    }
                    float t0 = (-extent[i] - o) / d;
                    float t1 = (extent[i] - o) / d;
                    Vector3 n0 = -axes[i], n1 = axes[i];
                    if (t0 > t1) { std::swap(t0, t1); std::swap(n0, n1); }
                    if (t0 > enter) { enter = t0; enterNormal = n0; }
                    if (t1 < leave) { leave = t1; leaveNormal = n1; }
                    if (enter > leave) { valid = false; break; }
                }
                if (valid && leave >= 0.0f) {
                    if (enter >= 0.0f) { Accept(collider, enter, enterNormal); }
                    else { Accept(collider, leave, leaveNormal); }
                }
                break;
            }
            case COLLIDER_SHAPE::Mesh: {
                auto* mesh = static_cast<MeshCollider*>(collider);
                const Vector3 end = origin + dir * nearest;
                for (unsigned int i = 0; i < mesh->GetTriangleNum(); ++i) {
                    Vector3 a, b, c, normal, point;
                    mesh->GetWorldTriangle(i, a, b, c, normal);
                    float rate = 0.0f;
                    Vector3 faceNormal;
                    if (SegmentToTriangle(origin, end, a, b, c, rate, point, faceNormal)) {
                        // endはこのメッシュの検査開始時点での最短距離
                        const float t = (point - origin).Dot(dir);
                        if (faceNormal.Dot(dir) > 0.0f) { faceNormal = -faceNormal; }
                        Accept(collider, t, faceNormal);
                    }
                }
                break;
            }
            default: break; // Line / XzCircle はこの3Dレイでは対象外
            }
        }
    }
    return result.IsHit();
}

#pragma endregion

#pragma region デバッグ描画

// チャンクのグリッド線描画
void CollisionManager::DrawChunkGrid(const Vector3& cameraPos) const
{
	if (!App::GetIns().IsDrawDebug()) { return; }

	const unsigned int color = GetColor(255, 255, 255);

	// カメラ周辺どこまで描くか
	const float drawRange = 3000.0f;

	int minX = ToChunkIndex(cameraPos.x - drawRange, CHUNK_SIZE);
	int maxX = ToChunkIndex(cameraPos.x + drawRange, CHUNK_SIZE);

	int minY = ToChunkIndex(cameraPos.y - drawRange, CHUNK_SIZE);
	int maxY = ToChunkIndex(cameraPos.y + drawRange, CHUNK_SIZE);

	int minZ = ToChunkIndex(cameraPos.z - drawRange, CHUNK_SIZE);
	int maxZ = ToChunkIndex(cameraPos.z + drawRange, CHUNK_SIZE);

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			DrawLine3D(
				VGet(x * CHUNK_SIZE, y * CHUNK_SIZE, minZ * CHUNK_SIZE),
				VGet(x * CHUNK_SIZE, y * CHUNK_SIZE, (maxZ + 1) * CHUNK_SIZE),
				color
			);
		}
	}

	for (int z = minZ; z <= maxZ; z++) {
		for (int y = minY; y <= maxY; y++) {
			DrawLine3D(
				VGet(minX * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE),
				VGet((maxX + 1) * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE),
				color
			);
		}
	}

	for (int x = minX; x <= maxX; x++) {
		for (int z = minZ; z <= maxZ; z++) {
			DrawLine3D(
				VGet(x * CHUNK_SIZE, minY * CHUNK_SIZE, z * CHUNK_SIZE),
				VGet(x * CHUNK_SIZE, (maxY + 1) * CHUNK_SIZE, z * CHUNK_SIZE),
				color
			);
		}
	}
}

void CollisionManager::DrawCheckLog(void) const
{
	if (!App::GetIns().IsDrawDebug()) { return; }

	// 描画位置
	const int x = 10;
	int y = 200;

	// 文字色
	const int color = GetColor(255, 255, 255);

	// 行間
	const int lineHeight = 20;

	DrawFormatString(x, y, color, "候補数 : %llu", static_cast<unsigned long long>(checkStats.pairCandidates));
	y += lineHeight;

	DrawFormatString(x, y, color, "判定数 : %llu", static_cast<unsigned long long>(checkStats.uniquePairs));
	y += lineHeight;

	DrawFormatString(x, y, color, "衝突数 : %llu", static_cast<unsigned long long>(checkStats.hitPairs));
	y += lineHeight;

	DrawFormatString(x, y, color, "チャンク : %.3f ms", checkStats.buildChunksMs);
	y += lineHeight;

	DrawFormatString(x, y, color, "衝突判定 : %.3f ms", checkStats.matchingMs);
	y += lineHeight;

	DrawFormatString(x, y, color, "ソルバー : %.3f ms", checkStats.solverMs);
}

#pragma endregion