#include "GameScene.h"

#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../../Manager/TimeScale/TimeScale.h"
#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Sound/SoundManager.h"
#include "../../Manager/Font/FontManager.h"

#include "../../Manager/Camera/FollowRemote/FollowRemoteCamera.h"

#include "../SceneManager.h"

#include "../ActorUseDefine.h"

#include "../Common/PostEffect/CRTPostEffect/CRTPostEffect.h"
#include "../Common/PostEffect/FocusLinesPostEffect/FocusLinesPostEffect.h"

#include "../../Object/Common/DebugObject/BoxDebugObject.h"
#include "../../Object/Common/DebugObject/SphereDebugObject.h"
#include "../../Object/Common/DebugObject/CapsuleDebugObject.h"
#include "../../Object/Common/DebugObject/MeshDebugObject.h"

#include "../../Object/Player/Player.h"

#include "../../Object/Water.h"

GameScene::GameScene() : SceneBase()
{
}

void GameScene::SubPostLoad(void)
{
	Snd::GetIns().ChangeScene("Game");

	AddActor(new BoxDebugObject(Vector3(20000, 1000, 20000), Vector3::Yonly(-500), false));

	//AddActor(new CapsuleDebugObject(Vector3(50, 0, 0), Vector3(-50, 0, 0), 50.0f, Vector3(), true, true, true, 50, true));
	AddActor(new Player);
	AddActor(new Water);

	for (int i = 0; i < 50; i++) {
		AddActor(new SphereDebugObject(50.0f, Vector3::Xonly(10.0f * 25 - (10.0f * i))));
		AddActor(new SphereDebugObject(50.0f, Vector3::Zonly(10.0f * 25 - (10.0f * i))));
	}
}

void GameScene::SubPostInit(void)
{
	AddPostEffect(new FocusLinesPostEffect(1.5f, 18.0f, 100.0f));
}

void GameScene::SubPostUpdate(void)
{
	// ƒQ[ƒ€I—¹ˆ—
	if (Input::GetIns().GetInfo(KEY_TYPE::End).down) {
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Title);
	}

	// Œˆ’è
	if (CheckHitKey(KEY_INPUT_RSHIFT) == 1) {
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::GameClear);
	}
}

void GameScene::SubUiDraw(void)
{
	DrawStringToHandle(0, 0, "ƒQ[ƒ€", 0xffffff, Font::GetIns().GetFont(FontKinds::Marumiya40));
}

void GameScene::CreateCamera(void)
{
	camera = new FollowRemoteCamera(&ActorSerch<Player>(actors)->GetTrans().pos);
}