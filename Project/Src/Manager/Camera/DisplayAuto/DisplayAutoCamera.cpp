#include "DisplayAutoCamera.h"

#include "../../../Utility/Utility.h"

#include "../../../Application/Application.h"

#include "../../TimeScale/TimeScale.h"

DisplayAutoCamera::DisplayAutoCamera(const Vector3& fixedLookAtPos, const Vector3& lookAtDiff, float ROT_POWER, const Vector3& angle, float fov) :
	CameraBase(Vector3(), Vector3(), fov),

	fixedLookAtPos(fixedLookAtPos),
	lookAtDiff(lookAtDiff),

	ROT_POWER(ROT_POWER),

	controlAngle(angle)
{
	// Œ»Ý‚Ì’Ç]‘ÎÛ‚ÌÀ•W‚ÆŠp“xî•ñ‚©‚çŽ©g(ƒJƒƒ‰)‚ÌÀ•W‚ðŽZo‚·‚é
	pos = fixedLookAtPos + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) }));

	// Šp“x
	this->angle = CalcCameraAngle(pos, fixedLookAtPos);
}

void DisplayAutoCamera::NormalUpdate(void)
{
	// ‰ñ“]ˆ—iÝ’è‚³‚ê‚½’l‰¡Œü‚«‚É‰ñ‚µ‘±‚¯‚éj
	controlAngle += (Vector3::Yonly(1.0f).Normalized() * ROT_POWER) * TimeScale::Get();

	if (controlAngle.y >= Deg2Rad(360.0f)) { controlAngle.y -= Deg2Rad(360.0f); }
	if (controlAngle.y <= Deg2Rad(0.0f)) { controlAngle.y += Deg2Rad(360.0f); }

	// Œ»Ý‚Ì’Ç]‘ÎÛ‚ÌÀ•W‚ÆŠp“xî•ñ‚©‚çŽ©g(ƒJƒƒ‰)‚ÌÀ•W‚ðŽZo‚·‚é
	SmoothCameraMove(pos, fixedLookAtPos + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(controlAngle.x,controlAngle.y) })));

	// Šp“x
	angle = CalcCameraAngle(pos, fixedLookAtPos);
}