#include "FreeCamera.h"

#include "../../../Utility/Utility.h"

#include "../../../Application/Application.h"

#include "../../Input/InputManager.h"
#include "../../TimeScale/TimeScale.h"

FreeCamera::FreeCamera(float MOVE_POWER, float ROT_POWER, AngleTag, const Vector3& pos, const Vector3& angle, float fov) :
	CameraBase(pos, angle, fov),

	MOVE_POWER(MOVE_POWER),
	ROT_POWER(ROT_POWER)
{
	// カメラ回転が有効だったらマウスを固定しておく
	if (ROT_POWER != 0.0f) { Input::GetIns().SetMouseFixed(true); }
}

FreeCamera::FreeCamera(float MOVE_POWER, float ROT_POWER, LookAtTag, const Vector3& pos, const Vector3& lookPos, float fov) :
	CameraBase(pos, CalcCameraAngle(pos, lookPos), fov),

	MOVE_POWER(MOVE_POWER),
	ROT_POWER(ROT_POWER)
{
	// カメラ回転が有効だったらマウスを固定しておく
	if (ROT_POWER != 0.0f) { Input::GetIns().SetMouseFixed(true); }
}

void FreeCamera::NormalUpdate(void)
{
	Vector3 rotInput = Vector3();
	if (RotationInput(rotInput)) {

		// 回転させる
		angle += (rotInput * ROT_POWER) * TimeScale::Get();

		// 回転の数値制御
		if (angle.y <= Deg2Rad(0.0f)) { angle.y += Deg2Rad(360.0f); }
		if (angle.y >= Deg2Rad(360.0f)) { angle.y -= Deg2Rad(360.0f); }
		if (angle.x <= Deg2Rad(0.0f)) { angle.x += Deg2Rad(360.0f); }
		if (angle.x >= Deg2Rad(360.0f)) { angle.x -= Deg2Rad(360.0f); }
	}

	Vector3 moveInput = Vector3();
	if (MoveInput(moveInput)) {
		// 入力があれば、方向×スピードで移動量を作って、座標に足して移動
		SmoothCameraMove(pos, pos + (moveInput.TransMat(MGetRotY(angle.y)) * MOVE_POWER) * TimeScale::Get());
	}
}

void FreeCamera::SubRelease(void)
{
	// 生成時固定したマウスを戻す
	Input::GetIns().SetMouseFixed(false);
}