#include "Vector2.h"

#include <cmath>

#include "Vector2I.h"

#include "Vector3.h"

#pragma region ÉRÉìÉXÉgÉâÉNÉ^

Vector2::Vector2(void)
	: x(0.0f), y(0.0f)
{
}

Vector2::Vector2(float x, float y)
	: x(x), y(y)
{
}

Vector2::Vector2(float value)
	: x(value), y(value)
{
}

Vector2::Vector2(int x, int y)
	: x(static_cast<float>(x)), y(static_cast<float>(y))
{
}

Vector2::Vector2(int value)
	: x(static_cast<float>(value)), y(static_cast<float>(value))
{
}

Vector2::Vector2(const Vector2I& value)
	: x(static_cast<float>(value.x)),
	y(static_cast<float>(value.y))
{
}

#pragma endregion


#pragma region Vector2ê∂ê¨(ì¡éÍ)

Vector2 Vector2::Xonly(float value)
{
	return Vector2(value, 0.0f);
}

Vector2 Vector2::Yonly(float value)
{
	return Vector2(0.0f, value);
}

#pragma endregion


#pragma region ââéZ

// ë„ì¸
void Vector2::operator=(const float value)
{
	x = value;
	y = value;
}

void Vector2::operator=(const Vector2I value)
{
	x = static_cast<float>(value.x);
	y = static_cast<float>(value.y);
}


// â¡éZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector2 Vector2::operator+(const Vector2 value) const
{
	return Vector2(
		x + value.x,
		y + value.y
	);
}

void Vector2::operator+=(const Vector2 value)
{
	x += value.x;
	y += value.y;
}


Vector2 Vector2::operator+(float value) const
{
	return Vector2(
		x + value,
		y + value
	);
}

void Vector2::operator+=(float value)
{
	x += value;
	y += value;
}


Vector2 Vector2::operator+(int value) const
{
	return Vector2(
		x + static_cast<float>(value),
		y + static_cast<float>(value)
	);
}

void Vector2::operator+=(int value)
{
	x += static_cast<float>(value);
	y += static_cast<float>(value);
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`â¡éZ


// å∏éZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector2 Vector2::operator-(const Vector2 value) const
{
	return Vector2(
		x - value.x,
		y - value.y
	);
}

void Vector2::operator-=(const Vector2 value)
{
	x -= value.x;
	y -= value.y;
}


Vector2 Vector2::operator-(float value) const
{
	return Vector2(
		x - value,
		y - value
	);
}

void Vector2::operator-=(float value)
{
	x -= value;
	y -= value;
}


Vector2 Vector2::operator-(int value) const
{
	return Vector2(
		x - static_cast<float>(value),
		y - static_cast<float>(value)
	);
}

void Vector2::operator-=(int value)
{
	x -= static_cast<float>(value);
	y -= static_cast<float>(value);
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`å∏éZ


// èÊéZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector2 Vector2::operator*(const Vector2 value) const
{
	return Vector2(
		x * value.x,
		y * value.y
	);
}

void Vector2::operator*=(const Vector2 value)
{
	x *= value.x;
	y *= value.y;
}


Vector2 Vector2::operator*(float value) const
{
	return Vector2(
		x * value,
		y * value
	);
}

void Vector2::operator*=(float value)
{
	x *= value;
	y *= value;
}


Vector2 Vector2::operator*(int value) const
{
	return Vector2(
		x * static_cast<float>(value),
		y * static_cast<float>(value)
	);
}

void Vector2::operator*=(int value)
{
	x *= static_cast<float>(value);
	y *= static_cast<float>(value);
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`èÊéZ


// èúéZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector2 Vector2::operator/(const Vector2 value) const
{
	return Vector2(
		x / value.x,
		y / value.y
	);
}

void Vector2::operator/=(const Vector2 value)
{
	x /= value.x;
	y /= value.y;
}


Vector2 Vector2::operator/(float value) const
{
	return Vector2(
		x / value,
		y / value
	);
}

void Vector2::operator/=(float value)
{
	x /= value;
	y /= value;
}


Vector2 Vector2::operator/(int value) const
{
	return Vector2(
		x / static_cast<float>(value),
		y / static_cast<float>(value)
	);
}

void Vector2::operator/=(int value)
{
	x /= static_cast<float>(value);
	y /= static_cast<float>(value);
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`èúéZ


// îªíËÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

bool Vector2::operator==(const Vector2 value) const
{
	return x == value.x && y == value.y;
}

bool Vector2::operator!=(const Vector2 value) const
{
	return x != value.x || y != value.y;
}


bool Vector2::operator==(const float value) const
{
	return x == value && y == value;
}

bool Vector2::operator!=(const float value) const
{
	return x != value || y != value;
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`îªíË

#pragma endregion


#pragma region å^ïœä∑

Vector2I Vector2::ToVector2I(void) const
{
	return Vector2I(
		static_cast<int>(std::round(x)),
		static_cast<int>(std::round(y))
	);
}

Vector3 Vector2::ToVector3XZ(void) const
{
	return Vector3(
		x,
		0.0f,
		y
	);
}

Vector3 Vector2::ToVector3XZInvertY(void) const
{
	return Vector3(
		x,
		0.0f,
		-y
	);
}

Vector3 Vector2::ToVector3YX(void) const
{
	return Vector3(
		y,
		x,
		0.0f
	);
}

#pragma endregion


float Vector2::LengthSq(void) const
{
	return x * x + y * y;
}

float Vector2::Length(void) const
{
	return std::sqrt(LengthSq());
}


Vector2 Vector2::Normalized(void) const
{
	const float length = Length();

	if (length == 0.0f)
	{
		return Vector2(0.0f, 0.0f);
	}

	return Vector2(
		x / length,
		y / length
	);
}

void Vector2::Normalize(void)
{
	const float length = Length();

	if (length == 0.0f)
	{
		x = 0.0f;
		y = 0.0f;
		return;
	}

	x /= length;
	y /= length;
}


Vector2 Vector2::TransMat(float rot) const
{
	const float cosRot = std::cos(rot);
	const float sinRot = std::sin(rot);

	return Vector2(
		x * cosRot - y * sinRot,
		x * sinRot + y * cosRot
	);
}

void Vector2::TransMatOwn(float rot)
{
	const float cosRot = std::cos(rot);
	const float sinRot = std::sin(rot);

	const float oldX = x;
	const float oldY = y;

	x = oldX * cosRot - oldY * sinRot;
	y = oldX * sinRot + oldY * cosRot;
}