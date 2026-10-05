#pragma once

#include<string>
#include<vector>
#include"../../Common/Vector2.h"
#include"../../Common/Vector2I.h"

class InputManager
{
private:
	
	// コンストラクタ/デストラクタ
	InputManager();
	~InputManager() = default;

	// コピー・ムーブ操作を禁止
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	static InputManager* ins;

public:

#pragma region シングルトン定義
	// 生成/初期化処理
	static void CreateIns(void) { if (ins == nullptr) { ins = new InputManager(); ins->Init(); } }
	// 取得
	static InputManager& GetIns(void) { return *ins; }
	// 終了処理/削除
	static void DeleteIns(void) { if (ins != nullptr) { ins->Release(); delete ins; ins = nullptr; } }
#pragma endregion

	void Update(void);

	// ボタン定義
	enum class KEY_TYPE
	{
		None = -1,

#pragma region プレイヤー操作

		// 移動
		PlayerMoveFront,
		PlayerMoveBack,
		PlayerMoveRight,
		PlayerMoveLeft,

		// ジャンプ
		PlayerJump,

		// ダッシュ
		PlayerDash,

		PlayerKickDownAttack,

#pragma endregion

#pragma region カメラ操作

		// 移動
		CameraMoveFront,
		CameraMoveBack,
		CameraMoveRight,
		CameraMoveLeft,
		CameraMoveUp,
		CameraMoveDown,

		// 回転
		CameraRotUp,
		CameraRotDown,
		CameraRotRight,
		CameraRotLeft,

#pragma endregion

#pragma region システム操作

		// 汎用決定キー
		Enter,

		// 汎用キャンセルキー
		Cancel,

		// 汎用ポーズキー
		Pause,

		// 汎用終了キー
		End,

		// 汎用方向キー（上）
		Up,
		// 汎用方向キー（下）
		Down,
		// 汎用方向キー（左）
		Left,
		// 汎用方向キー（右）
		Right,

#pragma endregion

#pragma region デバッグキー
		// デバッグオブジェクト操作（前）
		DebugObjectFront,
		// デバッグオブジェクト操作（後）
		DebugObjectBack,
		// デバッグオブジェクト操作（右）
		DebugObjectRight,
		// デバッグオブジェクト操作（左）
		DebugObjectLeft,
		// デバッグオブジェクト操作（上）
		DebugObjectUp,
		// デバッグオブジェクト操作（下）
		DebugObjectDown,

		// デバッグ描画スイッチ
		DebugDrawSwitch,

		// デバッグシーンの切り替え
		DebugModeSwitch,

		// デバッグモード時元のシーンの更新
		DebugModeToUpdate,

		// シーンの再ロード
		DebugReload,
#pragma endregion

		Max,
	};

	// キーの状態保存構造体
	struct KEY_INFO { bool prev = false, now = false, up = false, down = false; };

	// キーの状態取得
	const KEY_INFO GetInfo(KEY_TYPE k) { return keyInfo[(int)k]; }

	// コントローラーが接続されているかどうか（true = 接続されている、false = 接続されていない）
	bool GetControllerConnect(void) const;

	/// <summary>
	/// コントローラーの右スティックの入力ベクトルを取得（接続されていない場合はゼロベクトルが返る）
	/// </summary>
	/// <param name="normalize">true = 戻り値を正規化する、false = 戻り値を正規化しない</param>
	Vector2 GetLeftStickVec(bool normalize = true) const;

	/// <summary>
	/// コントローラーの右スティックの入力ベクトルを取得（接続されていない場合はゼロベクトルが返る）
	/// </summary>
	/// <param name="normalize">true = 戻り値を正規化する、false = 戻り値を正規化しない</param>
	Vector2 GetRightStickVec(bool normalize = true) const;

	// マウス情報保存構造体
	struct MOUSE_INFO
	{
		Vector2I now = {};
		Vector2I prev = {};
		Vector2 moveVec = {};
	};

	// マウス情報の構造体を取得
	const MOUSE_INFO& GetMouseInfo(void)const { return mouseInfo; }

	// マウスの移動ベクトルを取得
	const Vector2& GetMouseMoveVec(void)const { return mouseInfo.moveVec; }

	/// <summary>
	/// マウスカーソルを中心に固定する設定
	/// (固定した場合GetMouceMove()にてマウスの単位ベクトルを受け取れる)
	/// </summary>
	/// <param name="fixed">true=固定する、false=固定しない</param>
	void SetMouseFixed(bool fixed);

	// 最後の入力の種類(キーボード,マウスorコントローラー)を判別するフラグ（true = コントローラー、false = キーボード,マウス）
	bool LastInputKinds(void)const { return lastInputKinds; }

private:

	// 初期化処理
	void Init(void);
	// 終了処理
	void Release(void);

	// キーに関する更新
	void KeyUpdate(void);

	// キーの情報
	KEY_INFO keyInfo[(int)KEY_TYPE::Max];

	// コントローラーのボタン以外のキーの種類
	enum class CONTROLLER_OTHERS
	{
		LeftstickUp,
		LeftstickDown,
		LeftstickRight,
		LeftstickLeft,

		RightstickUp,
		RightstickDown,
		RightstickRight,
		RightstickLeft,

		LeftTrigger,
		RightTrigger,
	};

	// キーボードの登録情報
	std::vector<int>keyboardFormat[(int)KEY_TYPE::Max];
	// コントローラーのボタンの登録情報
	std::vector<int>controllerButtonFormat[(int)KEY_TYPE::Max];
	// マウスのボタンの登録情報
	std::vector<int>mouseButtonFormat[(int)KEY_TYPE::Max];
	// コントローラーのボタン以外の登録情報
	std::vector<CONTROLLER_OTHERS>controllerOthersFormat[(int)KEY_TYPE::Max];

	// コントローラーのボタン以外の入力情報の更新
	bool ControllerOthersInput(const CONTROLLER_OTHERS& input);

	// マウスに関する更新
	void MouseUpdate(void);

	// マウスの情報
	MOUSE_INFO mouseInfo;

	// マウスの座標を固定するかどうか
	bool mouseFixed;

	// マウスの移動ベクトルの最低しきい値
	const float MOUSE_THRESHOLD = 1.0f;

	// 最後の入力の種類(キーボード,マウスorコントローラー)を判別するフラグ（true = コントローラー、false = キーボード,マウス）
	bool lastInputKinds;
};

using Input = InputManager;
using KEY_TYPE = Input::KEY_TYPE;