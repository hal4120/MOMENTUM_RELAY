#include "FollowRemoteCamera.h"

#include "../../../Utility/Utility.h"

#include "../../../Application/Application.h"

#include "../../Input/InputManager.h"
#include "../../TimeScale/TimeScale.h"

FollowRemoteCamera::FollowRemoteCamera(
	const Vector3* targetPos,
	const Vector3& cameraOffset,
	const Vector3& lookAtOffset,
	float ROT_POWER,
	const Vector3& angle,
	float fov
) :
	CameraBase(Vector3(), Vector3(), fov),

	targetPos(targetPos),

	cameraOffset(cameraOffset),

	lookAtOffset(lookAtOffset),
	lookAtPos(),

	ROT_POWER(ROT_POWER),

	controlAngle(angle)
{
	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	pos = *targetPos + cameraOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) }));
	lookAtPos = *targetPos + lookAtOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x, controlAngle.y) }));

	// 角度
	this->angle = CalcCameraAngle(pos, lookAtPos);

	// カメラ回転が有効だったらマウスを固定しておく
	if (ROT_POWER != 0.0f) { Input::GetIns().SetMouseFixed(true); }
}

void FollowRemoteCamera::NormalUpdate(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (targetPos == nullptr) { return; }

	Vector3 rotInput = Vector3();
	if (RotationInput(rotInput)) {

		controlAngle += (rotInput * ROT_POWER) * TimeScale::Get();

		// 回転の数値制御
		if (controlAngle.y <= Deg2Rad(0.0f)) { controlAngle.y += Deg2Rad(360.0f); }
		if (controlAngle.y >= Deg2Rad(360.0f)) { controlAngle.y -= Deg2Rad(360.0f); }
		if (controlAngle.x < Deg2Rad(-85.0f)) { controlAngle.x = Deg2Rad(-85.0f); }
		if (controlAngle.x > Deg2Rad(85.0f)) { controlAngle.x = Deg2Rad(85.0f); }
	}

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	SmoothCameraMove(pos, *targetPos + cameraOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) })));
	SmoothCameraMove(lookAtPos, *targetPos + lookAtOffset.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x, controlAngle.y) })));

	// 角度
	angle = CalcCameraAngle(pos, lookAtPos);
}

void FollowRemoteCamera::SubRelease(void)
{
	// 生成時固定したマウスを戻す
	Input::GetIns().SetMouseFixed(false);
}
