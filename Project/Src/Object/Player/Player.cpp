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

#include "State/PlayerIdleState.h"
#include "State/PlayerMoveState.h"
#include "State/PlayerJumpState.h"

Player::Player() : CharacterBase()
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
	trans.centerDiff = Vector3(0.0f, -81.8f, 0.0f) * trans.scale;

	// モデルの角度のズレの補正
	trans.SetLocalRotation(Quaternion::FromRotationY(Deg2Rad(180.0f)));

	// シェーダー登録
	CreateShader(new DefaultShader());

#pragma endregion


#pragma region アニメーション読み込み

	// アニメーションコントローラーの生成
	CreateAnimationController();

	// アニメーションの読み込み
	AddInFbxAnimation((int)ANIME_TYPE::Max, ANIME_SPEED_TABLE, ANIME_LOOP_TABLE);

#pragma endregion


#pragma region コライダーの生成

	AddCollider(
		new CapsuleCollider(
			COLLIDER_TAG::Player,
			Vector3::Yonly(56.0f) * trans.scale,
			Vector3::Yonly(-56.0f) * trans.scale,
			25.0f * trans.scale.MaxElementF()
		)
	);

#pragma endregion


#pragma region 下位アクターの生成

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
			[&]() { AnimePlay(ANIME_TYPE::Stamp); },
			std::bind(&Player::IsAnimeEnd, this),
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

#pragma endregion
}

void Player::OnCollision(COLLIDER_TAG ownTag, const ColliderBase& other, const CollisionResult& result)
{
}

void Player::SubOnGrounded(COLLIDER_TAG ownTag, const ColliderBase& other)
{
	ActorBase::SubOnGrounded(ownTag, other);

	static bool postEffectFlg = false;

	switch (other.GetTag())
	{

	case COLLIDER_TAG::IceStage: {

		DECEL_RATE = 0.05f;

		if (!postEffectFlg) {
			SceneManager::GetIns().GetScene()->AddPostEffect(new FocusLinesPostEffect(1.5f, 18.0f, 100.0f));
			SceneManager::GetIns().GetScene()->AddPostEffect(new CRTPostEffect(1.0f, 0.05f));
			postEffectFlg = true;
		}

		break;
	}

	default: {

		DECEL_RATE = 3.0f;

		if (postEffectFlg) {
			SceneManager::GetIns().GetScene()->RemovePostEffect<FocusLinesPostEffect>();
			SceneManager::GetIns().GetScene()->RemovePostEffect<CRTPostEffect>();
			postEffectFlg = false;
		}

		break;
	}

	}
}

void Player::SubUpdate(void)
{
	//static char type = 0;
	//static bool prev = false, now = false;

	//prev = now;
	//now = CheckHitKey(KEY_INPUT_SPACE) == 1;

	//if (!prev && now) {

	//	if (++type > 1) { type = 0; }

	//	switch (type){
	//	case 0: { CreateShader(new DefaultShader()); break; }
	//	case 1: { CreateShader(new RimLightShader()); break; }
	//	}
	//}
}
