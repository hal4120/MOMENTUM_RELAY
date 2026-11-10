#pragma once

#include"../SceneBase.h"

class CityMap;

class GameScene : public SceneBase
{
public:
	GameScene();
	~GameScene()override = default;

private:

	// スカイボックスを使用する
	bool UseSkyBox(void)const override { return true; }

#pragma region 主要関数再定義

	// 読み込み
	void SubPostLoad(void)override;

	// 初期化
	void SubPostInit(void)override;

	// 更新
	void SubPostUpdate(void)override;

	// 描画
	void SubPreDraw(void) override;

	// UI描画
	void SubUiDraw(void)override;

	// 解放
	void SubPreRelease(void) override;

#pragma endregion

	void CreateCamera(void)override;

	void DrawControlGuide(void);

	void DrawMomentumGuide(void);

	CityMap* map;
};