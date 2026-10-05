#include "Vector3.h"

#include <cmath>
#include <algorithm>

#include "Vector2.h"

#pragma region ÉRÉìÉXÉgÉâÉNÉ^

Vector3::Vector3(void)
	: x(0.0f), y(0.0f), z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z)
{
}

Vector3::Vector3(float value)
	: x(value), y(value), z(value)
{
}

Vector3::Vector3(int x, int y, int z)
	: x(static_cast<float>(x)),
	y(static_cast<float>(y)),
	z(static_cast<float>(z))
{
}

Vector3::Vector3(int value)
	: x(static_cast<float>(value)),
	y(static_cast<float>(value)),
	z(static_cast<float>(value))
{
}

Vector3::Vector3(const VECTOR& value)
	: x(value.x), y(value.y), z(value.z)
{
}

#pragma endregion


#pragma region Vector3ê∂ê¨(ì¡éÍ)

Vector3 Vector3::Xonly(float value)
{
	return Vector3(value, 0.0f, 0.0f);
}

Vector3 Vector3::Yonly(float value)
{
	return Vector3(0.0f, value, 0.0f);
}

Vector3 Vector3::Zonly(float value)
{
	return Vector3(0.0f, 0.0f, value);
}

Vector3 Vector3::XYonly(float x, float y)
{
	return Vector3(x, y, 0.0f);
}

Vector3 Vector3::XZonly(float x, float z)
{
	return Vector3(x, 0.0f, z);
}

Vector3 Vector3::YZonly(float y, float z)
{
	return Vector3(0.0f, y, z);
}

#pragma endregion


#pragma region ââéZ

// ë„ì¸
void Vector3::operator=(const float value)
{
	x = value;
	y = value;
	z = value;
}

void Vector3::operator=(const Vector3& value)
{
	x = value.x;
	y = value.y;
	z = value.z;
}

void Vector3::operator=(const VECTOR& value)
{
	x = value.x;
	y = value.y;
	z = value.z;
}


// â¡éZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector3 Vector3::operator+(const Vector3& value) const
{
	return Vector3(
		x + value.x,
		y + value.y,
		z + value.z
	);
}

void Vector3::operator+=(const Vector3& value)
{
	x += value.x;
	y += value.y;
	z += value.z;
}


Vector3 Vector3::operator+(const VECTOR& value) const
{
	return Vector3(
		x + value.x,
		y + value.y,
		z + value.z
	);
}

void Vector3::operator+=(const VECTOR& value)
{
	x += value.x;
	y += value.y;
	z += value.z;
}


Vector3 Vector3::operator+(float value) const
{
	return Vector3(
		x + value,
		y + value,
		z + value
	);
}

void Vector3::operator+=(float value)
{
	x += value;
	y += value;
	z += value;
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`â¡éZ


// å∏éZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector3 Vector3::operator-(const Vector3& value) const
{
	return Vector3(
		x - value.x,
		y - value.y,
		z - value.z
	);
}

void Vector3::operator-=(const Vector3& value)
{
	x -= value.x;
	y -= value.y;
	z -= value.z;
}


Vector3 Vector3::operator-(const VECTOR& value) const
{
	return Vector3(
		x - value.x,
		y - value.y,
		z - value.z
	);
}

void Vector3::operator-=(const VECTOR& value)
{
	x -= value.x;
	y -= value.y;
	z -= value.z;
}


Vector3 Vector3::operator-(float value) const
{
	return Vector3(
		x - value,
		y - value,
		z - value
	);
}

void Vector3::operator-=(float value)
{
	x -= value;
	y -= value;
	z -= value;
}


Vector3 Vector3::operator-(void) const
{
	return Vector3(-x, -y, -z);
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`å∏éZ


// èÊéZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector3 Vector3::operator*(const Vector3& value) const
{
	return Vector3(
		x * value.x,
		y * value.y,
		z * value.z
	);
}

void Vector3::operator*=(const Vector3& value)
{
	x *= value.x;
	y *= value.y;
	z *= value.z;
}


Vector3 Vector3::operator*(const VECTOR& value) const
{
	return Vector3(
		x * value.x,
		y * value.y,
		z * value.z
	);
}

void Vector3::operator*=(const VECTOR& value)
{
	x *= value.x;
	y *= value.y;
	z *= value.z;
}


Vector3 Vector3::operator*(float value) const
{
	return Vector3(
		x * value,
		y * value,
		z * value
	);
}

void Vector3::operator*=(float value)
{
	x *= value;
	y *= value;
	z *= value;
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`èÊéZ


// èúéZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector3 Vector3::operator/(const Vector3& value) const
{
	return Vector3(
		x / (value.x != 0.0f ? value.x : 1.0f),
		y / (value.y != 0.0f ? value.y : 1.0f),
		z / (value.z != 0.0f ? value.z : 1.0f)
	);
}

void Vector3::operator/=(const Vector3& value)
{
	x /= (value.x != 0.0f ? value.x : 1.0f);
	y /= (value.y != 0.0f ? value.y : 1.0f);
	z /= (value.z != 0.0f ? value.z : 1.0f);
}


Vector3 Vector3::operator/(const VECTOR& value) const
{
	return Vector3(
		x / (value.x != 0.0f ? value.x : 1.0f),
		y / (value.y != 0.0f ? value.y : 1.0f),
		z / (value.z != 0.0f ? value.z : 1.0f)
	);
}

void Vector3::operator/=(const VECTOR& value)
{
	x /= (value.x != 0.0f ? value.x : 1.0f);
	y /= (value.y != 0.0f ? value.y : 1.0f);
	z /= (value.z != 0.0f ? value.z : 1.0f);
}


Vector3 Vector3::operator/(float value) const
{
	const float div = (value != 0.0f ? value : 1.0f);

	return Vector3(
		x / div,
		y / div,
		z / div
	);
}

void Vector3::operator/=(float value)
{
	const float div = (value != 0.0f ? value : 1.0f);

	x /= div;
	y /= div;
	z /= div;
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`èúéZ


// îªíËÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

bool Vector3::operator==(const Vector3& value) const
{
	return x == value.x &&
		y == value.y &&
		z == value.z;
}

bool Vector3::operator!=(const Vector3& value) const
{
	return !(*this == value);
}


bool Vector3::operator==(const VECTOR& value) const
{
	return x == value.x &&
		y == value.y &&
		z == value.z;
}

bool Vector3::operator!=(const VECTOR& value) const
{
	return !(*this == value);
}


bool Vector3::operator==(float value) const
{
	return x == value &&
		y == value &&
		z == value;
}

bool Vector3::operator!=(float value) const
{
	return !(*this == value);
}


bool Vector3::operator<(const Vector3& value) const
{
	return x < value.x &&
		y < value.y &&
		z < value.z;
}

bool Vector3::operator<(const VECTOR& value) const
{
	return x < value.x &&
		y < value.y &&
		z < value.z;
}

bool Vector3::operator<(float value) const
{
	return x < value &&
		y < value &&
		z < value;
}


bool Vector3::operator<=(const Vector3& value) const
{
	return x <= value.x &&
		y <= value.y &&
		z <= value.z;
}

bool Vector3::operator<=(const VECTOR& value) const
{
	return x <= value.x &&
		y <= value.y &&
		z <= value.z;
}

bool Vector3::operator<=(float value) const
{
	return x <= value &&
		y <= value &&
		z <= value;
}


bool Vector3::operator>(const Vector3& value) const
{
	return x > value.x &&
		y > value.y &&
		z > value.z;
}

bool Vector3::operator>(const VECTOR& value) const
{
	return x > value.x &&
		y > value.y &&
		z > value.z;
}

bool Vector3::operator>(float value) const
{
	return x > value &&
		y > value &&
		z > value;
}


bool Vector3::operator>=(const Vector3& value) const
{
	return x >= value.x &&
		y >= value.y &&
		z >= value.z;
}

bool Vector3::operator>=(const VECTOR& value) const
{
	return x >= value.x &&
		y >= value.y &&
		z >= value.z;
}

bool Vector3::operator>=(float value) const
{
	return x >= value &&
		y >= value &&
		z >= value;
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`îªíË

#pragma endregion


#pragma region å^ïœä∑

VECTOR Vector3::ToVECTOR(void) const
{
	return VGet(x, y, z);
}

Vector2 Vector3::ToVector2XY(void) const
{
	return Vector2(x, y);
}

Vector2 Vector3::ToVector2YX(void) const
{
	return Vector2(y, x);
}

Vector2 Vector3::ToVector2XZ(void) const
{
	return Vector2(x, z);
}

Vector2 Vector3::ToVector2ZX(void) const
{
	return Vector2(z, x);
}

Vector2 Vector3::ToVector2YZ(void) const
{
	return Vector2(y, z);
}

Vector2 Vector3::ToVector2ZY(void) const
{
	return Vector2(z, y);
}

#pragma endregion


#pragma region ÉxÉNÉgÉãââéZ

float Vector3::Dot(const Vector3& value) const
{
	return x * value.x +
		y * value.y +
		z * value.z;
}

Vector3 Vector3::Cross(const Vector3& value) const
{
	return Vector3(
		y * value.z - z * value.y,
		z * value.x - x * value.z,
		x * value.y - y * value.x
	);
}

float Vector3::LengthSq(void) const
{
	return x * x + y * y + z * z;
}

float Vector3::Length(void) const
{
	return std::sqrt(LengthSq());
}

Vector3 Vector3::Normalized(void) const
{
	const float length = Length();

	if (length == 0.0f)
	{
		return Vector3();
	}

	return *this / length;
}

void Vector3::Normalize(void)
{
	const float length = Length();

	if (length == 0.0f)
	{
		return;
	}

	*this /= length;
}

#pragma endregion


#pragma region çsóÒïœä∑

Vector3 Vector3::TransMat(const MATRIX& mat) const
{
	if (*this == 0.0f)
	{
		return Vector3();
	}

	return Vector3(VTransform(ToVECTOR(), mat));
}

void Vector3::TransMatOwn(const MATRIX& mat)
{
	if (*this == 0.0f)
	{
		return;
	}

	*this = VTransform(ToVECTOR(), mat);
}

#pragma endregion


#pragma region ÇªÇÃëº

Vector3 Vector3::Abs(void) const
{
	return Vector3(
		std::abs(x),
		std::abs(y),
		std::abs(z)
	);
}

float Vector3::MaxElementF(void) const
{
	return (std::max)({ x, y, z });
}

float Vector3::MinElementF(void) const
{
	return (std::min)({ x, y, z });
}

Vector3 Vector3::MaxElement(void) const
{
	if (x >= y && x >= z)
	{
		return Vector3::Xonly(x);
	}

	if (y >= z)
	{
		return Vector3::Yonly(y);
	}

	return Vector3::Zonly(z);
}

Vector3 Vector3::MinElement(void) const
{
	if (x <= y && x <= z)
	{
		return Vector3::Xonly(x);
	}

	if (y <= z)
	{
		return Vector3::Yonly(y);
	}

	return Vector3::Zonly(z);
}

#pragma endregion