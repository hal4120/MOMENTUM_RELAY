#pragma once

#include <stdexcept>

#include "CollisionManager.h"

class CurrentCollisionManager
{
public:

	// 取得
	static CollisionManager& Get(void) {
		if (collision == nullptr) { throw std::runtime_error("当たり判定管理クラスが存在しない状態でCurrentCollisionManagerへの参照がありました"); }
		return *collision;
	}
	// 設定
	static void Set(CollisionManager* currentCollisionManager) { collision = currentCollisionManager; }

private:

	// 現在の当たり判定管理クラス
	static CollisionManager* collision;

	CurrentCollisionManager() = default;
	~CurrentCollisionManager() = default;
};