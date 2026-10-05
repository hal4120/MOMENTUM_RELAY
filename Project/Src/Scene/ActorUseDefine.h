#pragma once

#include <vector>

#include "../Object/Common/ActorBase/ActorBase.h"

// 配列の中から特定のオブジェクトを探す（単体）
template<typename T = ActorBase>
T* ActorSerch(std::vector<ActorBase*>& actors) {
	for (auto* obj : actors) {
		if (T* cast = dynamic_cast<T*>(obj)) { return cast; }
	}
	return nullptr;
}

// 配列の中から特定のオブジェクトを探す（複数）
template<typename T = ActorBase>
std::vector<T*> ActorArraySerch(std::vector<ActorBase*>& actors) {
	std::vector<T*> objArray;
	for (auto* obj : actors) {
		if (T* cast = dynamic_cast<T*>(obj)) { objArray.emplace_back(cast); }
	}
	return objArray;
}