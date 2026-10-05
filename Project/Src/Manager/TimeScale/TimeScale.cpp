#include "TimeScale.h"

TimeScale* TimeScale::ins = nullptr;

TimeScale::TimeScale() :
	timeScale(1.0f),

	isChangingTimeScale(false),

	beforeTimeScale(1.0f),
	targetTimeScale(1.0f),

	changeTime(0),
	changeTimeCounter(0),

	temporaryOrderStack()
{
}

void TimeScale::Update(void)
{
	// 時間倍率変更中
	if (isChangingTimeScale) {

		// 時間倍率変更カウンターを進める
		changeTimeCounter++;

		// 時間倍率変更中
		if (changeTimeCounter < changeTime) {

			// 進行割合を求める
			float t = static_cast<float>(changeTimeCounter) / static_cast<float>(changeTime);

			// 割合に応じて時間倍率を変更する
			timeScale = beforeTimeScale + (targetTimeScale - beforeTimeScale) * t;
		}
		// 時間倍率変更完了
		else {


			// 時間倍率変更中フラグを下ろす
			isChangingTimeScale = false;

			// 時間倍率を変更する
			timeScale = targetTimeScale;

			// 変更前の倍率の保持を消去
			beforeTimeScale = targetTimeScale;

			// カウンターをリセットする
			changeTimeCounter = 0;

			// 変更にかける時間の設定を消去
			changeTime = 0;
		}

		// 時間倍率変更中はその他の処理はしない
		return;
	}

	// 一時的な時間倍率変更中
	if (!temporaryOrderStack.empty()) {

		// 一番最後に追加された要件を消化していく
		TemporaryOrder& backOrder = temporaryOrderStack.back();

		// 一時的な時間倍率変更カウンターを進める
		backOrder.counter--;

		// 一時的な時間倍率変更終了
		if (backOrder.counter <= 0.0f) {

			// 時間倍率を元に戻す（元に戻す際は、変更したときと同じ時間をかけて戻す）
			ChangeTimeScale(backOrder.beforeTimeScale, backOrder.changeTime);

			// 要件を削除
			temporaryOrderStack.pop_back();
		}
	}
}

void TimeScale::ChangeTimeScale(float targetTimeScale, unsigned short changeTime)
{
	if (isChangingTimeScale) { return; }

	// 時間変更のフラグを立てる
	isChangingTimeScale = changeTime > 0.0f;

	// 時間倍率変更前の倍率を保持
	beforeTimeScale = timeScale;

	// 変更に時間をかける(1フレーム以上かける)場合
	if (isChangingTimeScale) {

		// 時間倍率変更の目標値の設定
		this->targetTimeScale = targetTimeScale;

		// 変更にかける時間の設定
		this->changeTime = changeTime;

		// 変更カウンターを初期化
		changeTimeCounter = 0;
	}
	// 即時変更する場合
	else {
		// 直接時間倍率を変更
		timeScale = targetTimeScale;
	}
}

void TimeScale::TemporaryScale(float targetTimescale, unsigned short temporaryTime, unsigned short changeTime)
{
	if (isChangingTimeScale) { return; }

	// 変更の処理
	ChangeTimeScale(targetTimescale, changeTime);

	// 1つの要件を保存
	temporaryOrderStack.emplace_back(TemporaryOrder(beforeTimeScale, changeTime, temporaryTime));
}
