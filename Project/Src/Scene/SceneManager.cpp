#include "SceneManager.h"

#include <algorithm>
#include <stdexcept>

#include "../pch.h"

#include "../Manager/Loading/Loading.h"

#include "../Manager/Camera/CurrentCamera.h"

#include "Title/TitleScene.h"
#include "Game/GameScene.h"
#include "GameClear/GameClearScene.h"
#include "GameOver/GameOverScene.h"

#include "Common/Fade/Default/DefaultFadeScene.h"

SceneManager* SceneManager::ins = nullptr;

SceneManager::SceneManager(void) :

	scenes(),

	requests(),

	loadingScene(nullptr),
	loadCommit(LOAD_COMMIT::None),

	fade(nullptr),
	fadeScene(nullptr),

	isWaitFadeOut(false),

	fadeCommit(LOAD_COMMIT::None),

	fillLight(-1),
	rimLight(-1)
{
}

SceneManager::~SceneManager(void) = default;

void SceneManager::Init(void)
{
	// ローディング画面管理クラスの生成
	Loading::CreateInstance();
	Loading::GetInstance()->Init();
	Loading::GetInstance()->Load();

	// DxLibの3D描画設定
	Init3D();

	// 最初のタイトルシーンは予約を経由せず、この初期化処理内で読み込みを開始する
	StartLoad(CreateScene(SCENE_ID::Title), LOAD_COMMIT::Jump);
}

void SceneManager::Release(void)
{
	// 予約中のシーンを破棄
	requests.clear();
	fadeScene.reset();

	// ロード途中のシーンも正しく解放する
	if (loadingScene != nullptr) {
		loadingScene->Release();
		loadingScene.reset();
	}

	// スタック内の全シーン解放
	for (std::unique_ptr<SceneBase>& scene : scenes) {
		if (scene != nullptr) { scene->Release(); }
	}
	scenes.clear();

	if (fade != nullptr) {
		fade->Release();
		fade.reset();
	}

	// SceneManagerで生成したライトをまとめて削除
	DeleteLightHandleAll();
	fillLight = -1;
	rimLight = -1;

	// Loading解放
	if (Loading::GetInstance() != nullptr)	{
		Loading::GetInstance()->Release();
		Loading::DeleteInstance();
	}
}

void SceneManager::Update(void)
{
	// フェードシーンは通常シーンとは別枠で最前面に更新する
	if (fade != nullptr) {

		fade->Update();

		if (isWaitFadeOut && fade->IsFadeOutEnd()) {
			isWaitFadeOut = false;
			StartLoad(std::move(fadeScene), fadeCommit);
		}

		if (fade->IsEnd()) {
			fade->Release();
			fade.reset();
		}
	}

	if (loadingScene != nullptr) { UpdateLoading(); return; }

	if (!scenes.empty()) {
		// 下のシーンの更新を止めるシーンがあれば、そこから上だけを更新する
		const std::size_t firstIndex = GetFirstUpdateIndex();
		for (std::size_t i = firstIndex; i < scenes.size(); i++) { scenes[i]->Update(); }
	}

	// シーン遷移要求があれば1件ずつ適用する
	ApplyRequest();
}

void SceneManager::Draw(void)
{
	// シーンが存在する場合、透明シーンの範囲だけ下から描画する
	if (!scenes.empty())	{
		// 透明シーンの下も描画対象に含めるため、描画開始インデックスを取得する
		const std::size_t firstIndex = GetFirstDrawIndex();
		for (std::size_t i = firstIndex; i < scenes.size(); i++) { scenes[i]->Draw(); }
	}

	// フェードは必ず最前面へ描画する
	if (fade != nullptr) { fade->Draw(); }

	// ロード中はシーンの上へLoading画面を描画する
	if (loadingScene != nullptr) { Loading::GetInstance()->Draw(); }
}

#pragma region 公開シーン遷移関数

void SceneManager::ChangeScene(std::unique_ptr<SceneBase> scene)
{
	// nullptrの遷移先は登録しない（Popだけはsceneを持たないため例外）
	SceneRequest request;

	// シーン切り替え要求を登録する。既に遷移要求がある、ロード中、フェード中の場合は新しい要求を受け付けない
	request.type = REQUEST_TYPE::Change;

	// 遷移先のシーンをムーブして登録する。std::moveを使うことで所有権を移動させ、不要なコピーを避ける
	request.scene = std::move(scene);

	// シーン遷移要求をキューに追加する。std::moveを使うことで所有権を移動させ、不要なコピーを避ける
	AddRequest(std::move(request));
}

void SceneManager::ChangeScene(SCENE_ID scene)
{
	ChangeScene(CreateScene(scene));
}

void SceneManager::ChangeSceneFade(std::unique_ptr<SceneBase> scene, FADE_TYPE fadeType, unsigned short FADE_TIME, unsigned int FADE_OUT_COLOR, unsigned int FADE_IN_COLOR)
{
	// 一時変数にシーン遷移要求を作成し、必要な情報を設定する～～～～～～～～～～～

	SceneRequest request;

	// シーンの切り替え要求種類
	request.type = REQUEST_TYPE::ChangeFade;

	// 遷移先のシーン
	request.scene = std::move(scene);

	// フェードの種類
	request.fadeType = fadeType;

	// フェード1フレーム未満は無効とし、最低1フレームのフェード時間を保証する
	request.fadeTime = (std::max)(static_cast<unsigned short>(1), FADE_TIME);

	// フェードアウトの色
	request.fadeOutColor = FADE_OUT_COLOR;
	// フェードインの色
	request.fadeInColor = FADE_IN_COLOR;

	// ～～～～～～～～～～～一時変数にシーン遷移要求を作成し、必要な情報を設定する

	// 作成したシーン遷移要求をキューに追加する
	AddRequest(std::move(request));
}

void SceneManager::ChangeSceneFade(SCENE_ID scene, FADE_TYPE fadeType, unsigned short FADE_TIME, unsigned int FADE_OUT_COLOR, unsigned int FADE_IN_COLOR)
{
	ChangeSceneFade(CreateScene(scene), fadeType, FADE_TIME, FADE_OUT_COLOR, FADE_IN_COLOR);
}

void SceneManager::PushScene(std::unique_ptr<SceneBase> scene)
{
	// 一時変数にシーン遷移要求を作成し、必要な情報を設定する～～～～～～～～～～～

	SceneRequest request;

	// シーンの切り替え要求種類
	request.type = REQUEST_TYPE::Push;

	// 遷移先のシーン
	request.scene = std::move(scene);

	// ～～～～～～～～～～～一時変数にシーン遷移要求を作成し、必要な情報を設定する

	// 作成したシーン遷移要求をキューに追加する
	AddRequest(std::move(request));
}

void SceneManager::PopScene(std::size_t popNum)
{
	// 一時変数にシーン遷移要求を作成し、必要な情報を設定する～～～～～～～～～～～

	SceneRequest request;

	// シーンの切り替え要求種類
	request.type = REQUEST_TYPE::Pop;

	// ポップするシーンの数を設定する。スタック数を超える値が指定されても範囲内に丸める
	request.popNum = popNum;

	// ～～～～～～～～～～～一時変数にシーン遷移要求を作成し、必要な情報を設定する

	// 作成したシーン遷移要求をキューに追加する
	AddRequest(std::move(request));
}

void SceneManager::JumpScene(std::unique_ptr<SceneBase> scene)
{
	// 一時変数にシーン遷移要求を作成し、必要な情報を設定する～～～～～～～～～～～

	SceneRequest request;

	// シーンの切り替え要求種類
	request.type = REQUEST_TYPE::Jump;

	// 遷移先のシーン
	request.scene = std::move(scene);

	// ～～～～～～～～～～～一時変数にシーン遷移要求を作成し、必要な情報を設定する

	// 作成したシーン遷移要求をキューに追加する
	AddRequest(std::move(request));
}

void SceneManager::JumpScene(SCENE_ID scene)
{
	JumpScene(CreateScene(scene));
}

void SceneManager::JumpSceneFade(std::unique_ptr<SceneBase> scene, FADE_TYPE fadeType, unsigned short FADE_TIME, unsigned int FADE_OUT_COLOR, unsigned int FADE_IN_COLOR)
{
	// 一時変数にシーン遷移要求を作成し、必要な情報を設定する～～～～～～～～～～～

	SceneRequest request;

	// シーンの切り替え要求種類
	request.type = REQUEST_TYPE::JumpFade;

	// 遷移先のシーン
	request.scene = std::move(scene);

	// フェードの種類
	request.fadeType = fadeType;

	// フェード1フレーム未満は無効とし、最低1フレームのフェード時間を保証する
	request.fadeTime = (std::max)(static_cast<unsigned short>(1), FADE_TIME);

	// フェードアウトの色
	request.fadeOutColor = FADE_OUT_COLOR;
	// フェードインの色
	request.fadeInColor = FADE_IN_COLOR;

	// ～～～～～～～～～～～一時変数にシーン遷移要求を作成し、必要な情報を設定する

	// 作成したシーン遷移要求をキューに追加する
	AddRequest(std::move(request));
}

void SceneManager::JumpSceneFade(SCENE_ID scene, FADE_TYPE fadeType, unsigned short FADE_TIME, unsigned int FADE_OUT_COLOR, unsigned int FADE_IN_COLOR)
{
	JumpSceneFade(CreateScene(scene), fadeType, FADE_TIME, FADE_IN_COLOR, FADE_OUT_COLOR);
}

#pragma endregion

void SceneManager::AddRequest(SceneRequest request)
{
	// nullptrの遷移先は登録しない（Popだけはsceneを持たないため例外）
	if (request.type != REQUEST_TYPE::Pop && request.scene == nullptr) { return; }

	// 既に遷移要求がある、ロード中、フェード中の場合は新しい要求を受け付けない
	// 同一フレームにClearとGameOverが同時発生しても、最初の要求だけを採用する
	if (IsTransition()) { return; }

	requests.emplace_back(std::move(request));
}

void SceneManager::ApplyRequest(void)
{
	if (requests.empty()) { return; }
	if (loadingScene != nullptr) { return; }

	// 1フレームで複数の構造変更を行わず、必ず1件ずつ適用する
	SceneRequest request = std::move(requests.front());

	requests.pop_front();

	ApplyRequest(std::move(request));
}

void SceneManager::ApplyRequest(SceneRequest request)
{
	switch (request.type) {

	case REQUEST_TYPE::Change: { StartLoad(std::move(request.scene), LOAD_COMMIT::Change); break; }

	case REQUEST_TYPE::Push: { StartLoad(std::move(request.scene), LOAD_COMMIT::Push); break; }

	case REQUEST_TYPE::Pop: {

		// スタック数を超える値が指定されても範囲内に丸める
		const std::size_t popNum = (std::min)(request.popNum, scenes.size());

		for (std::size_t i = 0; i < popNum; i++) {
			scenes.back()->Release();
			scenes.pop_back();
		}

		break;
	}

	case REQUEST_TYPE::Jump: { StartLoad(std::move(request.scene), LOAD_COMMIT::Jump); break; }

	case REQUEST_TYPE::ChangeFade: {

		// まずフェードアウトだけ開始し、完全に暗くなってからロードする
		fadeScene = std::move(request.scene);

		// フェードアウトが終わったらロードするため、ロードコミットはフェードアウト後に設定する
		fadeCommit = LOAD_COMMIT::ChangeFade;

		// フェードアウトが終わるまでロードを待つフラグを立てる
		isWaitFadeOut = true;

		// フェードシーンを生成し、ロードと初期化を行う。フェードシーンは通常のシーンとは別枠で最前面に更新する
		fade = CreateFadeScene(request.fadeType, request.fadeTime, request.fadeOutColor, request.fadeInColor);

		// フェードシーンが生成できた場合のみロードと初期化を行う。nullptrの場合はフェードなしで遷移する
		if (fade != nullptr) { fade->Load(); fade->Init(); }

		break;
	}

	case REQUEST_TYPE::JumpFade: {

		// まずフェードアウトだけ開始し、完全に暗くなってからロードする
		fadeScene = std::move(request.scene);

		// フェードアウトが終わったらロードするため、ロードコミットはフェードアウト後に設定する
		fadeCommit = LOAD_COMMIT::JumpFade;

		// フェードアウトが終わるまでロードを待つフラグを立てる
		isWaitFadeOut = true;

		// フェードシーンを生成し、ロードと初期化を行う。フェードシーンは通常のシーンとは別枠で最前面に更新する
		fade = CreateFadeScene(request.fadeType, request.fadeTime, request.fadeOutColor, request.fadeInColor);

		// フェードシーンが生成できた場合のみロードと初期化を行う。nullptrの場合はフェードなしで遷移する
		if (fade != nullptr) { fade->Load(); fade->Init(); }

		break;
	}

	default: { break; }
	}

	// 1番上に重なっているシーンのカメラをCurrentCameraに設定する
	SetCurrentCamera();
}

void SceneManager::StartLoad(std::unique_ptr<SceneBase> scene, LOAD_COMMIT commit)
{
	// 安全処理
	if (scene == nullptr) { return; }

	// 新規シーンをロード中としてローカル変数で保持する
	loadingScene = std::move(scene);

	// ロード完了後の反映方法をローカル変数で保持する
	loadCommit = commit;

	// ポーズ等の軽量シーンはローディング画面を挟まず、その場でLoad / Initする
	if (!loadingScene->IsUseLoadingScreen()) {

		loadingScene->Load();
		loadingScene->Init();

		CommitLoadedScene();

		// 1番上に重なっているシーンのカメラをCurrentCameraに設定する
		SetCurrentCamera();

		return;
	}

	// DxLibの非同期ロードフラグを有効にしてからシーンのLoadを呼ぶ
	Loading::GetInstance()->StartAsyncLoad();
	loadingScene->Load();
	Loading::GetInstance()->EndAsyncLoad();
}

void SceneManager::UpdateLoading(void)
{
	// ローディング画面の更新を行う
	Loading::GetInstance()->Update();

	// DxLibの非同期ロードと最低表示時間が両方終わるまで待つ
	if (Loading::GetInstance()->IsLoading()) { return; }

	// ～～～↓ロード完了↓～～～

	// ロード完了したシーンをスタックに登録する
	CommitLoadedScene();

	// ロード対象を初期化する
	scenes.back()->Init();

	// 1番上に重なっているシーンのカメラをCurrentCameraに設定する
	SetCurrentCamera();
}

void SceneManager::CommitLoadedScene(void)
{
	// ロード完了したシーンをスタックに登録する
	CommitScene(std::move(loadingScene), loadCommit);

	// フェード遷移の場合、シーンの入れ替えと初期化がすべて終わってから明るくする
	if (loadCommit == LOAD_COMMIT::ChangeFade || loadCommit == LOAD_COMMIT::JumpFade) {
		if (fade != nullptr) { fade->StartFadeIn(); }
	}

	// 読み込み完了後の反映方法を未設定に戻す
	loadCommit = LOAD_COMMIT::None;
}

void SceneManager::CommitScene(std::unique_ptr<SceneBase> scene, LOAD_COMMIT commit)
{
	if (scene == nullptr) { return; }

	switch (commit) {

	case LOAD_COMMIT::Change:
	case LOAD_COMMIT::ChangeFade: {

		// 末尾だけを入れ替える
		if (scenes.empty()) { scenes.emplace_back(std::move(scene)); }
		else {
			scenes.back()->Release();
			scenes.back() = std::move(scene);
		}

		break;
	}

	case LOAD_COMMIT::Push: {

		// 既存シーンを残して末尾へ追加する
		scenes.emplace_back(std::move(scene));

		break;
	}

	case LOAD_COMMIT::Jump:
	case LOAD_COMMIT::JumpFade: {

		// すべて解放して遷移先だけにする
		for (std::unique_ptr<SceneBase>& oldScene : scenes) {
			if (oldScene != nullptr) { oldScene->Release(); }
		}
		scenes.clear();

		scenes.emplace_back(std::move(scene));

		break;
	}

	case LOAD_COMMIT::None:
	default: { break; }

	}
}

std::size_t SceneManager::GetFirstUpdateIndex(void)const
{
	// 安全処理
	if (scenes.empty()) { return 0; }

	// 末尾から探索
	std::size_t index = scenes.size() - 1;

	// 末尾から先頭へ確認し、下の更新を許可するシーンの間だけ遡る
	while (index > 0 && scenes[index]->IsLowerSceneUpdate()) { index--; }

	// 結果を返す
	return index;
}

std::size_t SceneManager::GetFirstDrawIndex(void)const
{
	// 安全処理
	if (scenes.empty()) { return 0; }

	// 末尾から探索
	std::size_t index = scenes.size() - 1;

	// 末尾から先頭へ確認し、下の描画を許可するシーンの間だけ遡る
	while (index > 0 && scenes[index]->IsLowerSceneDraw()) { index--; }

	// 結果を返す
	return index;
}

std::unique_ptr<SceneBase> SceneManager::CreateScene(SCENE_ID scene)const
{
	switch (scene) {

	case SCENE_ID::Title: { return std::make_unique<TitleScene>(); }

	case SCENE_ID::Game: { return std::make_unique<GameScene>(); }

	case SCENE_ID::GameClear: { return std::make_unique<GameClearScene>(); }

	case SCENE_ID::GameOver: { return std::make_unique<GameOverScene>(); }

	default: { return nullptr; }
	}
}

std::unique_ptr<FadeSceneBase> SceneManager::CreateFadeScene(FADE_TYPE fadeType, unsigned short FADE_TIME, unsigned int FADE_OUT_COLOR,	unsigned int FADE_IN_COLOR)const
{
	switch (fadeType)	{

	case FADE_TYPE::Default: { return std::make_unique<DefaultFadeScene>(FADE_TIME, FADE_OUT_COLOR, FADE_IN_COLOR); }

	default: { return std::make_unique<DefaultFadeScene>(FADE_TIME, FADE_OUT_COLOR, FADE_IN_COLOR); }
	}
}

void SceneManager::SetCurrentCamera(void) const
{
	// 初期化
	CurrentCamera::Set(nullptr);

	// 安全処理
	if (scenes.empty()) { return; }

	// 末尾から先頭へ確認し、カメラを使用しているシーンを探す
	for (int index = (int)(scenes.size() - 1); index >= 0; index--) {

		// カメラのインスタンスのポインターを取得
		CameraBase* cameraIns = scenes.at(index)->GetCamera();

		// 取得したインスタンスが有効かどうか
		if (cameraIns == nullptr) { continue; }

		// 有効ならそれをカレントカメラに設定
		CurrentCamera::Set(cameraIns);

		// 終了
		break;
	}
}

void SceneManager::Init3D(void)
{
#pragma region 基本描画設定

	SetBackgroundColor(0, 0, 0);
	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetUseBackCulling(true);
	SetTextureAddressMode(DX_TEXADDRESS_WRAP);
	SetDrawBright(255, 255, 255);

#pragma endregion

#pragma region フォグ設定

	SetFogEnable(true);
	SetFogColor(200, 200, 200);
	SetFogStartEnd(5000.0f, 10000.0f);

#pragma endregion

#pragma region ライティング全体設定

	SetUseLighting(true);
	SetUseSpecular(false);
	SetGlobalAmbientLight(GetColorF(0.38f, 0.38f, 0.38f, 1.0f));

#pragma endregion

#pragma region メインライト設定

	VECTOR mainDirection = VNorm(VGet(0.0f, 0.35f, 1.0f));
	ChangeLightTypeDir(mainDirection);
	SetLightDirection(mainDirection);
	SetLightEnable(true);
	SetLightDifColor(GetColorF(0.62f, 0.62f, 0.62f, 1.0f));
	SetLightAmbColor(GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
	SetLightSpcColor(GetColorF(0.0f, 0.0f, 0.0f, 1.0f));

#pragma endregion

#pragma region 補助ライト設定

	fillLight = CreateDirLightHandle(VNorm(VGet(0.05f, 0.15f, 1.0f)));

	if (fillLight >= 0) {
		SetLightDifColorHandle(fillLight, GetColorF(0.20f, 0.20f, 0.20f, 1.0f));
		SetLightAmbColorHandle(fillLight, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
		SetLightSpcColorHandle(fillLight, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
		SetLightEnableHandle(fillLight, true);
	}

#pragma endregion

#pragma region リムライト設定

	rimLight = CreateDirLightHandle(VNorm(VGet(0.0f, 0.10f, -1.0f)));

	if (rimLight >= 0) {
		SetLightDifColorHandle(rimLight, GetColorF(0.13f, 0.13f, 0.13f, 1.0f));
		SetLightAmbColorHandle(rimLight, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
		SetLightSpcColorHandle(rimLight, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
		SetLightEnableHandle(rimLight, true);
	}

#pragma endregion

#pragma region マテリアル設定

	MATERIALPARAM material{};
	material.Diffuse = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = GetColorF(0.6f, 0.6f, 0.6f, 1.0f);
	material.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	material.Power = 0.0f;
	SetMaterialParam(material);

#pragma endregion
}

