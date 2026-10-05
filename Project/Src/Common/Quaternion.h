#pragma once

#include "../pch.h"

struct Vector3;

/// 3次元空間上の回転を表すクォータニオン
struct Quaternion
{
	float x;
	float y;
	float z;
	float w;

#pragma region コンストラクタ

	/// <summary>
	/// 単位クォータニオンを生成する
	/// </summary>
	Quaternion(void);

	/// <summary>
	/// 各成分を指定して生成する
	/// </summary>
	Quaternion(float x, float y, float z, float w);

#pragma endregion


#pragma region 生成

	/// <summary>
	/// 単位クォータニオンを取得する
	/// </summary>
	static Quaternion Identity(void);

	/// <summary>
	/// 指定した軸を中心とした回転を生成する
	/// </summary>
	/// <param name="axis">回転軸</param>
	/// <param name="angle">回転量（ラジアン）</param>
	static Quaternion FromAxisAngle(const Vector3& axis, float angle);

	/// <summary>
	/// X軸回転を生成する
	/// </summary>
	static Quaternion FromRotationX(float angle);

	/// <summary>
	/// Y軸回転を生成する
	/// </summary>
	static Quaternion FromRotationY(float angle);

	/// <summary>
	/// Z軸回転を生成する
	/// </summary>
	static Quaternion FromRotationZ(float angle);

	/// <summary>
	/// X → Z → Y の順でEuler角を適用した回転を生成する
	/// </summary>
	static Quaternion FromEulerXZY(const Vector3& angle);

	/// <summary>
	/// 回転行列からクォータニオンを生成する
	/// </summary>
	static Quaternion FromMatrix(const MATRIX& matrix);

#pragma endregion


#pragma region 演算

	/// <summary>
	/// クォータニオン同士を乗算する
	/// </summary>
	Quaternion operator*(const Quaternion& value) const;

	/// <summary>
	/// クォータニオン同士を乗算し、結果を自身へ代入する
	/// </summary>
	void operator*=(const Quaternion& value);

	/// <summary>
	/// 同値比較
	/// </summary>
	bool operator==(const Quaternion& value) const;

	/// <summary>
	/// 非同値比較
	/// </summary>
	bool operator!=(const Quaternion& value) const;

#pragma endregion


#pragma region 基本操作

	/// <summary>
	/// 長さを取得する
	/// </summary>
	float Length(void) const;

	/// <summary>
	/// 長さの二乗を取得する
	/// </summary>
	float LengthSq(void) const;

	/// <summary>
	/// 正規化したクォータニオンを取得する
	/// </summary>
	Quaternion Normalized(void) const;

	/// <summary>
	/// 自身を正規化する
	/// </summary>
	void Normalize(void);

	/// <summary>
	/// 共役クォータニオンを取得する
	/// </summary>
	Quaternion Conjugated(void) const;

	/// <summary>
	/// 逆クォータニオンを取得する
	/// </summary>
	Quaternion Inversed(void) const;

	/// <summary>
	/// 内積を取得する
	/// </summary>
	static float Dot(const Quaternion& q1, const Quaternion& q2);

#pragma endregion


#pragma region 回転

	/// <summary>
	/// 回転行列へ変換する
	/// </summary>
	MATRIX ToMatrix(void) const;

	/// <summary>
	/// ベクトルをこのクォータニオンで回転させる
	/// </summary>
	Vector3 Rotate(const Vector3& vec) const;

#pragma endregion


#pragma region 補間

	/// <summary>
	/// 球面線形補間を行う
	/// </summary>
	static Quaternion Slerp(
		const Quaternion& start,
		const Quaternion& end,
		float rate
	);

#pragma endregion
};