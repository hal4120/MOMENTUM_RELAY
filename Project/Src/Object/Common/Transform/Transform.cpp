#include "Transform.h"

#include "../../../Utility/Utility.h"

Vector3 Transform::Velocity(void) const
{
	return pos - prevPos;
}

MATRIX Transform::RotationMat(void) const
{
	return rotation.ToMatrix();
}

Quaternion Transform::FinalRotation(void) const
{
	// ローカル補正 → 本体回転

	const MATRIX localMat = localRotation.ToMatrix();
	const MATRIX rotationMat = rotation.ToMatrix();

	const MATRIX finalMat = MMult(localMat, rotationMat);

	return Quaternion::FromMatrix(finalMat);
}

MATRIX Transform::FinalRotationMat(void) const
{
	return MMult(localRotation.ToMatrix(), rotation.ToMatrix());
}

Vector3 Transform::VTrans(const Vector3& v) const
{
	if (v == 0.0f) { return Vector3(); }

	return Vector3(VTransform(v.ToVECTOR(), RotationMat()));
}

Vector3 Transform::VTrans(const VECTOR& v) const
{
	return VTrans(Vector3(v));
}

#pragma region 回転設定

void Transform::SetRotation(const Quaternion& rotation)
{
	this->rotation = rotation.Normalized();
}

void Transform::SetAngle(const Vector3& angle)
{
	rotation = Quaternion::FromEulerXZY(angle);
}

void Transform::SetAngleDeg(const Vector3& deg)
{
	SetAngle(Deg2Rad(deg));
}

void Transform::SetAngleXDeg(float deg)
{
	SetAngle(Vector3::Xonly(Deg2Rad(deg)));
}

void Transform::SetAngleYDeg(float deg)
{
	SetAngle(Vector3::Yonly(Deg2Rad(deg)));
}

void Transform::SetAngleZDeg(float deg)
{
	SetAngle(Vector3::Zonly(Deg2Rad(deg)));
}

void Transform::AddAngle(const Vector3& angle)
{
	const Quaternion addRotation = Quaternion::FromEulerXZY(angle);

	// 回転の合成順序を確実に既存の行列規約へ合わせる。
	const MATRIX result = MMult(rotation.ToMatrix(), addRotation.ToMatrix());

	rotation = Quaternion::FromMatrix(result);
}

void Transform::AddAngleDeg(const Vector3& deg)
{
	AddAngle(Deg2Rad(deg));
}

void Transform::AddAngleXDeg(float deg)
{
	AddAngle(Vector3::Xonly(Deg2Rad(deg)));
}

void Transform::AddAngleYDeg(float deg)
{
	AddAngle(Vector3::Yonly(Deg2Rad(deg)));
}

void Transform::AddAngleZDeg(float deg)
{
	AddAngle(Vector3::Zonly(Deg2Rad(deg)));
}

void Transform::SetLocalRotation(const Quaternion& rotation)
{
	localRotation = rotation.Normalized();
}

void Transform::SetLocalAngle(const Vector3& angle)
{
	localRotation = Quaternion::FromEulerXZY(angle);
}

void Transform::SetLocalAngleDeg(const Vector3& deg)
{
	SetLocalAngle(Deg2Rad(deg));
}

#pragma endregion

void Transform::LoadModel(std::string path)
{
	model = MV1LoadModel(("Data/Model/" + path + ".mv1").c_str());
}

void Transform::Duplicate(int model)
{
	this->model = MV1DuplicateModel(model);
}

void Transform::LoadEffect(std::string path)
{
	model = LoadEffekseerEffect(("Data/Effect/" + path + ".efk").c_str());
}

void Transform::Attach(void)
{
	if (model == -1) { return; }

	// 最終的な回転
	const MATRIX rotationMat = FinalRotationMat();

	// centerDiffも最終的なモデル姿勢に合わせて回転させる。
	const Vector3 drawPos = pos + Vector3(VTransform(centerDiff.ToVECTOR(), rotationMat));

	// スケール
	MATRIX modelMat = MGetScale(scale.ToVECTOR());

	// 回転
	modelMat = MMult(modelMat, rotationMat);

	// 座標
	modelMat = MMult(modelMat, MGetTranslate(drawPos.ToVECTOR()));

	// モデルへ適用
	MV1SetMatrix(model, modelMat);
}

void Transform::Draw(void)
{
	// 動的オブジェクトは毎フレームAttachしなおす
	if (dynamicFlg) { Attach(); }

	// 描画
	MV1DrawModel(model);
}

void Transform::Release(void)
{
	if (model == -1) { return; }

	MV1DeleteModel(model);
}