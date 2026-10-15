#include "CameraBase.h"

#include "../../Utility/Utility.h"

#include "../../Application/Application.h"

#include "../Input/InputManager.h"
#include "../TimeScale/TimeScale.h"

#include "CameraEvent/CameraEventBase.h"

// カメラ回転におけるマウス感度
float CameraBase::mouseRotSensi = CameraBase::DEFAULT_MOUSE_ROT_SENSI;
// カメラ回転におけるコントローラー感度
float CameraBase::controllerRotSensi = CameraBase::DEFAULT_CONTROLLER_ROT_SENSI;
// カメラ移動における補間数値
float CameraBase::smoothCameraMoveRate = CameraBase::DEFAULT_SMOOTH_CAMERA_MOVE_RATE;

CameraBase::CameraBase(const Vector3& pos, const Quaternion& rotation, float fov) :
	pos(pos), rotation(rotation.Normalized()), fov(fov),

	useTimeScale(true),

	cameraEvent(nullptr)
{
}

CameraBase::CameraBase(const Vector3& pos, const Vector3& angle, float fov) :
	CameraBase(pos, Quaternion::FromEulerXZY(angle), fov)
{
}

void CameraBase::Update(void)
{
	// カメライベントがあればそれを更新する
	if (cameraEvent) {

		// イベント更新
		cameraEvent->Update(*this);

		// イベント終了チェック
		if (cameraEvent->IsEnd()) { EndEvent(); }
	}
	else {
		// なければ通常の更新
		NormalUpdate();
	}
}

void CameraBase::Apply(void) 
{
	// クリップ距離を設定する(ClearDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// 視野角を設定する(ClearDrawScreenでリセットされる)
	SetupCamera_Perspective(fov);

	// カメラ情報の適用
	const MATRIX viewMatrix = MMult(
		MGetTranslate((-pos).ToVECTOR()),
		rotation.Inversed().ToMatrix()
	);
	SetCameraViewMatrix(viewMatrix);
}

void CameraBase::DrawDebug(void) const
{
	if (App::GetIns().IsDrawDebug()) {
		DrawFormatString(
			0, 70, 0xffffff,
			"カメラ座標　 ：(% .1f, % .1f, % .1f)",
			pos.x, pos.y, pos.z
		);
		const Vector3 forward = rotation.Rotate(Vector3(0.0f, 0.0f, 1.0f));
		DrawFormatString(
			0, 90, 0xffffff,
			"カメラ前方向 ：(% .2f, % .2f, % .2f)",
			forward.x, forward.y, forward.z
		);
		DrawFormatString(
			0, 110, 0xffffff,
			"カメラ回転Q ：(% .3f, % .3f, % .3f, % .3f)",
			rotation.x, rotation.y, rotation.z, rotation.w
		);
	}
}

void CameraBase::Release(void)
{
	// 派生での追加終了処理
	SubRelease();

	// 現在イベントがあれば強制終了する
	EndEvent();
}

float CameraBase::GetTimeScale(void)const
{
	return (useTimeScale) ? TimeScale::Get() : 1.0f;
}

#pragma region イベント関数

void CameraBase::StartEvent(CameraEventBase* event)
{
	// 安全処理
	if (event == nullptr) { return; }

	// すでにイベント中なら現在のものを終了
	EndEvent();

	cameraEvent = event;
	cameraEvent->Start(*this);
}

void CameraBase::EndEvent(void)
{
	// 現在イベントがなければ、処理なし
	if (cameraEvent == nullptr) { return; }

	// 終了
	cameraEvent->End(*this);
	delete cameraEvent;
	cameraEvent = nullptr;
}

bool CameraBase::RotationInput(Vector3& out)
{
	// コントローラーの右スティックベクトルを代入
	out = Input::GetIns().GetRightStickVec(false).ToVector3YX() * controllerRotSensi;
	// 入力があれば終了
	if (out != 0.0f) { return true; }

	// コントローラーの右スティックが入力なしならマウスの移動ベクトルを代入
	out = Input::GetIns().GetMouseMoveVec().ToVector3YX() * mouseRotSensi;
	// 入力があれば終了
	if (out != 0.0f) { return true; }

	// マウスが動いてなかったらボタンでの入力を検出してボタンごとに回転方向を 加算/減算 していく
	if (Input::GetIns().GetInfo(KEY_TYPE::CameraRotLeft).now) { out.y--; }
	if (Input::GetIns().GetInfo(KEY_TYPE::CameraRotRight).now) { out.y++; }
	if (Input::GetIns().GetInfo(KEY_TYPE::CameraRotUp).now) { out.x--; }
	if (Input::GetIns().GetInfo(KEY_TYPE::CameraRotDown).now) { out.x++; }
	// 入力があれば正規化して終了
	if (out != 0.0f) { out.Normalize(); return true; }

	// 最後まで入力がなければ未入力として「false」を返して終了
	return false;
}

bool CameraBase::MoveInput(Vector3& out)
{
	// コントローラーの左スティックベクトルを代入
	out = Input::GetIns().GetLeftStickVec(false).ToVector3XZInvertY() * controllerRotSensi;

	// コントローラーの左スティックの入力が検出されなかった場合、ボタンでの入力を検出してボタンごとに移動方向を 加算/減算 していく
	if (out == 0.0f) {
		if (Input::GetIns().GetInfo(KEY_TYPE::CameraMoveFront).now) { out.z++; }
		if (Input::GetIns().GetInfo(KEY_TYPE::CameraMoveBack).now) { out.z--; }
		if (Input::GetIns().GetInfo(KEY_TYPE::CameraMoveRight).now) { out.x++; }
		if (Input::GetIns().GetInfo(KEY_TYPE::CameraMoveLeft).now) { out.x--; }
		out.Normalize();
	}

	// Y軸はボタンのため共通して検出する
	if (Input::GetIns().GetInfo(KEY_TYPE::CameraMoveUp).now) { out.y++; }
	if (Input::GetIns().GetInfo(KEY_TYPE::CameraMoveDown).now) { out.y--; }

	return out != 0.0f;
}

void CameraBase::SmoothCameraMove(Vector3& cameraPos, const Vector3& desiredPos)
{
	cameraPos += ((desiredPos - cameraPos) * smoothCameraMoveRate) * TimeScale::Get();
}

#pragma endregion
