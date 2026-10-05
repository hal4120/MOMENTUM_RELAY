#include "Quaternion.h"

#include <algorithm>
#include <cmath>

#include "Vector3.h"


Quaternion::Quaternion(void) :
	x(0.0f),
	y(0.0f),
	z(0.0f),
	w(1.0f)
{
}

Quaternion::Quaternion(float x, float y, float z, float w) :
	x(x),
	y(y),
	z(z),
	w(w)
{
}


Quaternion Quaternion::Identity(void)
{
	return Quaternion();
}


Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle)
{
	const float axisLengthSq =
		axis.x * axis.x +
		axis.y * axis.y +
		axis.z * axis.z;

	// 回転軸として使用できない場合は単位クォータニオンを返す
	if (axisLengthSq <= 0.000001f)
	{
		return Identity();
	}

	const float axisLength = sqrtf(axisLengthSq);

	const float nx = axis.x / axisLength;
	const float ny = axis.y / axisLength;
	const float nz = axis.z / axisLength;

	const float halfAngle = angle * 0.5f;

	const float sinHalf = sinf(halfAngle);
	const float cosHalf = cosf(halfAngle);

	return Quaternion(
		nx * sinHalf,
		ny * sinHalf,
		nz * sinHalf,
		cosHalf
	);
}


Quaternion Quaternion::FromRotationX(float angle)
{
	return FromAxisAngle(
		Vector3(1.0f, 0.0f, 0.0f),
		angle
	);
}

Quaternion Quaternion::FromRotationY(float angle)
{
	return FromAxisAngle(
		Vector3(0.0f, 1.0f, 0.0f),
		angle
	);
}

Quaternion Quaternion::FromRotationZ(float angle)
{
	return FromAxisAngle(
		Vector3(0.0f, 0.0f, 1.0f),
		angle
	);
}


Quaternion Quaternion::FromEulerXZY(const Vector3& angle)
{
	/*
		ここでは既存ベースの
		MatrixAllMultXZY()

		X → Z → Y

		と同じ回転結果になることを優先する。

		Quaternionの乗算規約を手計算で決め打ちせず、
		既存と同じDxLibの回転行列を作成してから
		Quaternionへ変換する。
	*/

	MATRIX matrix = MGetIdent();

	matrix = MMult(matrix, MGetRotX(angle.x));
	matrix = MMult(matrix, MGetRotZ(angle.z));
	matrix = MMult(matrix, MGetRotY(angle.y));

	return FromMatrix(matrix);
}


Quaternion Quaternion::FromMatrix(const MATRIX& matrix)
{
	/*
		入力は純粋な回転行列を想定する。

		モデルのワールド行列など、
		スケールを含む行列を渡す場合は
		呼び出し側でスケールを除去してから渡すこと。
	*/

	const float m00 = matrix.m[0][0];
	const float m01 = matrix.m[0][1];
	const float m02 = matrix.m[0][2];

	const float m10 = matrix.m[1][0];
	const float m11 = matrix.m[1][1];
	const float m12 = matrix.m[1][2];

	const float m20 = matrix.m[2][0];
	const float m21 = matrix.m[2][1];
	const float m22 = matrix.m[2][2];

	Quaternion result;

	const float trace = m00 + m11 + m22;

	if (trace > 0.0f) {

		const float s = sqrtf(trace + 1.0f) * 2.0f;

		result.w = 0.25f * s;
		result.x = (m12 - m21) / s;
		result.y = (m20 - m02) / s;
		result.z = (m01 - m10) / s;
	}
	else if (m00 > m11 && m00 > m22) {
		const float s = sqrtf(1.0f + m00 - m11 - m22) * 2.0f;

		result.w = (m12 - m21) / s;
		result.x = 0.25f * s;
		result.y = (m01 + m10) / s;
		result.z = (m02 + m20) / s;
	}
	else if (m11 > m22) {
		const float s = sqrtf(1.0f + m11 - m00 - m22) * 2.0f;

		result.w = (m20 - m02) / s;
		result.x = (m01 + m10) / s;
		result.y = 0.25f * s;
		result.z = (m12 + m21) / s;
	}
	else {
		const float s = sqrtf(1.0f + m22 - m00 - m11) * 2.0f;

		result.w = (m01 - m10) / s;
		result.x = (m02 + m20) / s;
		result.y = (m12 + m21) / s;
		result.z = 0.25f * s;
	}

	return result.Normalized();
}


Quaternion Quaternion::operator*(const Quaternion& value) const
{
	return Quaternion(
		w * value.x + x * value.w + y * value.z - z * value.y,
		w * value.y - x * value.z + y * value.w + z * value.x,
		w * value.z + x * value.y - y * value.x + z * value.w,
		w * value.w - x * value.x - y * value.y - z * value.z
	);
}

void Quaternion::operator*=(const Quaternion& value)
{
	*this = *this * value;
}

bool Quaternion::operator==(const Quaternion& value) const
{
	return
		x == value.x &&
		y == value.y &&
		z == value.z &&
		w == value.w;
}

bool Quaternion::operator!=(const Quaternion& value) const
{
	return !(*this == value);
}


float Quaternion::Length(void) const
{
	return sqrtf(LengthSq());
}

float Quaternion::LengthSq(void) const
{
	return
		x * x +
		y * y +
		z * z +
		w * w;
}


Quaternion Quaternion::Normalized(void) const
{
	const float length = Length();

	if (length <= 0.000001f)
	{
		return Identity();
	}

	const float invLength = 1.0f / length;

	return Quaternion(
		x * invLength,
		y * invLength,
		z * invLength,
		w * invLength
	);
}

void Quaternion::Normalize(void)
{
	*this = Normalized();
}


Quaternion Quaternion::Conjugated(void) const
{
	return Quaternion(
		-x,
		-y,
		-z,
		w
	);
}


Quaternion Quaternion::Inversed(void) const
{
	const float lengthSq = LengthSq();

	if (lengthSq <= 0.000001f)
	{
		return Identity();
	}

	const Quaternion conjugate = Conjugated();

	const float invLengthSq = 1.0f / lengthSq;

	return Quaternion(
		conjugate.x * invLengthSq,
		conjugate.y * invLengthSq,
		conjugate.z * invLengthSq,
		conjugate.w * invLengthSq
	);
}


float Quaternion::Dot(
	const Quaternion& q1,
	const Quaternion& q2
)
{
	return
		q1.x * q2.x +
		q1.y * q2.y +
		q1.z * q2.z +
		q1.w * q2.w;
}


MATRIX Quaternion::ToMatrix(void) const
{
	const Quaternion q = Normalized();

	const float xx = q.x * q.x;
	const float yy = q.y * q.y;
	const float zz = q.z * q.z;

	const float xy = q.x * q.y;
	const float xz = q.x * q.z;
	const float yz = q.y * q.z;

	const float wx = q.w * q.x;
	const float wy = q.w * q.y;
	const float wz = q.w * q.z;

	MATRIX result = MGetIdent();

	/*
		DxLibで現在使用している行列規約に合わせた形。
		FromMatrix() と対になる。
	*/

	result.m[0][0] = 1.0f - 2.0f * (yy + zz);
	result.m[0][1] = 2.0f * (xy + wz);
	result.m[0][2] = 2.0f * (xz - wy);

	result.m[1][0] = 2.0f * (xy - wz);
	result.m[1][1] = 1.0f - 2.0f * (xx + zz);
	result.m[1][2] = 2.0f * (yz + wx);

	result.m[2][0] = 2.0f * (xz + wy);
	result.m[2][1] = 2.0f * (yz - wx);
	result.m[2][2] = 1.0f - 2.0f * (xx + yy);

	return result;
}


Vector3 Quaternion::Rotate(const Vector3& vec) const
{
	return Vector3(
		VTransform(
			vec.ToVECTOR(),
			ToMatrix()
		)
	);
}


Quaternion Quaternion::Slerp(
	const Quaternion& start,
	const Quaternion& end,
	float rate
)
{
	Quaternion q1 = start.Normalized();
	Quaternion q2 = end.Normalized();

	rate = std::clamp(rate, 0.0f, 1.0f);

	float dot = Dot(q1, q2);

	/*
		q と -q は同じ回転を表す。

		内積が負の場合、そのまま補間すると
		遠い方の回転経路を通るため、
		q2を反転させて最短経路を使用する。
	*/
	if (dot < 0.0f)
	{
		q2.x = -q2.x;
		q2.y = -q2.y;
		q2.z = -q2.z;
		q2.w = -q2.w;

		dot = -dot;
	}

	dot = std::clamp(dot, -1.0f, 1.0f);

	/*
		ほぼ同じ回転の場合は
		Slerpの計算が不安定になるため線形補間する。
	*/
	if (dot > 0.9995f)
	{
		Quaternion result(
			q1.x + (q2.x - q1.x) * rate,
			q1.y + (q2.y - q1.y) * rate,
			q1.z + (q2.z - q1.z) * rate,
			q1.w + (q2.w - q1.w) * rate
		);

		return result.Normalized();
	}

	const float theta = acosf(dot);
	const float sinTheta = sinf(theta);

	const float weight1 =
		sinf((1.0f - rate) * theta) / sinTheta;

	const float weight2 =
		sinf(rate * theta) / sinTheta;

	Quaternion result(
		q1.x * weight1 + q2.x * weight2,
		q1.y * weight1 + q2.y * weight2,
		q1.z * weight1 + q2.z * weight2,
		q1.w * weight1 + q2.w * weight2
	);

	return result.Normalized();
}