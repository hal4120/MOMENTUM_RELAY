#pragma once

#include <stdexcept>

#include "CameraBase.h"

class CurrentCamera
{
public:

	// æ“¾
	static CameraBase& Get(void) {
		if (camera == nullptr) { throw std::runtime_error("ƒJƒƒ‰‚ª‘¶İ‚µ‚È‚¢ó‘Ô‚ÅCurrentCamera‚Ö‚ÌQÆ‚ª‚ ‚è‚Ü‚µ‚½"); }
		return *camera;
	}
	// İ’è
	static void Set(CameraBase* currentCamera) { camera = currentCamera; }

private:

	// Œ»İ‚ÌƒJƒƒ‰
	static CameraBase* camera;

	CurrentCamera() = default;
	~CurrentCamera() = default;
};