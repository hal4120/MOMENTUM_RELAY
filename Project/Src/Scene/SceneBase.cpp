#include "SceneBase.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../pch.h"

#include "../Application/Application.h"

#include "../Common/Vector2I.h"

#include "../Manager/Camera/CameraBase.h"
#include "../Manager/Collision/CollisionManager.h"
#include "../Object/Common/ActorBase/ActorBase.h"

#include "Common/PostEffect/PostEffectBase.h"

#include "../Object/Common/SkyBox/SkyBox.h"

SceneBase::SceneBase(void) :

	state(STATE::Created),

	collision(nullptr),

	postEffects(),

	mainScreen(-1),
	tempScreen(),

	skyBox(nullptr),

	camera(nullptr),

	actors()
{
}

void SceneBase::Load(void)
{
	// 同じシーンへLoadが2回呼ばれた場合、モデルや画像の二重ロードにつながるため検出する
	if (state != STATE::Created) { throw std::logic_error("SceneBase::Load()が不正な状態で呼ばれました"); }

	// 派生先の読み込み（前）
	SubPreLoad();

	// 画面揺れを適用するため、一度このスクリーンへ3D描画をまとめる
	mainScreen = MakeScreen(App::SCREEN_SIZE_X, App::SCREEN_SIZE_Y, true);
	if (mainScreen < 0) { throw std::runtime_error("SceneBaseのメインスクリーン生成に失敗しました"); }
	tempScreen[0] = MakeScreen(App::SCREEN_SIZE_X, App::SCREEN_SIZE_Y, true);
	tempScreen[1] = MakeScreen(App::SCREEN_SIZE_X, App::SCREEN_SIZE_Y, true);

	// シーンごとに独立した当たり判定管理クラスを生成する
	if (UseCollisionManager()) { collision = new CollisionManager(); }

	// 派生先の読み込み（後）
	// Actorの生成はCollisionManager生成後に行う必要があるため、基本的にはここで行う
	SubPostLoad();

	// シーンごとに独立したカメラを生成する
	CreateCamera();

	// 全Actorが追加された後、巨大な静的オブジェクト等のチャンクを一度構築する
	if (collision != nullptr) { collision->InitBuildChunks(); }

	// スカイボックスを生成
	if (UseSkyBox()) { skyBox = new SkyBox(); }

	state = STATE::Loaded;
}

void SceneBase::Init(void)
{
	if (state != STATE::Loaded) { throw std::logic_error("SceneBase::Init()がLoad完了前、または二重に呼ばれました"); }

	// 派生先の初期化（前）
	SubPreInit();

	// カメラ初期化
	if (camera != nullptr) { camera->Init(); }

	// スカイボックス初期化
	if (skyBox != nullptr) { skyBox->Init(); }

	// シーンが所有するActorをすべて初期化する
	for (ActorBase* actor : actors) { actor->Init(); }

	// 派生先の初期化（後）
	SubPostInit();

	state = STATE::Initialized;
}

void SceneBase::Update(void)
{
	// ロード中や解放済みのシーンは更新しない
	if (state != STATE::Initialized) { return; }

	// 入力判定やシーン固有の事前処理
	SubPreUpdate();

	// カメラ情報をDxLibへ反映
	if (camera != nullptr) { camera->Apply(); }

	// オブジェクト全ての更新処理
	for (ActorBase* actor : actors) { actor->Update(); }

	// 当たり判定更新
	if (collision != nullptr) { collision->Check(); }

	// 当たり判定による押し出し後に行う共通処理
	SubPostUpdate();

	// カメラ更新
	if (camera != nullptr) { camera->Update(); }

	// スカイボックス更新
	if (skyBox != nullptr) { skyBox->Update(); }

	// ポストエフェクト更新
	for (PostEffectBase* postEffect : postEffects) { postEffect->Update(); }
}

void SceneBase::Draw(void)
{
	if (state != STATE::Initialized || mainScreen < 0) { return; }

#pragma region 画面揺れ用スクリーンへ描画

	SetDrawScreen(mainScreen);
	ClearDrawScreen();

	// カメラ情報をDxLibへ反映
	if (camera != nullptr) { camera->Apply(); }
	Effekseer_Sync3DSetting();

#pragma endregion

#pragma region メイン描画

	// スカイボックス描画
	if (skyBox != nullptr) { skyBox->Draw(); }

	// 通常描画
	SubPreDraw();
	ActorsDraw(actors, ACTOR_DRAW_TYPE::Default);
	SubPostDraw();

	// 半透明描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	ActorsDraw(actors, ACTOR_DRAW_TYPE::Alpha);
	SubAlphaDraw();

	ActorsColliderDebugDraw(actors);

	// デバッグ用チャンク描画
	if (collision != nullptr && camera != nullptr) { collision->DrawChunkGrid(camera->GetPos()); }

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#pragma endregion

#pragma region ポストエフェクト適用

	// 最初の入力はシーン本体
	int srcScreen = mainScreen;

	// tempScreen[0] → tempScreen[1] → tempScreen[0]...
	int tempIndex = 0;

	for (PostEffectBase* postEffect : postEffects) {
		if (postEffect == nullptr) { continue; }

		const int dstScreen = tempScreen[tempIndex];

		// srcScreenの内容にポストエフェクトを掛けて
		// dstScreenへ描画
		postEffect->Apply(srcScreen, dstScreen);

		// 今生成した画面を次の入力にする
		srcScreen = dstScreen;

		// 次は反対側の一時スクリーンを使用
		tempIndex = 1 - tempIndex;
	}

#pragma endregion

	SetDrawScreen(DX_SCREEN_BACK);

	// PostEffectが0個ならmainScreen、
	// 1個以上なら最後に生成されたtempScreenが入っている
	DrawGraph(0, 0, srcScreen, true);

#pragma region UI描画

	ActorsDraw(actors, ACTOR_DRAW_TYPE::Ui);
	SubUiDraw();

	if (camera != nullptr) { camera->DrawDebug(); }

#pragma endregion
}

void SceneBase::Release(void)
{
	// デストラクタとSceneManagerの両方から呼ばれても二重解放しない
	if (state == STATE::Released) { return; }

	// 派生先の解放（前）
	SubPreRelease();

	// 全てのポストエフェクトを解放
	for (PostEffectBase*& postEffect : postEffects) {
		postEffect->Release();
		delete postEffect;
		postEffect = nullptr;
	}
	postEffects.clear();

	// 全てのオブジェクトを解放
	for (ActorBase*& actor : actors) {
		actor->Release();
		delete actor;
		actor = nullptr;
	}
	actors.clear();

	// スカイボックス解放
	if (skyBox != nullptr) {
		skyBox->Release();
		delete skyBox;
		skyBox = nullptr;
	}

	// 当たり判定管理解放
	if (collision != nullptr) {
		collision->Clear();
		delete collision;
		collision = nullptr;
	}

	// カメラ解放
	if(camera != nullptr) {
		camera->Release();
		delete camera;
		camera = nullptr;
	}

	// 画面演出用のスクリーン解放
	if (mainScreen >= 0) {
		DeleteGraph(mainScreen);
		mainScreen = -1;
	}
	for (int& screen : tempScreen) {
		if (screen == 0) { continue; }
		DeleteGraph(screen);
		screen = -1;
	}

	// 派生先の解放（後）
	SubPostRelease();

	state = STATE::Released;
}

void SceneBase::AddPostEffect(PostEffectBase* postEffect)
{
	if (postEffect == nullptr) { return; }

	postEffect->Init();

	postEffects.emplace_back(postEffect);
}

void SceneBase::AddActor(ActorBase* newActor)
{
	// 安全処理
	if (newActor == nullptr) { return; }

	// Actor共通読み込み
	newActor->Load();

	// Actorが持つコライダーをCollisionManagerへ登録
	if (collision != nullptr) { collision->Add(newActor->GetColliders()); }

	// 読み込み以外で呼ばれたら
	if(state == STATE::Initialized) {
		// Actor共通初期化
		newActor->Init();

		// 当たり判定管理クラスのチャンクを再構築する
		if (collision != nullptr) { collision->InitBuildChunks(); }
	}

	// Actorをシーンの所有リストへ追加する
	actors.emplace_back(newActor);
}

void SceneBase::ActorsDraw(const std::vector<ActorBase*>& actors, ACTOR_DRAW_TYPE drawType)
{
	for (ActorBase* actor : actors) {
		if (actor->GetDrawType() == drawType) { actor->Draw(); }

		ActorsDraw(actor->GetChildActors(), drawType);
	}
}

void SceneBase::ActorsColliderDebugDraw(const std::vector<ActorBase*>& actors)
{
	if (!App::GetIns().IsDrawDebug()) { return; }

	for (ActorBase* actor : actors) {
		actor->DrawColliderDebug();

		ActorsColliderDebugDraw(actor->GetChildActors());
	}
}