#include "FreeCamera.h"

#include "../../../Utility/Utility.h"

#include "../../../Application/Application.h"

#include "../../Input/InputManager.h"
#include "../../TimeScale/TimeScale.h"

FreeCamera::FreeCamera(float MOVE_POWER, float ROT_POWER, AngleTag, const Vector3& pos, const Vector3& angle, float fov) :
	CameraBase(pos, angle, fov),

	MOVE_POWER(MOVE_POWER),
	ROT_POWER(ROT_POWER),
	controlAngle(angle)
{
	// カメラ回転が有効だったらマウスを固定しておく
	if (ROT_POWER != 0.0f) { Input::GetIns().SetMouseFixed(true); }
}

FreeCamera::FreeCamera(float MOVE_POWER, float ROT_POWER, LookAtTag, const Vector3& pos, const Vector3& lookPos, float fov) :
	CameraBase(pos, CalcCameraRotation(pos, lookPos), fov),

	MOVE_POWER(MOVE_POWER),
	ROT_POWER(ROT_POWER),
	controlAngle()
{
	const Vector3 vec = (lookPos - pos).Normalized();
	if (vec.LengthSq() > 0.000001f) {
		controlAngle = Vector3(-asinf(std::clamp(vec.y, -1.0f, 1.0f)), atan2f(vec.x, vec.z), 0.0f);
	}
	// カメラ回転が有効だったらマウスを固定しておく
	if (ROT_POWER != 0.0f) { Input::GetIns().SetMouseFixed(true); }
}

void FreeCamera::NormalUpdate(void)
{
	Vector3 rotInput = Vector3();
	if (RotationInput(rotInput)) {

		// 回転させる
		controlAngle += (rotInput * ROT_POWER) * GetTimeScale();

		// 回転の数値制御
		if (controlAngle.y <= Deg2Rad(0.0f)) { controlAngle.y += Deg2Rad(360.0f); }
		if (controlAngle.y >= Deg2Rad(360.0f)) { controlAngle.y -= Deg2Rad(360.0f); }
		if (controlAngle.x <= Deg2Rad(0.0f)) { controlAngle.x += Deg2Rad(360.0f); }
		if (controlAngle.x >= Deg2Rad(360.0f)) { controlAngle.x -= Deg2Rad(360.0f); }
		rotation = Quaternion::FromEulerXZY(controlAngle);
	}

	Vector3 moveInput = Vector3();
	if (MoveInput(moveInput)) {
		// 入力があれば、方向×スピードで移動量を作って、座標に足して移動
		SmoothCameraMove(pos, pos + (moveInput.TransMat(MGetRotY(controlAngle.y)) * MOVE_POWER) * GetTimeScale());
	}
}

void FreeCamera::SubRelease(void)
{
	// 生成時固定したマウスを戻す
	Input::GetIns().SetMouseFixed(false);
}