#include "Player.h"

#include "../../Utility/Utility.h"

#include "../../Scene/SceneManager.h"
#include "../../Scene/SceneBase.h"

#include "../Common/Collider/CapsuleCollider.h"

#include "../../Scene/Common/PostEffect/FocusLinesPostEffect/FocusLinesPostEffect.h"
#include "../../Scene/Common/PostEffect/CRTPostEffect/CRTPostEffect.h"

#include "../Common/Shader/DefaultShader.h"
#include "../Common/Shader/RimLightShader.h"
#include "../Common/Shader/WaterShader.h"
#include "../Common/Shader/ModelDamageFlashShader.h"

#include "Weapon/PlayerCollOperator.h"
#include "Weapon/PlayerMomentumQuickStealFind.h"

#include "State/PlayerIdleState.h"
#include "State/PlayerMoveState.h"
#include "State/PlayerJumpState.h"
#include "State/PlayerAttackState.h"
#include "State/PlayerRollingState.h"

#include "State/PlayerMomentumInputState.h"
#include "State/PlayerMomentumQuickState.h"
#include "State/PlayerMomentumAimState.h"

Player::Player() :
	CharacterBase(),

	target(nullptr)
{
}

void Player::Load(void)
{
#pragma region オブジェクト設定

	// 動的オブジェクトとしての処理を有効にする
	SetDynamicFlg(true);

	// 重力を有効にする
	SetGravityFlg(true);

	// 当たり判定による押し出しを有効にする
	SetPushFlg(true);

	// 押し出しだしによる重みを設定
	SetPushWeight(50);

#pragma endregion

#pragma region モデル設定

	// モデルの読み込み
	trans.LoadModel("Player/Player");

	// モデルのスケール設定
	trans.scale = 1;

	// モデルの中心点のズレの補正
	trans.centerDiff = Vector3(0.0f, -82.5f, 0.0f) * trans.scale;

	// モデルの角度のズレの補正
	trans.SetLocalRotation(Quaternion::FromRotationY(Deg2Rad(180.0f)));

#pragma endregion

#pragma region アニメーション読み込み

	// アニメーションコントローラーの生成
	CreateAnimationController();

	// アニメーションの読み込み
	for (int i = 0; i < (int)ANIME_TYPE::Max; i++) {
		AddAnimation(
			i,
			ANIME_SPEED_TABLE[i],
			ANIME_LOOP_TABLE[i],
			(ANIME_FILE_PATH + ANIME_NAME_TABLE[i] + ANIME_FILE_EXTENSION).c_str()
		);
	}

	SetPositionCorrectionFrame("mixamorig:Hips");
#pragma endregion

#pragma region コライダーの生成

	AddCollider(
		new CapsuleCollider(
			COLLIDER_TAG::Player,
			Vector3::Yonly(57.5f) * trans.scale,
			Vector3::Yonly(-57.5f) * trans.scale,
			25.0f * trans.scale.MaxElementF()
		)
	);

#pragma endregion

#pragma region 下位アクターの生成

	PlayerCollOperator* kickDownAttackCollOperator =
		new PlayerCollOperator(
			COLLIDER_TAG::PlayerKickDownAttack,
			10.0f,
			Vector3::Zonly(50.0f),
			trans
		);
	AddChildActor(kickDownAttackCollOperator);

	PlayerMomentumQuickStealFind* momentumQuickStealFind = new PlayerMomentumQuickStealFind(trans);
	AddChildActor(momentumQuickStealFind);

#pragma endregion

#pragma region 状態設定

	// 待機状態
	AddState(
		STATE::Idle,
		new PlayerIdleState([&]() { AnimePlay(ANIME_TYPE::Idle); })
	);

	// 移動状態
	AddState(
		STATE::Move,
		new PlayerMoveState(
			10.0f, 1.5f, 300,
			std::bind(&Player::MoveAccel, this, std::placeholders::_1),
			ACCEL_MAX,
			[&]() { AnimePlay(ANIME_TYPE::Walk); },
			[&]() { AnimePlay(ANIME_TYPE::Run); }
		)
	);

	// ジャンプ状態
	AddState(
		STATE::Jump,
		new PlayerJumpState(
			20.0f, velocity.y, isGround,
			std::bind(&Player::MoveAccel, this, std::placeholders::_1),
			[&]() { AnimePlay(ANIME_TYPE::JumpStart); },
			[&]() { AnimePlay(ANIME_TYPE::JumpLoop); },
			[&]() { AnimePlay(ANIME_TYPE::JumpEnd); },
			std::bind(&Player::IsAnimeEnd, this),
			[&]() { ChangeState(STATE::Idle); }
		)
	);

	// 攻撃状態
	AddState(
		STATE::Attack,
		new PlayerAttackState(
			0.5f, 0.7f,
			*kickDownAttackCollOperator,
			[&]() { AnimePlay(ANIME_TYPE::Totta); },
			std::bind(&Player::GetAnimeRatio, this),
			[&]() { ChangeState(STATE::Idle); }
		)
	);

	// 回避状態
	AddState(
		STATE::Rolling,
		new PlayerRollingState(
			10.0f,
			100,
			0.25f, 0.55f,
			trans.rotation,
			std::bind(&Player::MoveAccel, this, std::placeholders::_1),
			ACCEL_MAX,
			isInvincble,
			[&]() { AnimePlay(ANIME_TYPE::Rolling); },
			[&]() { return GetAnimeRatio(); },
			[&]() { ChangeState(STATE::Idle); }
		)
	);

	// 勢いを奪う行動の入力管理状態
	AddState(
		STATE::MomentumStealInput,
		new PlayerMomentumInputState(
			0.0f,
			10.0f,
			std::bind(&Player::IsSteal,this),
			[&]() { ChangeState(STATE::MomentumStealQuick); },
			[&]() { ChangeState(STATE::MomentumStealAim); }
		)
	);

	// 勢いを奪う行動の自動選択中状態
	AddState(
		STATE::MomentumStealQuick,
		new PlayerMomentumQuickState(
			5.0f,
			std::bind(&Player::IsSteal, this),
			this,
			target,
			*momentumQuickStealFind,
			[&]() { ChangeState(STATE::MomentumStealAim); },
			[&]() { ChangeState(STATE::Idle); }
		)
	);

	// 勢いを奪う行動のエイム選択中状態
	AddState(
		STATE::MomentumStealAim,
		new PlayerMomentumAimState(
			300.0f,
			0.01f,
			1000.0f,
			trans,
			target,
			[&]() { AnimePlay(ANIME_TYPE::MomentumStealAim, false); },
			[&]() { ChangeState(STATE::Idle); },
			[&]() { ChangeState(STATE::Idle); }
		)
	);

	// 「待機状態」->「移動状態」の自動遷移登録
	RegisterStateTransition(STATE::Idle, STATE::Move);
	// 「移動状態」->「待機状態」の自動遷移登録
	RegisterStateTransition(STATE::Move, STATE::Idle);

	// 「待機状態」->「ジャンプ状態」の自動遷移登録
	RegisterStateTransition(STATE::Idle, STATE::Jump);
	// 「移動状態」->「ジャンプ状態」の自動遷移登録
	RegisterStateTransition(STATE::Move, STATE::Jump);

	// 「待機状態」->「攻撃状態」の自動遷移登録
	RegisterStateTransition(STATE::Idle, STATE::Attack);
	// 「移動状態」->「攻撃状態」の自動遷移登録
	RegisterStateTransition(STATE::Move, STATE::Attack);

	// 「待機状態」->「回避状態」の自動遷移登録
	RegisterStateTransition(STATE::Idle, STATE::Rolling);
	// 「移動状態」->「回避状態」の自動遷移登録
	RegisterStateTransition(STATE::Move, STATE::Rolling);

	// 「待機状態」->「勢いを奪う状態」の自動遷移登録
	RegisterStateTransition(STATE::Idle, STATE::MomentumStealInput);
	// 「移動状態」->「勢いを奪う状態」の自動遷移登録
	RegisterStateTransition(STATE::Move, STATE::MomentumStealInput);

#pragma endregion
}

void Player::DefaultCreateShader(void)
{
	// シェーダー登録
	CreateShader(new DefaultShader());
}

void Player::OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)
{
	if (other.GetTag() == COLLIDER_TAG::Stage) {
	}
}

void Player::SubOnPushed(COLLIDER_TAG ownTag, const ColliderBase& other, const Vector3& pushVec)
{
	ActorBase::SubOnPushed(ownTag, other, pushVec);

	if (other.GetTag() == COLLIDER_TAG::IceStage && pushVec.Abs().MaxElement().y > 0.0f) { nowIsOnIce = true; }
}

void Player::SubUpdate(void)
{
	prevIsOnIce = nowIsOnIce;
	nowIsOnIce = false;
}