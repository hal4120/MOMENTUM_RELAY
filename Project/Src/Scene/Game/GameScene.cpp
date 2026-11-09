#include "GameScene.h"

#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../../Manager/TimeScale/TimeScale.h"
#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Sound/SoundManager.h"
#include "../../Manager/Font/FontManager.h"
#include "../../Manager/Shader/ShaderResourceManager.h"

#include "../../Manager/Camera/FollowRemote/FollowRemoteCamera.h"

#include "../SceneManager.h"

#include "../ActorUseDefine.h"

#include "../Common/PostEffect/CRTPostEffect/CRTPostEffect.h"
#include "../Common/PostEffect/FocusLinesPostEffect/FocusLinesPostEffect.h"
#include "../Common/PostEffect/MenacingPostEffect/MenacingPostEffect.h"
#include "../Common/PostEffect/PurpleFlamePostEffect/PurpleFlamePostEffect.h"
#include "../Common/PostEffect/DamageFlashPostEffect/DamageFlashPostEffect.h"

#include "../Common/PostEffect/UnderWaterPostEffect/UnderWaterPostEffect.h"

#include "../../Object/Maps/CityMap.h"
#include "../../Object/Maps/CityCollisionActor.h"

#include	"../../Object/Common/DebugObject/BoxDebugObject.h"
#include "../../Object/Common/DebugObject/IceBoxDebugObject.h"
#include "../../Object/Common/DebugObject/SphereDebugObject.h"
#include "../../Object/Common/DebugObject/CapsuleDebugObject.h"
#include "../../Object/Common/DebugObject/MeshDebugObject.h"

#include "../../Object/CameraCollOperator/CameraPointCollOperator.h"
#include "../../Object/CameraCollOperator/CameraLineCollOperator.h"

#include "../../Object/TestObject.h"
#include "../../Object/TestBoxObject.h"

#include "../../Object/TutorialStage/TutorialStage.h"

#include "../../Object/Player/Player.h"

#include "../../Object/CommonBuilding/Water.h"

GameScene::GameScene() :
	SceneBase(),

	map(nullptr)
{
}

void GameScene::SubPostLoad(void)
{
	Snd::GetIns().ChangeScene("Game");

	ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::Default);
	ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::Water);
	ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::UnderWater);
	ShaderResourceManager::GetIns().CreatePixelShader(PIXEL_SHADER_TYPE::SkyBox);

	ShaderResourceManager::GetIns().CreateVertexShader(VERTEX_SHADER_TYPE::Default1Frame);
	ShaderResourceManager::GetIns().CreateVertexShader(VERTEX_SHADER_TYPE::RimLight1Frame);

	map = new CityMap();
	map->Load("Data/Maps/City/CityMap.json", "Data/Maps/City");
	AddActor(new CityCollisionActor(*map));

	//AddActor(new BoxDebugObject(Vector3(20000, 1000, 20000), Vector3::Yonly(-500), false, false));

	//AddActor(new TutorialStage);

	AddActor(new IceBoxDebugObject(Vector3(1000, 100, 1000), Vector3::XZonly(-1500.0f, 1500.0f), false, false));
	//AddActor(new IceBoxDebugObject(Vector3(10000, 10000, 10), Vector3::YZonly(5000.0f, 5000.0f), false, false));
	//AddActor(new IceBoxDebugObject(Vector3(10000, 10000, 100), Vector3::YZonly(5000.0f, -5000.0f), false, false));
	//AddActor(new IceBoxDebugObject(Vector3(100, 10000, 10000), Vector3::XYonly(5000.0f, 5000.0f), false, false));
	//AddActor(new IceBoxDebugObject(Vector3(100, 10000, 10000), Vector3::XYonly(-5000.0f, 5000.0f), false, false));

#pragma region テストオブジェクト

	for (int i = 0; i < 10; i++) {
		AddActor(new TestObject(Vector3(-500, 300, -500), Vector3::Xonly(50.0f)));
		AddActor(new TestObject(Vector3(-500, 300, -500), Vector3::Zonly(50.0f)));
	}

	for (int i = 0; i < 10; i++) {
		AddActor(
			new TestBoxObject(
				//Vector3(100),
				Vector3(100, 200, 100),
				Vector3(0.0f, 300.0f, 0.0f),
				TestBoxObject::TestMode::FreeFall
			)
		);
	}

#pragma endregion

	AddActor(new Player);

	//AddActor(new Water(Vector3(), Vector3(50000, 100, 50000)));
}

void GameScene::SubPostInit(void)
{
}

void GameScene::SubPostUpdate(void)
{
	// ゲーム終了処理
	if (Input::GetIns().GetInfo(KEY_TYPE::End).down) {
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Title);
	}

	static bool prevDebugKey = false, nowDebugKey = false, isPostEffect = false;
	prevDebugKey = nowDebugKey;
	nowDebugKey = CheckHitKey(KEY_INPUT_RSHIFT) == 1;
	// 決定
	if (!prevDebugKey && nowDebugKey) {
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::GameClear);
	}

	switch (ActorSerch<Player>(actors)->OnIcePostEffectSwitch()) {
	case -1: { break; }
	case 0: {
		RemovePostEffect<FocusLinesPostEffect>();
		RemovePostEffect<CRTPostEffect>();
		RemovePostEffect<MenacingPostEffect>();
		//RemovePostEffect<PurpleFlamePostEffect>();
		break;
	}
	case 1: {
		AddPostEffect(new FocusLinesPostEffect(1.5f, 18.0f, 100.0f));
		AddPostEffect(new CRTPostEffect(1.0f, 0.05f));
		AddPostEffect(new MenacingPostEffect(2.0f, 0.5f, 1.0f));
		//AddPostEffect(new PurpleFlamePostEffect(10.0f, 0.4f, 1.75f));
		//AddPostEffect(new DamageFlashPostEffect(10.0f, 10.0f));
		break;
	}
	}

	//switch (ActorSerch<Water>(actors)->UnderWaterPostEffectSwitch()) {
	//case -1: { break; }
	//case 0: { RemovePostEffect<UnderWaterPostEffect>(); break; }
	//case 1: { AddPostEffect(new UnderWaterPostEffect()); break; }
	//}
}

void GameScene::SubPreDraw(void)
{
	map->Draw();
}

void GameScene::SubUiDraw(void)
{
	//DrawStringToHandle(0, 0, "ゲーム", 0xffffff, Font::GetIns().GetFont(FontKinds::Marumiya40));
}

void GameScene::SubPreRelease(void)
{
	if (map != nullptr) {
		map->Release();
		delete map;
		map = nullptr;
	}
}

void GameScene::CreateCamera(void)
{
	camera = new FollowRemoteCamera(&ActorSerch<Player>(actors)->GetTrans().pos);

	// カメラの当たり判定オペレーターを追加
	AddActor(new CameraPointCollOperator(camera->GetPos()));
	AddActor(new CameraLineCollOperator(camera->GetPos(), ActorSerch<Player>(actors)->GetTrans().pos));
}