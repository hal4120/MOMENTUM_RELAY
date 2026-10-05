#include "GameClearScene.h"

#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Sound/SoundManager.h"
#include "../../Manager/Font/FontManager.h"

#include "../SceneManager.h"

GameClearScene::GameClearScene() : SceneBase()
{
}

void GameClearScene::SubPostLoad(void)
{
	// 音声のシーン切り替え
	Snd::GetIns().ChangeScene("GameClear");
}

void GameClearScene::SubPostUpdate(void)
{
	if (
		Input::GetIns().GetInfo(KEY_TYPE::End).down ||
		Input::GetIns().GetInfo(KEY_TYPE::Enter).down
		) {
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Title);
	}
}

void GameClearScene::SubUiDraw(void)
{
	DrawStringToHandle(0, 0, "ゲームクリア", 0xffffff, Font::GetIns().GetFont(FontKinds::Marumiya40));
}