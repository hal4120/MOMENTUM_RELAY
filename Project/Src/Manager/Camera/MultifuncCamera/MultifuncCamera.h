#pragma once

#include "../CameraBase.h"

class MultifuncCamera : public CameraBase
{
public:

	// カメラモード
	enum class MODE
	{
		// 定点
		FixedPoint,

		// フリー
		Free,

		// ディスプレイ（手動回転）
		DisplayRemote,

		// ディスプレイ（自動回転）
		DisplayAuto,

		// 追従（手動操作）
		FollowRemote,

		// 追従（Y軸回転のみの手動操作）
		FollowYaw,

		// 追従（自動操作）
		FollowAuto,

		Max
	};

#pragma region コンストラクタ

	/// <summary>
	/// 定点モードとして生成
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="angle">角度</param>
	/// <param name="fov">視野角</param>
	MultifuncCamera(const Vector3& pos, const Vector3& angle, float fov = 60.0f * (DX_PI_F / 180.0f));

	/// <summary>
	/// フリーモードとして生成
	/// </summary>
	/// <param name="MOVE_POWER">移動速度</param>
	/// <param name="ROT_POWER">回転速度</param>
	/// <param name="pos">初期座標</param>
	/// <param name="angle">初期角度</param>
	/// <param name="fov">視野角</param>
	MultifuncCamera(float MOVE_POWER, float ROT_POWER, const Vector3& pos = Vector3(), const Vector3& angle = Vector3(), float fov = (60.0f * (DX_PI_F / 180.0f)));

	/// <summary>
	/// ディスプレイ（手動回転）または ディスプレイ（自動回転）モードとして生成
	/// </summary>
	/// <param name="mode">true = ディスプレイ（手動回転）、false = ディスプレイ（自動回転）</param>
	/// <param name="fixedLookAtPos">注視点（固定）</param>
	/// <param name="lookAtDiff">注視点からの相対座標</param>
	/// <param name="ROT_POWER">回転量</param>
	/// <param name="angle">初期角度</param>
	/// <param name="fov">視野角</param>
	MultifuncCamera(
		bool mode,
		const Vector3& fixedLookAtPos,
		const Vector3& lookAtDiff = Vector3::Zonly(-400),
		float ROT_POWER = 0.5f * (DX_PI_F / 180.0f),
		const Vector3& angle = Vector3(),
		float fov = 60.0f * (DX_PI_F / 180.0f)
	);

	/// <summary>
	/// 追従（手動操作）または 追従（Y軸回転のみの手動操作）モードとして生成
	/// </summary>
	/// <param name="mode">true = 追従（手動操作）、false = 追従（Y軸回転のみの手動操作）</param>
	/// <param name="targetPos">追従対象</param>
	/// <param name="cameraOffset">カメラ座標を割り出す追従対象からの相対座標</param>
	/// <param name="lookAtOffset">注視点を割り出す追従対象からの相対座標</param>
	/// <param name="ROT_POWER">回転量</param>
	/// <param name="angle">初期角度</param>
	/// <param name="fov">視野角</param>
	MultifuncCamera(
		bool mode,
		const Vector3* targetPos,
		const Vector3& cameraOffset = Vector3::Zonly(-400),
		const Vector3& lookAtOffset = Vector3(),
		float ROT_POWER = 3.0f * (DX_PI_F / 180.0f),
		const Vector3& angle = Vector3(),
		float fov = 60.0f * (DX_PI_F / 180.0f)
	);

	/// <summary>
	/// 追従（自動操作）モードとして生成
	/// </summary>
	/// <param name="targetPos">追従対象</param>
	/// <param name="focusPos">常に画角に収めたい対象</param>
	/// <param name="TARGET_DISTANCE_MIN">追従対象からカメラ座標までの最低距離</param>
	/// <param name="TARGET_DISTANCE_MAX">追従対象からカメラ座標までの最大距離</param>
	/// <param name="fov"></param>
	MultifuncCamera(
		const Vector3* targetPos,
		const Vector3* focusPos,
		float TARGET_DISTANCE_MIN = 350.0f, float TARGET_DISTANCE_MAX = 400.0f,
		float fov = 80.0f * (DX_PI_F / 180.0f)
	);

#pragma endregion

	// デストラクタ
	~MultifuncCamera()override = default;

#pragma region 主要関数

	// 初期化
	void Init(void)override;

#pragma endregion


#pragma region カメラモード切り替え

	/// <summary>
	/// 定点モードに切り替え
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="angle">角度</param>
	/// <param name="fov">視野角</param>
	void ChangeModeFixedPoint(const Vector3& pos, const Vector3& angle, float fov = 60.0f * (DX_PI_F / 180.0f));

	/// <summary>
	/// フリーモードに切り替え
	/// </summary>
	/// <param name="MOVE_POWER">移動速度</param>
	/// <param name="ROT_POWER">回転速度</param>
	/// <param name="pos">初期座標</param>
	/// <param name="angle">初期角度</param>
	/// <param name="fov">視野角</param>
	void ChangeModeFree(float MOVE_POWER, float ROT_POWER, const Vector3& pos = Vector3(), const Vector3& angle = Vector3(), float fov = (60.0f * (DX_PI_F / 180.0f)));

	/// <summary>
	/// ディスプレイ（手動回転）モードに切り替え
	/// </summary>
	/// <param name="fixedLookAtPos">注視点（固定）</param>
	/// <param name="lookAtDiff">注視点からの相対座標</param>
	/// <param name="ROT_POWER">回転量</param>
	/// <param name="angle">初期角度</param>
	/// <param name="fov">視野角</param>
	void ChangeModeDisplayRemote(
		const Vector3& fixedLookAtPos,
		const Vector3& lookAtDiff = Vector3::Zonly(-400),
		float ROT_POWER = 3.0f * (DX_PI_F / 180.0f),
		const Vector3& angle = Vector3(),
		float fov = 60.0f * (DX_PI_F / 180.0f)
	);

	/// <summary>
	/// ディスプレイ（自動回転）モードに切り替え
	/// </summary>
	/// <param name="fixedLookAtPos">注視点（固定）</param>
	/// <param name="lookAtDiff">注視点からの相対座標</param>
	/// <param name="ROT_POWER">回転量</param>
	/// <param name="angle">初期角度</param>
	/// <param name="fov">視野角</param>
	void ChangeModeDisplayAuto(
		const Vector3& fixedLookAtPos,
		const Vector3& lookAtDiff = Vector3::Zonly(-400),
		float ROT_POWER = 0.5f * (DX_PI_F / 180.0f),
		const Vector3& angle = Vector3(),
		float fov = 60.0f * (DX_PI_F / 180.0f)
	);

	/// <summary>
	/// 追従（手動操作）モードに切り替え
	/// </summary>
	/// <param name="targetPos">追従対象</param>
	/// <param name="cameraOffset">カメラ座標を割り出す追従対象からの相対座標</param>
	/// <param name="lookAtOffset">注視点を割り出す追従対象からの相対座標</param>
	/// <param name="ROT_POWER">回転量</param>
	/// <param name="angle">初期角度</param>
	/// <param name="fov">視野角</param>
	void ChangeModeFollowRemote(
		const Vector3* targetPos,
		const Vector3& cameraOffset = Vector3::Zonly(-400),
		const Vector3& lookAtOffset = Vector3(),
		float ROT_POWER = 3.0f * (DX_PI_F / 180.0f),
		const Vector3& angle = Vector3(),
		float fov = 60.0f * (DX_PI_F / 180.0f)
	);

	/// <summary>
	/// 追従（Y軸回転のみの手動操作）モードに切り替え
	/// </summary>
	/// <param name="targetPos">追従対象</param>
	/// <param name="cameraOffset">カメラ座標を割り出す追従対象からの相対座標</param>
	/// <param name="lookAtOffset">注視点を割り出す追従対象からの相対座標</param>
	/// <param name="ROT_POWER">回転量</param>
	/// <param name="angle">初期角度</param>
	/// <param name="fov">視野角</param>
	void ChangeModeFollowYaw(
		const Vector3* targetPos,
		const Vector3& cameraOffset = Vector3::Zonly(-400),
		const Vector3& lookAtOffset = Vector3(),
		float ROT_POWER = 3.0f * (DX_PI_F / 180.0f),
		const Vector3& angle = Vector3(),
		float fov = 60.0f * (DX_PI_F / 180.0f)
	);

	/// <summary>
	/// 追従（自動操作）モードに切り替え
	/// </summary>
	/// <param name="targetPos">追従対象</param>
	/// <param name="focusPos">常に画角に収めたい対象</param>
	/// <param name="TARGET_DISTANCE_MIN">追従対象からカメラ座標までの最低距離</param>
	/// <param name="TARGET_DISTANCE_MAX">追従対象からカメラ座標までの最大距離</param>
	/// <param name="fov"></param>
	void ChangeModeFollowAuto(
		const Vector3* targetPos,
		const Vector3* focusPos,
		float TARGET_DISTANCE_MIN = 350.0f, float TARGET_DISTANCE_MAX = 400.0f,
		float fov = 80.0f * (DX_PI_F / 180.0f)
	);

#pragma endregion


#pragma region セット関数

	// 追従モードのときの追従対象を途中で変更する。追従モード = FollowRemote or FollowYaw or FollowAuto
	void FollowToTargetChange(const Vector3* targetPos) {

		// 安全処理
		if (targetPos == nullptr) { return; }

		// 追従モード以外のときは処理なし
		if (mode != MODE::FollowRemote && mode != MODE::FollowYaw && mode != MODE::FollowAuto) { return; }

		// セット
		this->targetPos = targetPos;
	}

	// 追従(自動操作)モードのときの視野に含める対象物を途中で変更する。追従(自動操作)モード = FOLLOW_AUTO
	void FollowAutoToLookTargetChange(const Vector3* focusPos) {

		// 安全処理
		if (focusPos == nullptr) { return; }

		// 追従(自動操作)モード以外のときは処理なし
		if (mode != MODE::FollowAuto) { return; }

		// セット
		this->focusPos = focusPos;
	}

#pragma endregion

private:

	// モード
	MODE mode;

	// モード別更新処理を格納する配列
	void (MultifuncCamera::* modeFunc[(int)MODE::Max])(void);
#define SET_MODE_FUNC(mode, func) { modeFunc[(int)(mode)] = static_cast<void (MultifuncCamera::*)(void)>(func); }

#pragma region 定点カメラ（FixedPoint）
	// 更新処理
	void FixedPointModeFunc(void);
#pragma endregion

#pragma region フリー（Free）
	// 更新処理
	void FreeModeFunc(void);

	// 移動量
	float MOVE_POWER;

	// 回転量
	float ROT_POWER;
#pragma endregion

#pragma region ディスプレイ（手動回転）（DsiplayRemote）
	// 更新処理
	void DisplayRemoteModeFunc(void);

	// 注視点（固定）
	Vector3 fixedLookAtPos;

	// 注視点からの相対座標
	Vector3 lookAtDiff;

	// 回転量
	//float ROT_POWER;
	// ↑Freeのものを流用

	// 操作角度
	Vector3 controlAngle;
#pragma endregion

#pragma region ディスプレイ（自動回転）（DisplayAuto）
	// 更新処理
	void DisplayAutoModeFunc(void);

	// 注視点（固定）
	//const Vector3 fixedLookAtPos;
	// ↑LookAtFreeのものを流用

	// 注視点からの相対座標
	//const Vector3 lookAtDiff;
	// ↑LookAtFreeのものを流用

	// 回転量
	//float ROT_POWER;
	// ↑Freeのものを流用

	// 操作角度
	//Vector3 controlAngle;
	// ↑DsiplayRemoteのものを流用
#pragma endregion

#pragma region 追従（手動操作）（FollowRemote）
	// 更新処理
	void FollowRemoteModeFunc(void);

	// 追従対象
	const Vector3* targetPos;

	// カメラ座標を割り出す追従対象からの相対座標
	Vector3 cameraOffset;

	// 注視点を割り出す追従対象からの相対座標
	Vector3 lookAtOffset;
	// 注視点
	Vector3 lookAtPos;

	// 回転量
	//float ROT_POWER;
	// ↑Freeのものを流用

	// 操作角度
	//Vector3 controlAngle;
	// ↑DsiplayRemoteのものを流用
#pragma endregion

#pragma region 追従（Y軸回転のみの手動操作）（FollowYaw）
	// 更新処理
	void FollowYawModeFunc(void);

	// 追従対象
	//const Vector3* targetPos;
	// ↑FollowRemoteのものを流用

	// カメラ座標を割り出す追従対象からの相対座標
	//Vector3 cameraOffset;
	// ↑FollowRemoteのものを流用

	// 注視点を割り出す追従対象からの相対座標
	//Vector3 lookAtOffset;
	// ↑FollowRemoteのものを流用
	// 注視点
	//Vector3 lookAtPos;
	// ↑FollowRemoteのものを流用

	// 回転量
	//float ROT_POWER;
	// ↑Freeのものを流用

	// 操作角度
	//Vector3 controlAngle;
	// ↑DsiplayRemoteのものを流用
#pragma endregion

#pragma region 追従（自動操作）（FollowAuto）
	// 更新処理
	void FollowAutoModeFunc(void);

	// カメラ最低地上高
	static constexpr float CAMERA_DOWN = 200.0f;

	// 注視点最低地上高
	static constexpr float FOCUS_DOWN = 70.0f;

	// 追従対象
	//const Vector3* targetPos;
	// ↑FollowRemoteのものを流用

	// 画角に収めたい対象
	const Vector3* focusPos;

	// 注視点
	//Vector3 lookAtPos;
	// ↑FollowRemoteのものを流用

	// 最低距離
	float TARGET_DISTANCE_MIN;

	// 最大距離
	float TARGET_DISTANCE_MAX;
#pragma endregion

	// 更新
	void NormalUpdate(void)override;

	// 解放
	void SubRelease(void)override;
};