#include "GameOverScene.h"

#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Sound/SoundManager.h"
#include "../../Manager/Font/FontManager.h"

#include "../SceneManager.h"

GameOverScene::GameOverScene() : SceneBase()
{
}

void GameOverScene::SubPostLoad(void)
{
	// 音声のシーン切り替え
	Snd::GetIns().ChangeScene("GameOver");
}

void GameOverScene::SubPostUpdate(void)
{
	if (
		Input::GetIns().GetInfo(KEY_TYPE::End).down ||
		Input::GetIns().GetInfo(KEY_TYPE::Enter).down
		) {
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Title);
	}
}

void GameOverScene::SubUiDraw(void)
{
	DrawStringToHandle(0, 0, "タイトル", 0xffffff, Font::GetIns().GetFont(FontKinds::Marumiya40));
}