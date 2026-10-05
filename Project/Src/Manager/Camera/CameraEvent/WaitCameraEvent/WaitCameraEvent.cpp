#include "WaitCameraEvent.h"

#include <algorithm>

#include "../../../TimeScale/TimeScale.h"

WaitCameraEvent::WaitCameraEvent(float frame) :
	CameraEventBase(),

	currentFrame(0),
	maxFrame(std::max(frame, 1.0f))
{
}

void WaitCameraEvent::Update(CameraBase& camera)
{
	currentFrame += TimeScale::Get();
}