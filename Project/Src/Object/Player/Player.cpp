#include "Player.h"

#include "../../Utility/Utility.h"
#include "../../Manager/TimeScale/TimeScale.h"
#include <cmath>

#include "../../Scene/SceneManager.h"
#include "../../Scene/SceneBase.h"

#include "../Common/Collider/CapsuleCollider.h"

#include "../Common/PhysicsBody/PhysicsBody.h"

#include "../Common/MotionController/GroundMotionController.h"

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
#include "State/PlayerMomentumState.h"

Player::Player() :
	CharacterBase(),

	target(nullptr),

	stealVelocity(),
	stealSource(nullptr)
{
}

void Player::Load(void)
{
#pragma region オブジェクト設定

	// 動的オブジェクトとしての処理を有効にする
	SetColliderMobility(COLLIDER_MOBILITY::Movable);

	// 物理挙動の有効化
	EnablePhysics();

	// 重力を有効にする
	GetPhysicsBody()->SetGravityEnabled(true);

	// キャラクター用の物理挙動に設定する
	GetPhysicsBody()->SetMode(PhysicsBody::MotionMode::Character);

	// 当たり判定による押し出しを有効にする
	SetContactResponseEnabled(true);
	// 物理衝突時の重さを設定
	GetPhysicsBody()->SetMass(1.0f);

	// 移動・ジャンプ・回避で共有するControllerを生成
	GroundMotionController* groundMotionController = new GroundMotionController();

	// アクターにMotionControllerを登録
	AddMotionController(groundMotionController);

	// モデル制御情報を設定
	groundMotionController->SetTransform(&trans);
	// 接地判定の参照先を設定
	groundMotionController->SetGroundedSource(&isGround);
	// 接地時の加速度を設定
	groundMotionController->SetGroundAcceleration(3.0f);
	// 空中時の加速度を設定
	groundMotionController->SetAirAcceleration(3.0f);
	// 慣性操作の強さを設定
	groundMotionController->SetMomentumSteering(0.2f);
	// 慣性操作が効く速度の閾値を設定
	groundMotionController->SetMomentumThreshold(40.0f);
	// ジャンプ力を設定
	groundMotionController->SetJumpSpeed(20.0f);

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
			*groundMotionController,
			[&]() { AnimePlay(ANIME_TYPE::Walk); },
			[&]() { AnimePlay(ANIME_TYPE::Run); }
		)
	);

	// ジャンプ状態
	AddState(
		STATE::Jump,
		new PlayerJumpState(
			*groundMotionController,
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
			30.0f, 8.0f, // 前方速度・接地中の浮き上がり速度
			100,
			0.25f, 0.55f,
			trans.rotation,
			*groundMotionController,
			isInvincble,
			[&]() { AnimePlay(ANIME_TYPE::Rolling); },
			[&]() { return GetAnimeRatio(); },
			[&]() { ChangeState(STATE::Idle); }
		)
	);

	// 勢い操作の入力管理状態
	AddState(
		STATE::MomentumInput,
		new PlayerMomentumInputState(
			0.0f,
			15.0f,
			std::bind(&Player::IsSteal,this),
			[&]() { ChangeState(STATE::MomentumQuick); },
			[&]() { ChangeState(STATE::MomentumAim); }
		)
	);

	// 勢い操作の自動選択中状態
	AddState(
		STATE::MomentumQuick,
		new PlayerMomentumQuickState(
			5.0f,
			std::bind(&Player::IsSteal, this),
			this,
			target,
			*momentumQuickStealFind,
			[&]() { ChangeState(STATE::Momentum); },
			[&]() { ChangeState(STATE::Idle); }
		)
	);

	// 勢い操作のエイム選択中状態
	AddState(
		STATE::MomentumAim,
		new PlayerMomentumAimState(
			300.0f,
			0.01f,
			1000.0f,
			trans,
			target,
			[&]() { AnimePlay(ANIME_TYPE::MomentumStealAim, false); },
			[&]() { ChangeState(STATE::Momentum); },
			[&]() { ChangeState(STATE::Idle); }
		)
	);

	// 勢い操作状態
	AddState(
		STATE::Momentum,
		new PlayerMomentumState(
			target,
			stealVelocity,
			stealSource,
			[&]() { AnimePlay(ANIME_TYPE::Totta); },
			[&]() { return GetAnimeRatio(); },
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
	// 「ジャンプ状態」->「回避状態」の自動遷移登録
	RegisterStateTransition(STATE::Jump, STATE::Rolling);

	// 「待機状態」->「勢い操作入力状態」の自動遷移登録
	RegisterStateTransition(STATE::Idle, STATE::MomentumInput);
	// 「移動状態」->「勢い操作入力状態」の自動遷移登録
	RegisterStateTransition(STATE::Move, STATE::MomentumInput);
	// 「ジャンプ状態」->「勢い操作入力状態」の自動遷移登録
	RegisterStateTransition(STATE::Jump, STATE::MomentumInput);

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