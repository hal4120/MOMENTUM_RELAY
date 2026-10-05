#include "MultifuncCamera.h"

#include <algorithm>

#include "../../Input/InputManager.h"

#include "../../../Utility/Utility.h"

#pragma region コンストラクタ

// 定点モードとして生成
MultifuncCamera::MultifuncCamera(const Vector3& pos, const Vector3& angle, float fov) :
	CameraBase(),

	mode(MODE::FixedPoint),

	modeFunc(),

	MOVE_POWER(), ROT_POWER(),

	fixedLookAtPos(), lookAtDiff(),

	controlAngle(),

	targetPos(nullptr),

	cameraOffset(),

	lookAtOffset(),
	lookAtPos(),

	focusPos(nullptr),

	TARGET_DISTANCE_MIN(),
	TARGET_DISTANCE_MAX()
{
	ChangeModeFixedPoint(pos, angle, fov);
}

// フリーモードとして生成
MultifuncCamera::MultifuncCamera(float MOVE_POWER, float ROT_POWER, const Vector3& pos, const Vector3& angle, float fov) :
	CameraBase(),

	mode(MODE::Free),

	modeFunc(),

	MOVE_POWER(), ROT_POWER(),

	fixedLookAtPos(), lookAtDiff(),

	controlAngle(),

	targetPos(nullptr),

	cameraOffset(),

	lookAtOffset(),
	lookAtPos(),

	focusPos(nullptr),

	TARGET_DISTANCE_MIN(),
	TARGET_DISTANCE_MAX()
{
	ChangeModeFree(MOVE_POWER, ROT_POWER, pos, angle, fov);
}

// ディスプレイ（手動回転）または ディスプレイ（自動回転）モードとして生成
MultifuncCamera::MultifuncCamera(bool mode, const Vector3& fixedLookAtPos, const Vector3& lookAtDiff, float ROT_POWER, const Vector3& angle, float fov) :
	CameraBase(),

	mode(mode ? MODE::DisplayRemote : MODE::DisplayAuto),

	modeFunc(),

	MOVE_POWER(), ROT_POWER(),

	fixedLookAtPos(), lookAtDiff(),

	controlAngle(),

	targetPos(nullptr),

	cameraOffset(),

	lookAtOffset(),
	lookAtPos(),

	focusPos(nullptr),

	TARGET_DISTANCE_MIN(),
	TARGET_DISTANCE_MAX()
{
	if (mode) { ChangeModeDisplayRemote(fixedLookAtPos, lookAtDiff, ROT_POWER, angle, fov); }
	else { ChangeModeDisplayAuto(fixedLookAtPos, lookAtDiff, ROT_POWER, angle, fov); }
}

// 追従（手動操作）または 追従（Y軸回転のみの手動操作）モードとして生成
MultifuncCamera::MultifuncCamera(bool mode, const Vector3* targetPos, const Vector3& cameraOffset, const Vector3& lookAtOffset, float ROT_POWER, const Vector3& angle, float fov) :
	CameraBase(),

	mode(mode ? MODE::FollowRemote : MODE::FollowYaw),

	modeFunc(),

	MOVE_POWER(), ROT_POWER(),

	fixedLookAtPos(), lookAtDiff(),

	controlAngle(),

	targetPos(nullptr),

	cameraOffset(),

	lookAtOffset(),
	lookAtPos(),

	focusPos(nullptr),

	TARGET_DISTANCE_MIN(),
	TARGET_DISTANCE_MAX()
{
	if (mode) { ChangeModeFollowRemote(targetPos, cameraOffset, lookAtOffset, ROT_POWER, angle, fov); }
	else { ChangeModeFollowYaw(targetPos, cameraOffset, lookAtOffset, ROT_POWER, angle, fov); }
}

// 追従（自動操作）モードとして生成
MultifuncCamera::MultifuncCamera(const Vector3* targetPos, const Vector3* focusPos, float TARGET_DISTANCE_MIN, float TARGET_DISTANCE_MAX, float fov) :
	CameraBase(Vector3(), Vector3(), fov),

	mode(MODE::FollowAuto),

	modeFunc(),

	MOVE_POWER(), ROT_POWER(),

	fixedLookAtPos(), lookAtDiff(),

	controlAngle(),

	targetPos(nullptr),

	cameraOffset(),

	lookAtOffset(),
	lookAtPos(),

	focusPos(nullptr),

	TARGET_DISTANCE_MIN(),
	TARGET_DISTANCE_MAX()
{
	ChangeModeFollowAuto(targetPos, focusPos, TARGET_DISTANCE_MIN, TARGET_DISTANCE_MAX, fov);
}

#pragma endregion

#pragma region 主要関数

// 初期化
void MultifuncCamera::Init(void)
{
#pragma region モード別関数を格納

	// 定点
	SET_MODE_FUNC(MODE::FixedPoint, &MultifuncCamera::FixedPointModeFunc);

	// フリー
	SET_MODE_FUNC(MODE::Free, &MultifuncCamera::FreeModeFunc);

	// ディスプレイ（手動回転）
	SET_MODE_FUNC(MODE::DisplayRemote, &MultifuncCamera::DisplayRemoteModeFunc);

	// ディスプレイ（自動回転）
	SET_MODE_FUNC(MODE::DisplayAuto, &MultifuncCamera::DisplayAutoModeFunc);

	// 追従（手動操作）
	SET_MODE_FUNC(MODE::FollowRemote, &MultifuncCamera::FollowRemoteModeFunc);

	// 追従（Y軸回転のみの手動操作）
	SET_MODE_FUNC(MODE::FollowYaw, &MultifuncCamera::FollowYawModeFunc);

	// 追従（自動操作）
	SET_MODE_FUNC(MODE::FollowAuto, &MultifuncCamera::FollowAutoModeFunc);

#pragma endregion
}

// 更新
void MultifuncCamera::NormalUpdate(void)
{
	// モード別関数の呼び出し
	(this->*modeFunc[(int)mode])();
}

// 解放
void MultifuncCamera::SubRelease(void)
{
	switch (mode) {
	case MultifuncCamera::MODE::FollowRemote:
	case MultifuncCamera::MODE::FollowYaw: {
		targetPos = nullptr;
		break;
	}
	case MultifuncCamera::MODE::FollowAuto: {
		targetPos = nullptr;
		focusPos = nullptr;
		break;
	}
	default: { break; }
	}

	Input::GetIns().SetMouseFixed(false);
}

#pragma endregion

#pragma region カメラモード切り替え

// 定点モードに切り替え
void MultifuncCamera::ChangeModeFixedPoint(const Vector3& pos, const Vector3& angle, float fov)
{
	// 現在の情報を破棄
	Release();

	// 状態遷移
	mode = MODE::FixedPoint;

	// 座標を設定
	this->pos = pos;

	// 角度を設定
	this->angle = angle;

	// 視野角を設定
	this->fov = fov;
}

// フリーモードに切り替え
void MultifuncCamera::ChangeModeFree(float MOVE_POWER, float ROT_POWER, const Vector3& pos, const Vector3& angle, float fov)
{
	// 現在の情報を破棄
	Release();

	// マウス設定
	Input::GetIns().SetMouseFixed(true);

	// 状態遷移
	mode = MODE::Free;

	// 回転量
	this->ROT_POWER = ROT_POWER;

	// 移動量
	this->MOVE_POWER = MOVE_POWER;

	// 初期座標
	this->pos = pos;

	// 初期角度
	this->angle = angle;

	// 視野角を設定
	this->fov = fov;
}

// ディスプレイ（手動回転）モードに切り替え
void MultifuncCamera::ChangeModeDisplayRemote(const Vector3& fixedLookAtPos, const Vector3& lookAtDiff, float ROT_POWER, const Vector3& angle, float fov)
{
	// 現在の情報を破棄
	Release();

	// マウス設定
	Input::GetIns().SetMouseFixed(true);

	// 状態遷移
	mode = MODE::DisplayRemote;

	// 注視点
	this->fixedLookAtPos = fixedLookAtPos;

	// 注視点からの相対座標
	this->lookAtDiff = lookAtDiff;

	// 回転量
	this->ROT_POWER = ROT_POWER;

	// 初期操作角度
	controlAngle = angle;

	// 視野角を設定
	this->fov = fov;

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	pos = fixedLookAtPos + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) }));

	// 角度
	this->angle = CalcCameraAngle(pos, fixedLookAtPos);
}

// ディスプレイ（自動回転）モードに切り替え
void MultifuncCamera::ChangeModeDisplayAuto(const Vector3& fixedLookAtPos, const Vector3& lookAtDiff, float ROT_POWER, const Vector3& angle, float fov)
{
	// 現在の情報を破棄
	Release();

	// 状態遷移
	mode = MODE::DisplayAuto;

	// 注視点
	this->fixedLookAtPos = fixedLookAtPos;

	// 注視点からの相対座標
	this->lookAtDiff = lookAtDiff;

	// 回転量
	this->ROT_POWER = ROT_POWER;

	// 初期操作角度
	controlAngle = angle;

	// 視野角を設定
	this->fov = fov;

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	pos = fixedLookAtPos + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) }));

	// 角度
	this->angle = CalcCameraAngle(pos, fixedLookAtPos);
}

// 追従（手動操作）モードに切り替え
void MultifuncCamera::ChangeModeFollowRemote(const Vector3* targetPos, const Vector3& cameraOffset, const Vector3& lookAtOffset, float ROT_POWER, const Vector3& angle, float fov)
{
	if (targetPos == nullptr) { return; }

	// 現在の情報を破棄
	Release();

	// マウス設定
	Input::GetIns().SetMouseFixed(true);

	// 状態遷移
	mode = MODE::FollowRemote;

	// 追従対象を設定
	this->targetPos = targetPos;

	// カメラ座標を割り出す追従対象からの相対座標を設定
	this->cameraOffset = cameraOffset;

	// 注視点を割り出す追従対象からの相対座標を設定
	this->lookAtOffset = lookAtOffset;

	// 回転量を設定
	this->ROT_POWER = ROT_POWER;

	// 初期操作角度
	controlAngle = angle;

	// 視野角を設定
	this->fov = fov;

	// 座標を設定
	pos = *targetPos + cameraOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) }));

	// 注視点を設定
	lookAtPos = *targetPos + lookAtOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x, controlAngle.y) }));

	// 操作角度
	this->angle = CalcCameraAngle(pos, lookAtPos);
}

// 追従（Y軸回転のみの手動操作）モードに切り替え
void MultifuncCamera::ChangeModeFollowYaw(const Vector3* targetPos, const Vector3& cameraOffset, const Vector3& lookAtOffset, float ROT_POWER, const Vector3& angle, float fov)
{
	if (targetPos == nullptr) { return; }

	// 現在の情報を破棄
	Release();

	// マウス設定
	Input::GetIns().SetMouseFixed(true);

	// 状態遷移
	mode = MODE::FollowYaw;

	// 追従対象を設定
	this->targetPos = targetPos;

	// カメラ座標を割り出す追従対象からの相対座標を設定
	this->cameraOffset = cameraOffset;

	// 注視点を割り出す追従対象からの相対座標を設定
	this->lookAtOffset = lookAtOffset;

	// 回転量を設定
	this->ROT_POWER = ROT_POWER;

	// 初期操作角度
	controlAngle = angle;

	// 視野角を設定
	this->fov = fov;

	// 座標を設定
	pos = *targetPos + cameraOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) }));

	// 注視点を設定
	lookAtPos = *targetPos + lookAtOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x, controlAngle.y) }));

	// 操作角度
	this->angle = CalcCameraAngle(pos, lookAtPos);
}

// 追従（自動操作）モードに切り替え
void MultifuncCamera::ChangeModeFollowAuto(const Vector3* targetPos, const Vector3* focusPos, float TARGET_DISTANCE_MIN, float TARGET_DISTANCE_MAX, float fov)
{
	if (targetPos == nullptr || focusPos == nullptr) { return; }

	// 現在の情報を破棄
	Release();

	// 状態遷移
	mode = MODE::FollowAuto;

	// 追従対象を設定
	this->targetPos = targetPos;

	// 視野に入れる対象物
	this->focusPos = focusPos;

	// 最低距離
	this->TARGET_DISTANCE_MIN = TARGET_DISTANCE_MIN;

	// 最大距離
	this->TARGET_DISTANCE_MAX = TARGET_DISTANCE_MAX;

	// 視野角を設定
	this->fov = fov;

	FollowAutoModeFunc();
}

#pragma endregion

// ～～～～～～～～～↓各モードの処理↓～～～～～～～～～

#pragma region 定点カメラ（FixedPoint）

// 更新処理
void MultifuncCamera::FixedPointModeFunc(void)
{

}

#pragma endregion

#pragma region フリー（Free）

// 更新処理
void MultifuncCamera::FreeModeFunc(void)
{
	Vector3 rotInput = Vector3();
	if (RotationInput(rotInput)) {

		// 回転させる
		angle += rotInput * ROT_POWER;

		// 回転の数値制御
		if (angle.y <= Deg2Rad(0.0f)) { angle.y += Deg2Rad(360.0f); }
		if (angle.y >= Deg2Rad(360.0f)) { angle.y -= Deg2Rad(360.0f); }
		if (angle.x <= Deg2Rad(0.0f)) { angle.x += Deg2Rad(360.0f); }
		if (angle.x >= Deg2Rad(360.0f)) { angle.x -= Deg2Rad(360.0f); }
	}

	Vector3 moveInput = Vector3();
	if (MoveInput(moveInput)) {
		// 入力があれば、方向×スピードで移動量を作って、座標に足して移動
		SmoothCameraMove(pos, pos + (moveInput.TransMat(MGetRotY(angle.y)) * MOVE_POWER));
	}
}

#pragma endregion

#pragma region ディスプレイ（手動回転）（DsiplayRemote）

// 更新処理
void MultifuncCamera::DisplayRemoteModeFunc(void)
{
	Vector3 rotInput = Vector3();
	if (RotationInput(rotInput)) {

		// 回転させる
		controlAngle += rotInput * ROT_POWER;

		// 回転の数値制御
		if (controlAngle.y <= Deg2Rad(0.0f)) { controlAngle.y += Deg2Rad(360.0f); }
		if (controlAngle.y >= Deg2Rad(360.0f)) { controlAngle.y -= Deg2Rad(360.0f); }
		if (controlAngle.x < Deg2Rad(-85.0f)) { controlAngle.x = Deg2Rad(-85.0f); }
		if (controlAngle.x > Deg2Rad(85.0f)) { controlAngle.x = Deg2Rad(85.0f); }
	}

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	SmoothCameraMove(pos, fixedLookAtPos + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) })));

	// 角度
	angle = CalcCameraAngle(pos, fixedLookAtPos);
}

#pragma endregion

#pragma region ディスプレイ（自動回転）（DisplayAuto）

// 更新処理
void MultifuncCamera::DisplayAutoModeFunc(void)
{
	// 回転処理（設定された値横向きに回し続ける）
	controlAngle += Vector3::Yonly(1.0f).Normalized() * ROT_POWER;

	if (controlAngle.y >= Deg2Rad(360.0f)) { controlAngle.y -= Deg2Rad(360.0f); }
	if (controlAngle.y <= Deg2Rad(0.0f)) { controlAngle.y += Deg2Rad(360.0f); }

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	SmoothCameraMove(pos, fixedLookAtPos + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) })));

	// 角度
	angle = CalcCameraAngle(pos, fixedLookAtPos);
}

#pragma endregion

#pragma region 追従（手動操作）（FollowRemote）

// 更新処理
void MultifuncCamera::FollowRemoteModeFunc(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (targetPos == nullptr) { return; }

	// 回転処理
	Vector3 rotInput = Vector3();
	if (RotationInput(rotInput)) {

		controlAngle += rotInput * ROT_POWER;

		// 回転の数値制御
		if (controlAngle.y <= Deg2Rad(0.0f)) { controlAngle.y += Deg2Rad(360.0f); }
		if (controlAngle.y >= Deg2Rad(360.0f)) { controlAngle.y -= Deg2Rad(360.0f); }
		if (controlAngle.x < Deg2Rad(-85.0f)) { controlAngle.x = Deg2Rad(-85.0f); }
		if (controlAngle.x > Deg2Rad(85.0f)) { controlAngle.x = Deg2Rad(85.0f); }
	}

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	SmoothCameraMove(pos, *targetPos + cameraOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) })));
	lookAtPos = *targetPos + lookAtOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x, controlAngle.y) }));

	// 角度
	angle = CalcCameraAngle(pos, lookAtPos);
}

#pragma endregion

#pragma region 追従（Y軸回転のみの手動操作）（FollowYaw）

// 更新処理
void MultifuncCamera::FollowYawModeFunc(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (targetPos == nullptr) { return; }

	// 回転処理
	Vector3 rotInput = Vector3();
	if (RotationInput(rotInput)) {

		// Y軸回転のみにする
		rotInput.x = rotInput.z = 0.0f;

		// 回転
		controlAngle += rotInput * ROT_POWER;

		// 回転の数値制御
		if (controlAngle.y <= Deg2Rad(0.0f)) { controlAngle.y += Deg2Rad(360.0f); }
		if (controlAngle.y >= Deg2Rad(360.0f)) { controlAngle.y -= Deg2Rad(360.0f); }
	}

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	SmoothCameraMove(pos, *targetPos + cameraOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) })));
	lookAtPos = *targetPos + lookAtOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x, controlAngle.y) }));

	// 角度
	angle = CalcCameraAngle(pos, lookAtPos);
}

#pragma endregion

#pragma region 追従（自動操作）（FollowAuto）

// 更新処理
void MultifuncCamera::FollowAutoModeFunc(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (targetPos == nullptr || focusPos == nullptr) { return; }

	// ２点間ベクトル
	Vector3 atToTarget = *targetPos - *focusPos;

	// fovから必要距離を計算（縦fov基準）
	float needDist = std::clamp((atToTarget.Length() * 0.5f) / tanf(fov * 0.5f), TARGET_DISTANCE_MIN, TARGET_DISTANCE_MAX);

	// lookTargetからみてlookAtのそのさらに先にカメラをおきたいのでその方向を取得する
	Vector3 backDir = atToTarget.Normalized();

	// 目標カメラ位置
	Vector3 desiredPos = *targetPos + backDir * needDist;

	// 高さ補正
	desiredPos.y += std::clamp(((*targetPos - *focusPos) * 0.5f).Length(), 250.0f, 400.0f);

	// 移動
	SmoothCameraMove(pos, desiredPos);
	if (pos.y <= CAMERA_DOWN) { pos.y = CAMERA_DOWN; }

	// 注視点を算出
	lookAtPos = (*targetPos + *focusPos) * 0.5f;
	if (lookAtPos.y <= FOCUS_DOWN) { lookAtPos.y = FOCUS_DOWN; }

	// 角度
	angle = CalcCameraAngle(pos, lookAtPos);
}

#pragma endregion