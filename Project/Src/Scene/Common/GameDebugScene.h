#pragma once

#include "../../pch.h"

#include<functional>

#include"../SceneBase.h"

#include"../SceneManager.h"

#include "../../Manager/Camera/MultifuncCamera/MultifuncCamera.h"

#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Sound/SoundManager.h"

class GameDebugScene : public SceneBase
{
public:
	GameDebugScene(MultifuncCamera& camera, std::function<void(void)>CameraReset, std::function<void(void)>TopUpdate) : SceneBase(),
		camera(camera),
		TopUpdate(std::move(TopUpdate)),
		CameraReset(std::move(CameraReset))
	{
	}
	~GameDebugScene() = default;

private:

#pragma region 主要関数再定義
	// 初期化
	void SubPostInit(void)override { 
		camera.ChangeModeFree(Deg2Rad(2.0f), 10.0f, camera.GetPos(), camera.GetAngle()); 

		Snd::GetIns().AllStop();
	}

	// 更新
	void SubPostUpdate(void)override {

		if (Key::GetIns().GetInfo(KEY_TYPE::DEBUG_MODE_TOPUPDATE).now) { TopUpdate(); Snd::GetIns().PausePlay(); }
		else { Snd::GetIns().AllStop(); }

		camera.Update();

		if (
			Key::GetIns().GetInfo(KEY_TYPE::DEBUG_MODE_SWITCH).down ||
			Key::GetIns().GetInfo(KEY_TYPE::PAUSE).down
			) {
			SceneManager::GetIns().PopScene();
		}
	}

	// 終了
	void SubPreRelease(void) { CameraReset(); Snd::GetIns().PausePlay(); }
#pragma endregion

	bool IsUseLoadingScreen(void)const { return false; }

	// 当たり判定管理は使用しない
	bool UseCollisionManager(void)const override { return false; }

	MultifuncCamera& camera;

	std::function<void(void)>TopUpdate;
	std::function<void(void)>CameraReset;
};