#include "PlayerMoveState.h"

#include "../../../Common/Vector3.h"

#include "../../../Manager/Input/InputManager.h"
#include "../../../Manager/Camera/CurrentCamera.h"

PlayerMoveState::PlayerMoveState(
	float ACCEL_MAX, float DASH_SPEED_RATE, short DASH_STAMINA_MAX,

	std::function<void(const Vector3&)> moveAccel,

	float& accelMax,

	std::function<void(void)> playAnimeWalk,
	std::function<void(void)> playAnimeRun
) :
	ACCEL_MAX(ACCEL_MAX), DASH_SPEED_RATE(DASH_SPEED_RATE), DASH_STAMINA_MAX(DASH_STAMINA_MAX),

	moveAccel(moveAccel),

	accelMax(accelMax),

	playAnimeWalk(playAnimeWalk),
	playAnimeRun(playAnimeRun),

	isDash(false),
	dashStamina(DASH_STAMINA_MAX),
	isTired(false)
{
}

void PlayerMoveState::OwnStateConditionUpdate(void)
{
	// 移動入力があれば、自分の状態に遷移
	if (InputVec() != 0.0f) { OwnChangeState(); }
}

void PlayerMoveState::Enter(void)
{
	// 非ダッシュ
	if (Input::GetIns().GetInfo(KEY_TYPE::PlayerDash).now) { playAnimeRun(); }
	// ダッシュ
	else { playAnimeWalk(); }
}

void PlayerMoveState::Update(void)
{
	// 移動方向入力を取得
	Vector3 inputVec = InputVec();

	// ダッシュフラグを立てる(移動中じゃなければ、ダッシュフラグは変えない)
	isDash = (isTired) ? false : Input::GetIns().GetInfo(KEY_TYPE::PlayerDash).now && inputVec != 0.0f;

	// 移動量の最大値を更新する
	accelMax = (isDash) ? ACCEL_MAX * DASH_SPEED_RATE : ACCEL_MAX * inputVec.Length();

	// 最終的に入力があれば加速度に加算する
	if (inputVec != 0.0f) {

		// 移動方向をカメラで回転させる
		inputVec.TransMatOwn(MGetRotY(CurrentCamera::Get().GetAngle().y));

		// 移動
		moveAccel(inputVec);

		// ダッシュスタミナを更新 / アニメーションを更新
		if (isDash) {

			// ダッシュしているときはスタミナを減らす
			if (--dashStamina < 0) {
				dashStamina = 0;

				// 息切れ
				isTired = true;
			}

			// ダッシュしているときは走るアニメーションにする
			playAnimeRun();
		}
		else {
			// ダッシュしていないときは歩くアニメーションにする
			playAnimeWalk();
		}
	}
}

void PlayerMoveState::Exit(void)
{
	accelMax = ACCEL_MAX;
}

void PlayerMoveState::AlwaysUpdate(void)
{
	// ダッシュしていないときはスタミナを回復させる
	if (!isDash) {
		if (++dashStamina > DASH_STAMINA_MAX) {
			dashStamina = DASH_STAMINA_MAX;

			// 息切れ回復
			isTired = false;
		}
	}
}

Vector3 PlayerMoveState::InputVec(void) const
{
	// 返却用一時変数
	Vector3 vec = Vector3();

	// コントローラーの入力を取得
	vec = Input::GetIns().GetLeftStickVec(false).ToVector3XZInvertY();

	// 入力がなければ次にキーボードの入力を取得
	if (vec == 0.0f) {
		if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveRight).now) { vec.x++; }
		if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveLeft).now) { vec.x--; }
		if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveFront).now) { vec.z++; }
		if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMoveBack).now) { vec.z--; }
		vec.Normalize();
	}

	return vec;
}
