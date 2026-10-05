#include "Vector2I.h"

#include <cmath>

#include "Vector2.h"

#include "Vector3.h"

#pragma region ÉRÉìÉXÉgÉâÉNÉ^

Vector2I::Vector2I(void)
	: x(0), y(0)
{
}

Vector2I::Vector2I(int x, int y)
	: x(x), y(y)
{
}

Vector2I::Vector2I(int value)
	: x(value), y(value)
{
}

Vector2I::Vector2I(float x, float y)
	: x(static_cast<int>(x)), y(static_cast<int>(y))
{
}

Vector2I::Vector2I(float value)
	: x(static_cast<int>(value)), y(static_cast<int>(value))
{
}

Vector2I::Vector2I(const Vector2& value)
	: x(static_cast<int>(value.x)), y(static_cast<int>(value.y))
{
}

#pragma endregion


#pragma region Vector2Iê∂ê¨(ì¡éÍ)

Vector2I Vector2I::Xonly(int value)
{
	return Vector2I(value, 0);
}

Vector2I Vector2I::Yonly(int value)
{
	return Vector2I(0, value);
}

#pragma endregion


#pragma region ââéZ

// ë„ì¸
void Vector2I::operator=(const float value)
{
	x = static_cast<int>(value);
	y = static_cast<int>(value);
}


// â¡éZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector2I Vector2I::operator+(const Vector2I value) const
{
	return Vector2I(x + value.x, y + value.y);
}

void Vector2I::operator+=(const Vector2I value)
{
	x += value.x;
	y += value.y;
}


Vector2I Vector2I::operator+(int value) const
{
	return Vector2I(x + value, y + value);
}

void Vector2I::operator+=(int value)
{
	x += value;
	y += value;
}


Vector2I Vector2I::operator+(float value) const
{
	return Vector2I(
		static_cast<int>(x + value),
		static_cast<int>(y + value)
	);
}

void Vector2I::operator+=(float value)
{
	x = static_cast<int>(x + value);
	y = static_cast<int>(y + value);
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`â¡éZ


// å∏éZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector2I Vector2I::operator-(const Vector2I value) const
{
	return Vector2I(x - value.x, y - value.y);
}

void Vector2I::operator-=(const Vector2I value)
{
	x -= value.x;
	y -= value.y;
}


Vector2I Vector2I::operator-(int value) const
{
	return Vector2I(x - value, y - value);
}

void Vector2I::operator-=(int value)
{
	x -= value;
	y -= value;
}


Vector2I Vector2I::operator-(float value) const
{
	return Vector2I(
		static_cast<int>(x - value),
		static_cast<int>(y - value)
	);
}

void Vector2I::operator-=(float value)
{
	x = static_cast<int>(x - value);
	y = static_cast<int>(y - value);
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`å∏éZ


// èÊéZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector2I Vector2I::operator*(const Vector2I value) const
{
	return Vector2I(x * value.x, y * value.y);
}

void Vector2I::operator*=(const Vector2I value)
{
	x *= value.x;
	y *= value.y;
}


Vector2I Vector2I::operator*(int value) const
{
	return Vector2I(x * value, y * value);
}

void Vector2I::operator*=(int value)
{
	x *= value;
	y *= value;
}


Vector2I Vector2I::operator*(float value) const
{
	return Vector2I(
		static_cast<int>(x * value),
		static_cast<int>(y * value)
	);
}

void Vector2I::operator*=(float value)
{
	x = static_cast<int>(x * value);
	y = static_cast<int>(y * value);
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`èÊéZ


// èúéZÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

Vector2I Vector2I::operator/(const Vector2I value) const
{
	return Vector2I(x / value.x, y / value.y);
}

void Vector2I::operator/=(const Vector2I value)
{
	x /= value.x;
	y /= value.y;
}


Vector2I Vector2I::operator/(int value) const
{
	return Vector2I(x / value, y / value);
}

void Vector2I::operator/=(int value)
{
	x /= value;
	y /= value;
}


Vector2I Vector2I::operator/(float value) const
{
	return Vector2I(
		static_cast<int>(x / value),
		static_cast<int>(y / value)
	);
}

void Vector2I::operator/=(float value)
{
	x = static_cast<int>(x / value);
	y = static_cast<int>(y / value);
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`èúéZ


// îªíËÅ`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`

bool Vector2I::operator==(const Vector2I value) const
{
	return x == value.x && y == value.y;
}

bool Vector2I::operator!=(const Vector2I value) const
{
	return x != value.x || y != value.y;
}

bool Vector2I::operator==(const int value) const
{
	return x == value && y == value;
}

bool Vector2I::operator!=(const int value) const
{
	return x != value || y != value;
}


bool Vector2I::operator==(const float value) const
{
	return static_cast<float>(x) == value && static_cast<float>(y) == value;
}

bool Vector2I::operator!=(const float value) const
{
	return static_cast<float>(x) != value || static_cast<float>(y) != value;
}

// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`îªíË

#pragma endregion


#pragma region å^ïœä∑

Vector2 Vector2I::ToVector2(void) const
{
	return Vector2(
		static_cast<float>(x),
		static_cast<float>(y)
	);
}

Vector3 Vector2I::ToVector3XZ(void) const
{
	return Vector3(
		static_cast<float>(x),
		0.0f,
		static_cast<float>(y)
	);
}

Vector3 Vector2I::ToVector3XZInvertY(void) const
{
	return Vector3(
		static_cast<float>(x),
		0.0f,
		static_cast<float>(-y)
	);
}

Vector3 Vector2I::ToVector3YX(void) const
{
	return Vector3(
		static_cast<float>(y),
		static_cast<float>(x),
		0.0f
	);
}

#pragma endregion


float Vector2I::LengthSq(void) const
{
	return static_cast<float>(x * x + y * y);
}

float Vector2I::Length(void) const
{
	return std::sqrt(LengthSq());
}

Vector2 Vector2I::Normalized(void) const
{
	const float length = Length();

	if (length == 0.0f)
	{
		return Vector2(0.0f, 0.0f);
	}

	return Vector2(
		static_cast<float>(x) / length,
		static_cast<float>(y) / length
	);
}