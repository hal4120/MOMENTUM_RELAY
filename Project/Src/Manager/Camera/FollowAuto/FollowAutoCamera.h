#pragma once

#include "../CameraBase.h"

class FollowAutoCamera : public CameraBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="targetPos">追従対象</param>
	/// <param name="focusPos">常に画角に収めたい対象</param>
	/// <param name="TARGET_DISTANCE_MIN">追従対象からカメラ座標までの最低距離</param>
	/// <param name="TARGET_DISTANCE_MAX">追従対象からカメラ座標までの最大距離</param>
	/// <param name="fov"></param>
	FollowAutoCamera(
		const Vector3* targetPos,
		const Vector3* focusPos,
		float TARGET_DISTANCE_MIN = 350.0f, float TARGET_DISTANCE_MAX = 400.0f,
		float fov = 80.0f * (DX_PI_F / 180.0f)
	);

	// デストラクタ
	~FollowAutoCamera()override = default;

	// 追従対象を途中で変更する
	void TargetChange(const Vector3* targetPos) {
		if (targetPos == nullptr) { return; }
		this->targetPos = targetPos;
	}

	// 画角に収めたい対象を途中で変更する
	void FollowAutoToLookTargetChange(const Vector3* focusPos) {
		if (focusPos == nullptr) { return; }
		this->focusPos = focusPos;
	}

private:

	// カメラ最低地上高
	static constexpr float CAMERA_DOWN = 85.0f;

	// 注視点最低地上高
	static constexpr float FOCUS_DOWN = 70.0f;


	// 追従対象
	const Vector3* targetPos;

	// 画角に収めたい対象
	const Vector3* focusPos;

	// 注視点
	Vector3 lookAtPos;

	// 最低距離
	float TARGET_DISTANCE_MIN;

	// 最大距離
	float TARGET_DISTANCE_MAX;

	// 更新
	void NormalUpdate(void)override;
};