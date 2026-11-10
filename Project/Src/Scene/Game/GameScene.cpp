#include "GameScene.h"

#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../../Manager/TimeScale/TimeScale.h"
#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Sound/SoundManager.h"
#include "../../Manager/Font/FontManager.h"
#include "../../Manager/Shader/ShaderResourceManager.h"

#include "../../Manager/Camera/FollowRemote/FollowRemoteCamera.h"

#include "../../Manager/Camera/CameraEvent/GameStartCameraEvent/GameStartCameraEvent.h"

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

#include "../../Object/TestObject/TestObject.h"
#include "../../Object/TestObject/TestBoxObject.h"

#include "../../Object/GoalArea/GoalArea.h"

#include "../../Object/Player/Player.h"

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

	AddActor(new GoalArea);

#pragma region テストオブジェクト

	for (int i = 0; i < 100; i++) {
		AddActor(new TestObject(Vector3(-500, 300, -500), Vector3::Yonly(50.0f)));
	}

#pragma endregion

	AddActor(new Player);
}

void GameScene::SubPostInit(void)
{
    // SceneManagerにGameSceneが登録された後にイベントを開始する。
    // Load中のCreateCamera()から開始するとGetScene()が旧シーンを返す。
    const Vector3& goalPos = ActorSerch<GoalArea>(actors)->GetTrans().pos;

    camera->StartEvent(
        new GameStartCameraEvent(
            GameStartCameraEvent::LookAt,
            goalPos + Vector3(600, 600, 600),
            goalPos,
            camera->GetFov(),
            100.0f,
            150.0f
        )
    );
}

void GameScene::SubPostUpdate(void)
{
	// ゲーム終了処理
	if (Input::GetIns().GetInfo(KEY_TYPE::End).down) {
		SceneManager::GetIns().ChangeSceneFade(SCENE_ID::Title);
	}

	//switch (ActorSerch<Player>(actors)->OnIcePostEffectSwitch()) {
	//case -1: { break; }
	//case 0: {
	//	RemovePostEffect<FocusLinesPostEffect>();
	//	RemovePostEffect<CRTPostEffect>();
	//	RemovePostEffect<MenacingPostEffect>();
	//	break;
	//}
	//case 1: {
	//	AddPostEffect(new FocusLinesPostEffect(1.5f, 18.0f, 100.0f));
	//	AddPostEffect(new CRTPostEffect(1.0f, 0.05f));
	//	AddPostEffect(new MenacingPostEffect(2.0f, 0.5f, 1.0f));
	//	break;
	//}
	//}

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
	DrawControlGuide();
	DrawMomentumGuide();
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

void GameScene::DrawControlGuide(void)
{
	// 操作説明の開閉状態
	static bool isOpen = false;

	// 前フレームのキー入力状態
	static bool prevKeyDown = false;

	// キー「1」の現在の入力状態
	const bool keyDown = CheckHitKey(KEY_INPUT_1) != 0;

	// ダウントリガーで開閉を切り替える
	if (keyDown && !prevKeyDown) {
		isOpen = !isOpen;
	}

	// 次フレーム用に入力状態を保存
	prevKeyDown = keyDown;

	constexpr int BASE_X = 20;
	constexpr int BASE_Y = 20;

	constexpr int LABEL_X = BASE_X;
	constexpr int INPUT_X = BASE_X + 125;

	constexpr int ITEM_HEIGHT = 66;
	constexpr int LINE_HEIGHT = 23;

	const int font = Font::GetIns().GetFont(FontKinds::Default20);

	const unsigned int white = GetColor(255, 255, 255);
	const unsigned int yellow = GetColor(255, 225, 120);
	const unsigned int shadow = GetColor(35, 35, 35);

	// 文字に影をつけて描画
	auto drawText = [&](int x, int y, const char* str, unsigned int color)
		{
			DrawStringToHandle(x + 2, y + 2, str, shadow, font);
			DrawStringToHandle(x, y, str, color, font);
		};

	// 閉じている場合
	if (!isOpen) {
		drawText(BASE_X, BASE_Y, "キーボード「1」で操作説明を開く", yellow);
	}
	// 開いている場合
	else {

		drawText(BASE_X, BASE_Y, "キーボード「1」で操作説明を閉じる", yellow);

		// 操作項目の描画
		auto drawItem = [&](int index,
			const char* label,
			const char* keyboard,
			const char* controller)
			{
				const int y = BASE_Y + 40 + index * ITEM_HEIGHT;

				drawText(LABEL_X, y + 10, label, yellow);
				drawText(INPUT_X, y,
					keyboard, white);
				drawText(INPUT_X, y + LINE_HEIGHT,
					controller, white);
			};

		drawItem(0,
			"視点操作",
			"キーボード　　：マウス",
			"コントローラー：右スティック"
		);

		drawItem(1,
			"移動",
			"キーボード　　：WASDキー",
			"コントローラー：左スティック"
		);

		drawItem(2,
			"ダッシュ",
			"キーボード　　：左Shift",
			"コントローラー：X / Yボタン"
		);

		drawItem(3,
			"ジャンプ",
			"キーボード　　：スペースキー",
			"コントローラー：A / Bボタン"
		);

		drawItem(4,
			"回避",
			"キーボード　　：左Ctrl",
			"コントローラー：左トリガー"
		);
	}
}

void GameScene::DrawMomentumGuide(void)
{
	// 勢い操作説明の開閉状態
	static bool isOpen = false;

	// 前フレームの入力状態
	static bool prevKeyDown = false;

	// キー「2」のダウントリガー
	const bool keyDown = CheckHitKey(KEY_INPUT_2) != 0;

	if (keyDown && !prevKeyDown) {
		isOpen = !isOpen;
	}

	prevKeyDown = keyDown;

	// 右側の描画位置
	constexpr int BASE_X = 850;
	constexpr int BASE_Y = 20;

	constexpr int ITEM_HEIGHT = 32;
	constexpr int GROUP_HEIGHT = 145;

	const int font = Font::GetIns().GetFont(FontKinds::Default20);

	const unsigned int white = GetColor(255, 255, 255);
	const unsigned int yellow = GetColor(255, 225, 120);
	const unsigned int cyan = GetColor(120, 230, 255);
	const unsigned int shadow = GetColor(35, 35, 35);

	// 影付き文字描画
	auto drawText = [&](int x, int y, const char* str, unsigned int color)
		{
			DrawStringToHandle(x + 2, y + 2, str, shadow, font);
			DrawStringToHandle(x, y, str, color, font);
		};

	// 開閉案内
	if (!isOpen) {
		drawText(BASE_X, BASE_Y,
			"キーボード「2」で能力説明を開く", yellow);
		return;
	}

	drawText(BASE_X, BASE_Y,
		"キーボード「2」で能力説明を閉じる", yellow);

	// 操作キーの説明
	drawText(
		BASE_X,
		BASE_Y + 40,
		"キーボード　　：左クリック",
		cyan
	);
	drawText(
		BASE_X,
		BASE_Y + 60,
		"コントローラー：右トリガー",
		cyan
	);

	// 各状態の説明を描画
	auto drawGroup = [&](
		int y,
		const char* title,
		const char* tapText,
		const char* holdText1,
		const char* holdText2
		)
		{
			drawText(BASE_X, y, title, yellow);

			drawText(BASE_X + 10, y + ITEM_HEIGHT,
				"【単押し】", cyan);

			drawText(BASE_X + 125, y + ITEM_HEIGHT,
				tapText, white);

			drawText(BASE_X + 10, y + ITEM_HEIGHT * 2,
				"【長押し】", cyan);

			drawText(BASE_X + 125, y + ITEM_HEIGHT * 2,
				holdText1, white);

			drawText(BASE_X + 125, y + ITEM_HEIGHT * 3,
				holdText2, white);
		};

	// 勢いを保持していない場合
	drawGroup(
		BASE_Y + 120,
		"■ 勢いを保持していない場合",
		"最も近い対象から勢いを奪う",
		"時間が遅くなり、視点で対象を選択",
		"ボタンを離すと勢いを奪う"
	);

	// 勢いを保持している場合
	drawGroup(
		BASE_Y + 120 + GROUP_HEIGHT,
		"■ 勢いを保持している場合",
		"保持中の勢いを自分に付与",
		"時間が遅くなり、視点で対象を選択",
		"ボタンを離すと勢いを付与"
	);
}