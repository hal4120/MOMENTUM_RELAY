#pragma once

#include <unordered_map>
#include<vector>
#include <cmath>
#include <unordered_set>

class ColliderBase;

// チャンクサイズ
static constexpr float CHUNK_SIZE = 500.0f;

// チャンク座標
struct ChunkIndex {
	int x = 0, y = 0, z = 0;

	bool operator==(const ChunkIndex& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
};

// unordered_map 用ハッシュ
struct ChunkIndexHash {
	std::size_t operator()(const ChunkIndex& index) const {
		std::size_t h1 = std::hash<int>()(index.x);
		std::size_t h2 = std::hash<int>()(index.y);
		std::size_t h3 = std::hash<int>()(index.z);

		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}
};

// 1チャンクに所属するデータ
struct ChunkData {
	std::vector<ColliderBase*> colliders;
};

// チャンク管理マップ
using ChunkMap = std::unordered_map<ChunkIndex, ChunkData, ChunkIndexHash>;

// 座標からチャンク番号に変換する関数
static int ToChunkIndex(float pos, float chunkSize) {
	return static_cast<int>(std::floor(pos / chunkSize));
}

// 1グループが持つデータ
struct ColliderGroupData {
	// コライダー本体
	std::vector<ColliderBase*> colliders;

	// 静的コライダー参照
	ChunkMap staticChunks;

	// 動的コライダー参照
	ChunkMap dynamicChunks;
};

// 重複判定防止のペアキー(どのセットが判定を取ったか記録するため)
struct ColliderPairKey {
	ColliderBase* a = nullptr;
	ColliderBase* b = nullptr;

	ColliderPairKey(ColliderBase* lhs, ColliderBase* rhs) {
		if (lhs < rhs) { a = lhs; b = rhs; }
		else { a = rhs; b = lhs; }
	}

	bool operator==(const ColliderPairKey& other) const {
		return a == other.a && b == other.b;
	}
};

// 重複判定防止のペアキーのハッシュ
struct ColliderPairKeyHash {
	std::size_t operator()(const ColliderPairKey& key) const {
		return std::hash<ColliderBase*>()(key.a) ^ (std::hash<ColliderBase*>()(key.b) << 1);
	}
};