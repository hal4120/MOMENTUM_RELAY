#pragma once

#include <deque>

class TimeScale
{
private:

	// インスタンス
	static TimeScale* ins;

	// コンストラクタ
	TimeScale();
	// デストラクタ
	~TimeScale() = default;

public:

#pragma region シングルトン定義

	// 生成
	static void CreateIns(void) { if (ins == nullptr) ins = new TimeScale(); }

	// 取得
	static TimeScale& GetIns(void) { return *ins; }

	// 破棄
	static void DeleteIns(void) { if (ins != nullptr) { delete ins; ins = nullptr; } }

#pragma endregion

	// 更新処理
	void Update(void);

#pragma region 機能

	// 現在の時間倍率を取得する
	float GetTimeScale(void)const { return timeScale; }

	// 現在の時間倍率を取得する
	static float Get(void) { return ins->timeScale; }

	/// <summary>
	/// 時間倍率を変更する
	/// </summary>
	/// <param name="targetTimeScale">倍率指定</param>
	/// <param name="changeSpeed">変更にかける時間（デフォルトは即時）</param>
	void ChangeTimeScale(float targetTimeScale, unsigned short changeTime = 0);

	/// <summary>
	/// 時間倍率をデフォルト値に変更する（デフォルト値 = 1）
	/// </summary>
	/// <param name="changeSpeed">変更にかける時間（デフォルトは即時）</param>
	void ChangeDefaultTimeScale(unsigned short changeTime = 0) { ChangeTimeScale(1.0f, changeTime); }

	/// <summary>
	/// 指定の倍率で、指定の時間更新した後、元の倍率に戻す
	/// </summary>
	/// <param name="targetTimescale">倍率指定</param>
	/// <param name="temporaryTime">指定した倍率で、どのくらい止めるか</param>
	/// <param name="changeTime">変更にかける時間（デフォルトは即時）</param>
	void TemporaryScale(float targetTimescale, unsigned short temporaryTime, unsigned short changeTime = 0);

	/// <summary>
	/// ヒットストップ
	/// </summary>
	/// <param name="time">ヒットストップさせる時間</param>
	void HitStop(unsigned short time = 20) { TemporaryScale(0.0f, time); }

#pragma endregion
	
private:

	// 時間の倍率
	float timeScale;

#pragma region 時間倍率変更に関する変数

	// 時間倍率変更中かどうか
	bool isChangingTimeScale;

	// 時間倍率変更前の値
	float beforeTimeScale;
	// 時間倍率変更の目標値
	float targetTimeScale;

	// 時間倍率変更にかける時間
	unsigned short changeTime;
	// 時間倍率変更にかけた時間
	unsigned short changeTimeCounter;

#pragma endregion

#pragma region 一時的な時間倍率変更に関する変数

	// 1つの要件構造体
	struct TemporaryOrder {

		// 元の倍率
		float beforeTimeScale;

		// この時間倍率変更にかける時間
		unsigned short changeTime;

		// 一時的な倍率変更の時間
		unsigned short counter;

		TemporaryOrder(float beforeTimeScale, unsigned short changeTime, unsigned short counter) :
			beforeTimeScale(beforeTimeScale),
			changeTime(changeTime),
			counter(counter)
		{
		}
	};

	// 一時的な時間倍率変更の要件スタック
	std::deque<TemporaryOrder> temporaryOrderStack;

#pragma endregion

};