#include "PlayerMomentumStealAimState.h"

#include "../../../Manager/Input/InputManager.h"

#include "../../../Manager/TimeScale/TimeScale.h"

#include "../../../Manager/Collision/CurrentCollisionManager.h"
#include "../../../Manager/Camera/CurrentCamera.h"

#include "../../../Manager/Camera/CameraEvent/MomentumStealAimCameraEvent/MomentumStealAimCameraEvent.h"

#include "../../../Scene/SceneManager.h"
#include "../../../Scene/SceneBase.h"

#include "../../../Scene/Common/PostEffect/TimeFreezePostEffect/TimeFreezePostEffect.h"
#include "../../../Scene/Common/PostEffect/CRTPostEffect/CRTPostEffect.h"
#include "../../../Scene/Common/PostEffect/AimCrosshairPostEffect/AimCrosshairPostEffect.h"

#include "../../../Scene/Common/PostEffect/MenacingPostEffect/MenacingPostEffect.h"

#include "../../Common/ActorBase/ActorBase.h"

#include "../../Common/Shader/RimLightShader.h"

PlayerMomentumStealAimState::PlayerMomentumStealAimState(
	float LIMIT_TIME,

	float TIME_SCALE,

	float RAYCAST_LENGTH,

	Transform& playerTrans,
	ActorBase*& target,

	const std::function<void(void)>& playAnimeAim,

	const std::function<void(void)>& changeStateSteal,
	const std::function<void(void)>& changeStateDefault
) :
	LIMIT_TIME(LIMIT_TIME),

	TIME_SCALE(TIME_SCALE),

	RAYCAST_LENGTH(RAYCAST_LENGTH),

	playerTrans(playerTrans),
	target(target),

	playAnimeAim(playAnimeAim),

	changeStateSteal(changeStateSteal),
	changeStateDefault(changeStateDefault),

	limitTimeCounter()
{
}

void PlayerMomentumStealAimState::Enter(void)
{
	// 時間倍率を遅くする（超スロー）
	TimeScale::GetIns().ChangeTimeScale(TIME_SCALE);

	// 専用のポストエフェクトを発生
	SceneManager::GetIns().GetScene()->AddPostEffect(new TimeFreezePostEffect());
	SceneManager::GetIns().GetScene()->AddPostEffect(new CRTPostEffect(1.0f, 0.05f));
	SceneManager::GetIns().GetScene()->AddPostEffect(new AimCrosshairPostEffect());
	//SceneManager::GetIns().GetScene()->AddPostEffect(new MenacingPostEffect(2.0f, 0.5f, 1.0f));

	// 専用のカメライベントを発生
	CurrentCamera::Get().StartEvent(new MomentumStealAimCameraEvent(playerTrans.pos, playerTrans.rotation));

	// アニメーション再生
	playAnimeAim();

	// 制限時間計測初期化
	limitTimeCounter = 0.0f;
}

void PlayerMomentumStealAimState::Update(void)
{
#pragma region ターゲット選択

	// レイキャスト
	RaycastHit result =
		CurrentCollisionManager::Get().Raycast(
			CurrentCamera::Get().GetPos(),
			CurrentCamera::Get().GetRotation().Rotate(Vector3::Zonly(1.0f)),
			RAYCAST_LENGTH
		);

	// 当たっていたら
	if (result.IsHit()) {
		// 新しいターゲットへ
		ChangeTarget(result.collider->GetActor());
	}
	// 当たっていなかったら
	else {
		// 何も選択していない状態
		ChangeTarget(nullptr);
	}

#pragma endregion

	// アップトリガーで選択確定
	if (Input::GetIns().GetInfo(KEY_TYPE::PlayerMomentumSteal).up) {

		// 実際に勢いを奪う状態へ遷移
		changeStateSteal();

		return;
	}

	// 制限時間計測
	limitTimeCounter++;

	// 制限時間に達していたら
	if (limitTimeCounter >= LIMIT_TIME) {

		// 通常状態へ遷移
		changeStateDefault();
		return;
	}
}

void PlayerMomentumStealAimState::Exit(void)
{
#pragma region プレイヤーの回転をもどす
	// カメラの回転からY軸回転だけを取得
	const Vector3 angle = playerTrans.rotation.ToEulerXZY();

	// X・Z軸回転を打ち消し、Y軸回転だけを残す
	playerTrans.rotation = Quaternion::FromEulerXZY(Vector3(0.0f, angle.y, 0.0f));
#pragma endregion

	// ターゲットの選択状態を解除
	if (target) {
		target->OnUntargeted();
		target->VelSteal();
	}

	// 時間倍率をもとにもどす
	TimeScale::GetIns().ChangeDefaultTimeScale();

	// ポストエフェクトを破棄
	SceneManager::GetIns().GetScene()->RemovePostEffect<AimCrosshairPostEffect>();
	SceneManager::GetIns().GetScene()->RemovePostEffect<CRTPostEffect>();
	SceneManager::GetIns().GetScene()->RemovePostEffect<TimeFreezePostEffect>();

	// カメライベントを破棄
	CurrentCamera::Get().EndEvent();
}

void PlayerMomentumStealAimState::ChangeTarget(ActorBase* target)
{
	// 同じだったら何もしない
	if (this->target == target) { return; }

	// 過去のターゲットの選択状態を解除
	if (this->target) { this->target->OnUntargeted(); }

	// 新しいターゲットに
	this->target = target;

	// 新しいターゲットの選択状態を有効に
	if (this->target) { this->target->OnTargeted(); }
}