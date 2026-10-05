#pragma once

#include "../ActorBase/ActorBase.h"

#include <map>

class CharacterStateBase;

class AnimationController;

class CharacterBase : public ActorBase
{
public:

	CharacterBase();
	CharacterBase(const std::string& parameterPath);

	virtual ~CharacterBase()override = default;

	// ステートの取得
	int GetState(void)const { return state; }

private:

	// アニメーションコントローラーのインスタンス
	AnimationController* anime;

	// ステート管理用変数
	int state;

	// ステート管理用マップ（キー：ステート番号、値：状態クラスのポインタ）
	std::map<int, CharacterStateBase*> stateMap;

	// ステートの追加
	void AddState(int stateNum, CharacterStateBase* stateIns);

	// 状態番号を指定して自動遷移判定を登録する
	void RegisterStateTransition(int beforeState, int afterState);

	// 指定のステートへ遷移
	void  ChangeState(int nextState);

	// 指定のステートインスタンスをゲットする関数
	CharacterStateBase& GetStateIns(int state);

	// キャラクターベース固有の更新処理
	void BaseUpdate(void) override final;
	// キャラクターベース固有の解放処理
	void BaseRelease(void) override final;

protected:

#pragma region ステート管理関数

	// ステートの追加
	template<typename StateEnum>
	void AddState(StateEnum stateNum, CharacterStateBase* stateIns) {
		AddState(static_cast<int>(stateNum), stateIns);
	}

	// 状態間の自動遷移判定を登録する
	template<typename StateEnum>
	void RegisterStateTransition(StateEnum beforeState, StateEnum afterState) {
		RegisterStateTransition(static_cast<int>(beforeState), static_cast<int>(afterState));
	}

	// 指定のステートへ遷移
	template<typename StateEnum>
	void  ChangeState(StateEnum nextState) { ChangeState(static_cast<int>(nextState)); }

	// 指定のステートインスタンスをゲットする関数
	template<typename StateEnum>
	CharacterStateBase& GetStateIns(StateEnum state) { return GetStateIns(static_cast<int>(state)); }

#pragma endregion

#pragma region アニメーションコントローラー

	// アニメーションコントローラーの作成
	void CreateAnimationController(void);
	
	/// <summary>
	/// モデルにくっついてるFBXアニメーションを全部登録する
	/// </summary>
	/// <param name="inFbxMaxIndex">くっついてるアニメーションの数</param>
	/// <param name="speed">再生速度</param>
	/// <param name="loop">ループ再生フラグ配列（指定なしで全てループ再生有効で登録）</param>
	void AddInFbxAnimation(int inFbxMaxIndex, const float speed, const bool* const loop = nullptr);

	/// <summary>
	/// モデルにくっついてるFBXアニメーションを全部登録する
	/// </summary>
	/// <param name="inFbxMaxIndex">くっついてるアニメーションの数</param>
	/// <param name="speed">再生速度配列</param>
	/// <param name="loop">ループ再生フラグ配列（指定なしで全てループ再生有効で登録）</param>
	void AddInFbxAnimation(int inFbxMaxIndex, const float* const speed, const bool* const loop = nullptr);

	/// <summary>
	/// 別ファイルに保存されているFBXアニメーションを登録する
	/// </summary>
	/// <param name="index">参照番号</param>
	/// <param name="speed">再生速度</param>
	/// <param name="loop">ループ再生フラグ</param>
	/// <param name="filePath">パス</param>
	void AddAnimation(int index, float speed, bool loop, const char* filePath);

	/// <summary>
	/// アニメーション再生
	/// </summary>
	/// <param name="type">参照番号</param>
	/// <param name="loop">ループ再生フラグ（指定なしで登録された情報で再生）</param>
	virtual void AnimePlay(int type, signed char loop = -1);
	
	/// <summary>
	/// アニメーション再生
	/// </summary>
	/// <param name="type">参照番号</param>
	/// <param name="loop">ループ再生フラグ（指定なしで登録された情報で再生）</param>
	template<typename AnimeEnum>
	void AnimePlay(AnimeEnum type, signed char loop = -1) { AnimePlay(static_cast<int>(type), loop); }

	// アニメーション終了チェック（true = 最後まで再生し終わっている、false = まだ再生中）
	bool IsAnimeEnd(void)const;

	// アニメーションの再生比率を取得（0.0f～1.0f）
	float GetAnimeRatio(void)const;

	// アニメーションの再生時間を取得
	float GetAnimeTotalTime(void)const;

	// アニメーションの再生タイプを取得（0 = ループ、1 = 1回再生）
	int GetAnimePlayType(void)const;

	// アニメーションの再生時間を取得（0.0f～再生時間）
	float GetAnimeStep(void)const;

	// アニメーションの再生時間を設定（0.0f～再生時間）
	void SetAnimeStep(float step);
	
#pragma endregion
};