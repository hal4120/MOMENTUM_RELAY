#include "PlayerMoveState.h"

#include "../../../Common/Vector3.h"

#include "../../../Manager/TimeScale/TimeScale.h"

#include "../../../Manager/Input/InputManager.h"
#include "../../../Manager/Camera/CurrentCamera.h"

#include "../../Common/MotionController/GroundMotionController.h"

PlayerMoveState::PlayerMoveState(
	float MOVE_SPEED, float DASH_SPEED_RATE, float DASH_STAMINA_MAX,

	GroundMotionController& motion,

	std::function<void(void)> playAnimeWalk,
	std::function<void(void)> playAnimeRun
) :
	MOVE_SPEED(MOVE_SPEED), DASH_SPEED_RATE(DASH_SPEED_RATE), DASH_STAMINA_MAX(DASH_STAMINA_MAX),

	motion(motion),

	playAnimeWalk(playAnimeWalk),
	playAnimeRun(playAnimeRun),

	isDash(false),
	dashStamina(DASH_STAMINA_MAX),
	isTired(false)
{
	motion.SetMoveSpeed(MOVE_SPEED);
}

void PlayerMoveState::OwnStateConditionUpdate(void)
{
	// 移動入力があれば、自分の状態に遷移
	if (InputVec() != 0.0f) { OwnChangeState(); }
}

void PlayerMoveState::Enter(void)
{
	// ダッシュ
	if (Input::GetIns().GetInfo(KEY_TYPE::PlayerDash).now) { playAnimeRun(); }
	// 非ダッシュ
	else { playAnimeWalk(); }
}

void PlayerMoveState::Update(void)
{
	// 移動方向入力を取得
	Vector3 inputVec = InputVec();

	// ダッシュフラグを立てる(移動中じゃなければ、ダッシュフラグは変えない)
	isDash = (isTired) ? false : Input::GetIns().GetInfo(KEY_TYPE::PlayerDash).now && inputVec != 0.0f;

	// 移動量の最大値を更新する
	motion.SetMoveSpeed((isDash) ? MOVE_SPEED * DASH_SPEED_RATE : MOVE_SPEED * inputVec.Length());

	// 最終的に入力があれば加速度に加算する
	if (inputVec != 0.0f) {

		// 移動方向をカメラで回転させる
		inputVec.TransMatOwn(MGetRotY(CurrentCamera::Get().GetRotation().GetYaw()));

		// 移動
		motion.ApplyInput(inputVec);

		// ダッシュスタミナを更新 / アニメーションを更新
		if (isDash) {

			// ダッシュしているときはスタミナを減らす
			dashStamina -= TimeScale::Get();
			if (dashStamina <= 0.0f) {
				dashStamina = 0.0f;

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
	isDash = false;
	motion.SetMoveSpeed(MOVE_SPEED);
}

void PlayerMoveState::AlwaysUpdate(void)
{
	// ダッシュしていないときはスタミナを回復させる
	if (!isDash) {
		dashStamina += TimeScale::Get();
		if (dashStamina >= DASH_STAMINA_MAX) {
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
