#pragma once

#include <map>

#include "ChunkStDefine.h"

#include "../../Object/Common/Collider/ColliderBase.h"

class LineCollider;
class SphereCollider;
class CapsuleCollider;
class BoxCollider;
class MeshCollider;
class XZCircleCollider;

class CollisionManager
{
private:

#pragma region コライダーグループ定義

	// 列挙型定義定義
	enum class COLLIDER_GROUP {

		// 基本種別～～～～～～～～～～～～

		// プレイヤー系
		Player,
		// エネミー系
		Enemy,
		// ステージ系
		Stage,

		// ～～～～～～～～～～～～基本種別


		// 特殊種別～～～～～～～～～～～～

		// プレイヤー系にだけ当たるコライダー
		PlayerOnly,
		// エネミー系にだけ当たるコライダー
		EnemyOnly,
		// ステージ系にだけ当たるコライダー
		StageOnly,

		// ～～～～～～～～～～～～特殊種別

		// その他(例外)
		Other,

		// 種類数
		Max
	};

	/*以下の組み合わせの判定が実行される（順番も以下の通り）
	*
	* ①プレイヤー系 × ステージ系
	* ②エネミー系	 × ステージ系
	* 
	* ③プレイヤー系 × エネミー系
	* 
	* ④プレイヤー系 × プレイヤー系にだけ当たるコライダー
	* ⑤エネミー系	 × エネミー系にだけ当たるコライダー
	* ⑥ステージ系	 × ステージ系にだけ当たるコライダー
	*
	* ⑦その他 × ステージ系
	* ⑧その他 × プレイヤー系
	* ⑨その他 × エネミー系
	* ⑩その他 × その他
	*/

	// 各タグのグループ分け
	const std::map<COLLIDER_TAG, COLLIDER_GROUP>TAG_TO_GROUP_LIST = {

		// プレイヤー系
		{ COLLIDER_TAG::Player,	COLLIDER_GROUP::Player },

		// エネミー系
		{ COLLIDER_TAG::Enemy,	COLLIDER_GROUP::Enemy },

		// ステージ系
		{ COLLIDER_TAG::Stage,	COLLIDER_GROUP::Stage },

		// プレイヤー系にだけ当たるコライダー

		// エネミー系にだけ当たるコライダー

		// ステージ系にだけ当たるコライダー
	};


#pragma endregion

public:
	CollisionManager() : groupColliders() {}
	~CollisionManager() = default;

	// オブジェクト追加
	void Add(ColliderBase* collider);
	// オブジェクト追加
	void Add(std::vector<ColliderBase*> collider);

	// チャンク分け(初期化時)
	void InitBuildChunks(void);

	// 判定実行
	void Check(void);

	// チャンク描画
	void DrawChunkGrid(const Vector3& cameraPos) const;

	// 解放
	void Clear(void) {
		for (ColliderGroupData& colliders : groupColliders) {
			colliders.dynamicChunks.clear();
			colliders.staticChunks.clear();
			colliders.colliders.clear();
		}
	}

private:

	// グループ別の格納配列
	ColliderGroupData groupColliders[(int)COLLIDER_GROUP::Max];

	// 重複判定防止の判定記録
	std::unordered_set<ColliderPairKey, ColliderPairKeyHash> checkedPairs;

	// MeshCollider内部BVH検索用の作業バッファ
	// clearのみで再利用し、判定ごとのメモリ確保を抑える
	std::vector<unsigned int> meshQueryBufferA;
	std::vector<unsigned int> meshQueryBufferB;


#pragma region 判定の振り分け

	// チャンク分け(更新時)
	void BuildChunks(void);

	// 指定のグループ同士を判定(2グループ指定)
	void Matching(COLLIDER_GROUP groupA, COLLIDER_GROUP groupB);
	// 指定のグループを総当たりで判定(1グループ指定)
	void Matching(COLLIDER_GROUP group);

	// チャンクを振り分けて判定実行
	void MatchingChunks(ChunkMap& aChunks, ChunkMap& bChunks);
	// チャンクを振り分けて判定実行
	void MatchingChunks(ChunkMap& chunks);

	// 重複判定チェック
	void CheckPairOnce(ColliderBase* a, ColliderBase* b);

	// 形状の振り分け
	bool IsHit(ColliderBase* a, ColliderBase* b, CollisionResult& result);

#pragma endregion

#pragma region 各形状の実判定

	// 同形状同士～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	// 線分×線分
	bool LineToLine(LineCollider* a, LineCollider* b, CollisionResult& result);

	// 球体×球体
	bool SphereToSphere(SphereCollider* a, SphereCollider* b, CollisionResult& result);

	// カプセル×カプセル
	bool CapsuleToCapsule(CapsuleCollider* a, CapsuleCollider* b, CollisionResult& result);

	// ボックス×ボックス
	bool BoxToBox(BoxCollider* a, BoxCollider* b, CollisionResult& result);

	// メッシュ×メッシュ
	bool MeshToMesh(MeshCollider* a, MeshCollider* b, CollisionResult& result);

	// XZ平面上の円形×XZ平面上の円形
	bool XZCircleToXZCircle(XZCircleCollider* a, XZCircleCollider* b, CollisionResult& result);

	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～同形状同士


	// 別形状同士～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	// 線分×球体
	bool LineToSphere(LineCollider* line, SphereCollider* sphere, CollisionResult& result);

	// 線分×カプセル
	bool LineToCapsule(LineCollider* line, CapsuleCollider* capsule, CollisionResult& result);

	// 線分×ボックス
	bool LineToBox(LineCollider* line, BoxCollider* box, CollisionResult& result);

	// 線分×メッシュ
	bool LineToMesh(LineCollider* line, MeshCollider* mesh, CollisionResult& result);

	// 球体×カプセル
	bool SphereToCapsule(SphereCollider* sphere, CapsuleCollider* capsule, CollisionResult& result);

	// 球体×ボックス
	bool SphereToBox(SphereCollider* sphere, BoxCollider* box, CollisionResult& result);

	// 球体×メッシュ
	bool SphereToMesh(SphereCollider* sphere, MeshCollider* mesh, CollisionResult& result);

	// 球体×XZ平面上の円形
	bool SphereToXZCircle(SphereCollider* sphere, XZCircleCollider* xzcircle, CollisionResult& result);

	// カプセル×ボックス
	bool CapsuleToBox(CapsuleCollider* capsule, BoxCollider* box, CollisionResult& result);

	// カプセル×メッシュ
	bool CapsuleToMesh(CapsuleCollider* capsule, MeshCollider* mesh, CollisionResult& result);

	// カプセル×XZ平面上の円形
	bool CapsuleToXZCircle(CapsuleCollider* capsule, XZCircleCollider* xzcircle, CollisionResult& result);

	// ボックス×メッシュ
	bool BoxToMesh(BoxCollider* box, MeshCollider* mesh, CollisionResult& result);

	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～別形状同士

#pragma endregion

#pragma region ユーティリティ

	// 重なるチャンクインデックス(ChunkIndex)一覧を取得する
	void GetOverlappedChunks(const ColliderBase::AABB& aabb, std::vector<ChunkIndex>& out) const {
		out.clear();

		ChunkIndex minIndex = {
			ToChunkIndex(aabb.min.x, CHUNK_SIZE),
			ToChunkIndex(aabb.min.y, CHUNK_SIZE),
			ToChunkIndex(aabb.min.z, CHUNK_SIZE)
		};

		ChunkIndex maxIndex = {
			ToChunkIndex(aabb.max.x, CHUNK_SIZE),
			ToChunkIndex(aabb.max.y, CHUNK_SIZE),
			ToChunkIndex(aabb.max.z, CHUNK_SIZE)
		};

		for (int z = minIndex.z; z <= maxIndex.z; z++) {
			for (int y = minIndex.y; y <= maxIndex.y; y++) {
				for (int x = minIndex.x; x <= maxIndex.x; x++) {
					out.push_back(ChunkIndex(x, y, z));
				}
			}
		}
	}

	// 指定のチャンクマップにコライダーを1つ登録
	void RegisterToChunks(ChunkMap& chunks, ColliderBase* collider)const {
		// 安全処理
		if (!collider) { return; }

		// 重なるチャンクインデックスを取得
		std::vector<ChunkIndex> indexes;
		GetOverlappedChunks(collider->GetAABB(), indexes);

		// 取得したチャンクインデックスすべてに登録
		for (const ChunkIndex& index : indexes) {
			chunks[index].colliders.emplace_back(collider);
		}
	}

	/// <summary>
	/// 指定した2つのコライダー同士に押し出し処理が必要かどうか
	/// </summary>
	/// <param name="a">コライダー１</param>
	/// <param name="b">コライダー２</param>
	/// <returns></returns>
	bool NeedPush(ColliderBase* a, ColliderBase* b)const {
		if (a->GetPushFlg() && b->GetPushFlg()) { return true; }
		return false;
	}

	/// <summary>
	/// 重みづけの割合を計算
	/// </summary>
	/// <param name="aWeight">（in） 重み</param>
	/// <param name="bWeight">（in） 重み</param>
	/// <param name="aWeightRatio">（out） 重みの割合</param>
	/// <param name="bWeightRatio">（out） 重みの割合</param>
	void WeightRatioCalculation(unsigned char aWeight, unsigned char bWeight, float& aWeightRatio, float& bWeightRatio)const {
		// お互いの重みにおける割合を計算（相手の重み ÷ 自分と相手の重みの合計）

		// 自分と相手の重みの合計
		float abWeightSum = (float)(aWeight + bWeight);

		if (abWeightSum != 0.0f) {
			aWeightRatio = (float)bWeight / abWeightSum;
			bWeightRatio = (float)aWeight / abWeightSum;
		}
		else {
			// 両方重み０だったら半分ずつで返す（例外処理）
			aWeightRatio = bWeightRatio = 0.5f;
		}
	}

	/// <summary>
	/// 2つのコライダーを重みに合わせて押し出す
	/// </summary>
	void ApplyPush(ColliderBase* a, ColliderBase* b, const Vector3& normal, float overlap)const;

	/// <summary>
	/// 2つのコライダーを押し出しベクトルに合わせて押し出す
	/// </summary>
	void ApplyPush(ColliderBase* a, ColliderBase* b, const Vector3& overlapVec)const;

	/// <summary>
	/// 指定した片方のコライダーだけを押し出す
	/// </summary>
	void ApplyPushOneSide(ColliderBase* dynamicColl, ColliderBase* staticColl, const Vector3& overlapVec)const;

	// 押し出しベクトルをコライダーへ適用し、接地判定も行う
	void MoveCollider(ColliderBase* ownCollider, const Vector3& pushVector, const ColliderBase& otherCollider)const;
#pragma endregion

};