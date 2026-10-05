#include "CharacterBase.h"

#include <stdexcept>

#include "../../../Application/Application.h"

#include "CharacterStateBase.h"

#include "../AnimationController/AnimationController.h"

CharacterBase::CharacterBase() :
	ActorBase(),

	state(-1),
	stateMap(),

	anime(nullptr)
{
}

CharacterBase::CharacterBase(const std::string& parameterPath):
	ActorBase(parameterPath),

	state(0),
	stateMap(),

	anime(nullptr)
{
}

void CharacterBase::BaseUpdate(void)
{
	// ステートの更新
	if (!stateMap.empty()) {
		stateMap.at(state)->OtherStateConditionsUpdate();
		stateMap.at(state)->Update();
		for (std::pair<const int, CharacterStateBase*>& s : stateMap) { s.second->AlwaysUpdate(); }
	}

	// アニメーション更新
	if (anime) { anime->Update(); }
}

void CharacterBase::BaseRelease(void)
{
	// ステート管理用マップの解放
	for (auto& s : stateMap) {
		if (s.second) { delete s.second; s.second = nullptr; }
	}
	stateMap.clear();

	// アニメーションコントローラーの解放（使われていたら）
	if (anime) {
		anime->Release();
		delete anime;
		anime = nullptr;
	}
}

#pragma region ステート管理関数

void CharacterBase::AddState(int stateNum, CharacterStateBase* stateIns)
{
	// 自分の状態に遷移させる関数のポインタをセットする（共通初期設定）
	stateIns->SetOwnChangeStatePtr([this, stateNum]() { ChangeState(stateNum); });

	// 現在自分の状態かどうかを返す関数のポインタをセットする（共通初期設定）
	stateIns->SetIsOwnStatePtr([this, stateNum]() { return state == stateNum; });

	// 格納
	stateMap.emplace(stateNum, stateIns);
}

void CharacterBase::RegisterStateTransition(int beforeState, int afterState)
{
	GetStateIns(beforeState).AddOtherStateCondition([this, afterState]() { GetStateIns(afterState).OwnStateConditionUpdate(); });
}

void CharacterBase::ChangeState(int nextState)
{
	// 遷移前のステートの終了処理を呼び出す
	if (stateMap.contains(state)) { stateMap.at(state)->Exit(); }

	// 遷移
	state = nextState;

	// 遷移後のステートの初期化処理を呼び出す
	if (stateMap.contains(state)) { stateMap.at(state)->Enter(); }
}

CharacterStateBase& CharacterBase::GetStateIns(int state)
{
	auto it = stateMap.find(state);
	if (it != stateMap.end()) { return *(it->second); }
	else { throw std::runtime_error("指定のステートインスタンスが見つかりません"); }
}

#pragma endregion


#pragma region アニメーションコントローラー

void CharacterBase::CreateAnimationController(void) { if (anime == nullptr) anime = new AnimationController(trans.model); }

void CharacterBase::AddInFbxAnimation(int inFbxMaxIndex, float speed, const bool* const loop)
{
	for (int index = 0; index < inFbxMaxIndex; index++) {
		anime->AddInFbx(index, speed, (loop != nullptr) ? loop[index] : true, index);
	}
}

void CharacterBase::AddInFbxAnimation(int inFbxMaxIndex, const float* const speed, const bool* const loop)
{
	for (int index = 0; index < inFbxMaxIndex; index++) {
		anime->AddInFbx(index, speed[index], (loop != nullptr) ? loop[index] : true, index);
	}
}

void CharacterBase::AddAnimation(int index, float speed, bool loop, const char* filePath)
{
	anime->Add(index, speed, loop, filePath);
}

void CharacterBase::AnimePlay(int type, signed char loop)
{
	anime->Play(type, loop);
}

bool CharacterBase::IsAnimeEnd(void) const { return anime->IsAnimEnd(); }

float CharacterBase::GetAnimeRatio(void) const { return anime->GetAnimeRatio(); }

float CharacterBase::GetAnimeTotalTime(void) const { return anime->GetAnimeTotalTime(); }

int CharacterBase::GetAnimePlayType(void)const { return anime->GetAnimePlayType(); }

float CharacterBase::GetAnimeStep(void)const { return anime->GetAnimeStep(); }

void CharacterBase::SetAnimeStep(float step) { anime->SetAnimeStep(step); }

#pragma endregion