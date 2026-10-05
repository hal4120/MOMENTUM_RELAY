#pragma once

#include <vector>
#include <functional>

class CharacterStateBase
{
public:
	CharacterStateBase() = default;
	virtual ~CharacterStateBase() = default;

	// 自分の状態に遷移する条件関数
	virtual void OwnStateConditionUpdate(void) {}

	// 他の状態に遷移する条件関数を追加する関数
	void AddOtherStateCondition(const std::function<void(void)>& func) {
		OtherStateConditions.push_back(func);
	}

	// 他の状態に遷移する条件関数をすべて呼び出す関数
	void OtherStateConditionsUpdate(void) {
		for (std::function<void(void)>& func : OtherStateConditions) { func(); }
	}

	// 状態遷移後1度行う初期化処理
	virtual void Enter(void) {}
	// 更新処理
	virtual void Update(void) {}
	// 状態遷移前1度行う終了処理
	virtual void Exit(void) {}

	// 状態関係なく呼び出す処理
	virtual void AlwaysUpdate(void) {}

	// 自分の状態に遷移させる関数のポインタを設定する
	void SetOwnChangeStatePtr(std::function<void(void)>ownChangeStatePtr) { this->ownChangeStatePtr = ownChangeStatePtr; }

	// 現在自分の状態かどうかを返す関数のポインタを設定する
	void SetIsOwnStatePtr(std::function<bool(void)>isOwnStatePtr) { this->isOwnStatePtr = isOwnStatePtr; }

protected:

	// 自分の状態に遷移させる
	void OwnChangeState(void)const { ownChangeStatePtr(); }

	// 現在自分の状態かどうか
	bool IsOwnState(void)const { return isOwnStatePtr(); }

private:

	// 自分の状態に遷移させる関数のポインタ
	std::function<void(void)> ownChangeStatePtr;

	// 現在自分の状態かどうかを返す関数のポインタ
	std::function<bool(void)> isOwnStatePtr;

	// 他の状態に遷移する条件関数のポインタを格納するベクター
	std::vector<std::function<void(void)>> OtherStateConditions;
};