#pragma once

#include "../pch.h"

struct Vector2;

struct Vector3
{
	float x, y, z;

#pragma region ƒRƒ“ƒXƒgƒ‰ƒNƒ^

	// Vector3¶¬
	Vector3(void);

	// Vector3¶¬
	Vector3(float x, float y, float z);

	// Vector3¶¬
	Vector3(float value);

	// Vector3¶¬
	Vector3(int x, int y, int z);

	// Vector3¶¬
	Vector3(int value);

	// Vector3¶¬
	Vector3(const VECTOR& value);

#pragma endregion


#pragma region Vector3¶¬(“Áê)

	// X‚Ì‚İ
	static Vector3 Xonly(float value);

	// Y‚Ì‚İ
	static Vector3 Yonly(float value);

	// Z‚Ì‚İ
	static Vector3 Zonly(float value);

	// XAY‚Ì‚İ
	static Vector3 XYonly(float x, float y);

	// XAZ‚Ì‚İ
	static Vector3 XZonly(float x, float z);

	// YAZ‚Ì‚İ
	static Vector3 YZonly(float y, float z);

#pragma endregion


#pragma region ‰‰Z

	// ‘ã“ü
	void operator=(const float value);

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Ì‘ã“ü
	void operator=(const Vector3& value);

	// DxLib‚ÌVECTORŒ^‚Ì‘ã“ü
	void operator=(const VECTOR& value);


	// ‰ÁZ`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚Ì‰ÁZ
	Vector3 operator+(const Vector3& value)const;

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚Ì‰ÁZi’¼Ú‘ã“üj
	void operator+=(const Vector3& value);


	// DxLib‚ÌVECTORŒ^‚Æ‚Ì‰ÁZ
	Vector3 operator+(const VECTOR& value)const;

	// DxLib‚ÌVECTORŒ^‚Æ‚Ì‰ÁZi’¼Ú‘ã“üj
	void operator+=(const VECTOR& value);


	// •‚“®¬”(float)Œ^‚Æ‚Ì‰ÁZi‘S—v‘f‚Ö‰ÁZj
	Vector3 operator+(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚Ì‰ÁZi‘S—v‘f‚Ö‰ÁZji’¼Ú‘ã“üj
	void operator+=(float value);

	// `````````````````````````‰ÁZ


	// Œ¸Z`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚ÌŒ¸Z
	Vector3 operator-(const Vector3& value)const;

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚ÌŒ¸Zi’¼Ú‘ã“üj
	void operator-=(const Vector3& value);


	// DxLib‚ÌVECTORŒ^‚Æ‚ÌŒ¸Z
	Vector3 operator-(const VECTOR& value)const;

	// DxLib‚ÌVECTORŒ^‚Æ‚ÌŒ¸Zi’¼Ú‘ã“üj
	void operator-=(const VECTOR& value);


	// •‚“®¬”(float)Œ^‚Æ‚ÌŒ¸Zi‘S—v‘f‚ÖŒ¸Zj
	Vector3 operator-(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌŒ¸Zi‘S—v‘f‚ÖŒ¸Zji’¼Ú‘ã“üj
	void operator-=(float value);


	// •„†”½“]
	Vector3 operator-(void)const;

	// `````````````````````````Œ¸Z


	// æZ`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚ÌæZ
	Vector3 operator*(const Vector3& value)const;

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚ÌæZi’¼Ú‘ã“üj
	void operator*=(const Vector3& value);


	// DxLib‚ÌVECTORŒ^‚Æ‚ÌæZ
	Vector3 operator*(const VECTOR& value)const;

	// DxLib‚ÌVECTORŒ^‚Æ‚ÌæZi’¼Ú‘ã“üj
	void operator*=(const VECTOR& value);


	// •‚“®¬”(float)Œ^‚Æ‚ÌæZi‘S—v‘f‚ÖæZj
	Vector3 operator*(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌæZi‘S—v‘f‚ÖæZji’¼Ú‘ã“üj
	void operator*=(float value);

	// `````````````````````````æZ


	// œZ`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚ÌœZ
	Vector3 operator/(const Vector3& value)const;

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚ÌœZi’¼Ú‘ã“üj
	void operator/=(const Vector3& value);


	// DxLib‚ÌVECTORŒ^‚Æ‚ÌœZ
	Vector3 operator/(const VECTOR& value)const;

	// DxLib‚ÌVECTORŒ^‚Æ‚ÌœZi’¼Ú‘ã“üj
	void operator/=(const VECTOR& value);


	// •‚“®¬”(float)Œ^‚Æ‚ÌœZi‘S—v‘f‚ÖœZj
	Vector3 operator/(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌœZi‘S—v‘f‚ÖœZji’¼Ú‘ã“üj
	void operator/=(float value);

	// `````````````````````````œZ


	// ”»’è``````````````````````````````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚Ì”»’èiŠe—v‘f“¯m‚ªŠ®‘Sˆê’v =utruevA‚»‚êˆÈŠO =ufalsevj
	bool operator==(const Vector3& value)const;

	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚Ì‹t”»’èiŠe—v‘f“¯m‚ªŠ®‘Sˆê’v =ufalsevA‚»‚êˆÈŠO =utruevj
	bool operator!=(const Vector3& value)const;


	// DxLib‚ÌVECTORŒ^‚Æ‚Ì”»’è
	bool operator==(const VECTOR& value)const;

	// DxLib‚ÌVECTORŒ^‚Æ‚Ì‹t”»’è
	bool operator!=(const VECTOR& value)const;


	// •‚“®¬”(float)Œ^‚Æ‚Ì”»’èiw’è”’l(value)‚ÆŠe—v‘f‚·‚×‚Ä‚ªŠ®‘Sˆê’v =utruevA‚»‚êˆÈŠO =ufalsevj
	bool operator==(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚Ì‹t”»’èiw’è”’l(value)‚ÆŠe—v‘f‚·‚×‚Ä‚ªŠ®‘Sˆê’v =ufalsevA‚»‚êˆÈŠO =utruevj
	bool operator!=(float value)const;


	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚Ì¬‚È‚è”»’èi‘S—v‘f‚ª¬‚³‚¢ =utruevj
	bool operator<(const Vector3& value)const;

	// DxLib‚ÌVECTORŒ^‚Æ‚Ì¬‚È‚è”»’èi‘S—v‘f‚ª¬‚³‚¢ =utruevj
	bool operator<(const VECTOR& value)const;

	// •‚“®¬”(float)Œ^‚Æ‚Ì¬‚È‚è”»’èi‘S—v‘f‚ªw’è”’l‚æ‚è¬‚³‚¢ =utruevj
	bool operator<(float value)const;


	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚ÌˆÈ‰º”»’èi‘S—v‘f‚ªˆÈ‰º =utruevj
	bool operator<=(const Vector3& value)const;

	// DxLib‚ÌVECTORŒ^‚Æ‚ÌˆÈ‰º”»’èi‘S—v‘f‚ªˆÈ‰º =utruevj
	bool operator<=(const VECTOR& value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌˆÈ‰º”»’èi‘S—v‘f‚ªw’è”’lˆÈ‰º =utruevj
	bool operator<=(float value)const;


	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚Ì‘å‚È‚è”»’èi‘S—v‘f‚ª‘å‚«‚¢ =utruevj
	bool operator>(const Vector3& value)const;

	// DxLib‚ÌVECTORŒ^‚Æ‚Ì‘å‚È‚è”»’èi‘S—v‘f‚ª‘å‚«‚¢ =utruevj
	bool operator>(const VECTOR& value)const;

	// •‚“®¬”(float)Œ^‚Æ‚Ì‘å‚È‚è”»’èi‘S—v‘f‚ªw’è”’l‚æ‚è‘å‚«‚¢ =utruevj
	bool operator>(float value)const;


	// “¯ˆê\‘¢‘Ì(Vector3)Œ^‚Æ‚ÌˆÈã”»’èi‘S—v‘f‚ªˆÈã =utruevj
	bool operator>=(const Vector3& value)const;

	// DxLib‚ÌVECTORŒ^‚Æ‚ÌˆÈã”»’èi‘S—v‘f‚ªˆÈã =utruevj
	bool operator>=(const VECTOR& value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌˆÈã”»’èi‘S—v‘f‚ªw’è”’lˆÈã =utruevj
	bool operator>=(float value)const;

	// ``````````````````````````````````````````````````”»’è

#pragma endregion


#pragma region Œ^•ÏŠ·

	// DxLib‚ÌVECTORŒ^‚Ö‚Ì•ÏŠ·
	VECTOR ToVECTOR(void)const;

	// Vector2Œ^‚Ö‚Ì•ÏŠ·iuxv->uxvAuyv->uyv‚Ì’Ê‚è‚Å‘ã“ü‚µ•ÏŠ·j
	Vector2 ToVector2XY(void)const;

	// Vector2Œ^‚Ö‚Ì•ÏŠ·iuyv->uxvAuxv->uyv‚Ì’Ê‚è‚Å‘ã“ü‚µ•ÏŠ·j
	Vector2 ToVector2YX(void)const;

	// Vector2Œ^‚Ö‚Ì•ÏŠ·iuxv->uxvAuzv->uyv‚Ì’Ê‚è‚Å‘ã“ü‚µ•ÏŠ·j
	Vector2 ToVector2XZ(void)const;

	// Vector2Œ^‚Ö‚Ì•ÏŠ·iuzv->uxvAuxv->uyv‚Ì’Ê‚è‚Å‘ã“ü‚µ•ÏŠ·j
	Vector2 ToVector2ZX(void)const;

	// Vector2Œ^‚Ö‚Ì•ÏŠ·iuyv->uxvAuzv->uyv‚Ì’Ê‚è‚Å‘ã“ü‚µ•ÏŠ·j
	Vector2 ToVector2YZ(void)const;

	// Vector2Œ^‚Ö‚Ì•ÏŠ·iuzv->uxvAuyv->uyv‚Ì’Ê‚è‚Å‘ã“ü‚µ•ÏŠ·j
	Vector2 ToVector2ZY(void)const;

#pragma endregion


#pragma region ƒxƒNƒgƒ‹‰‰Z

	// “àÏ
	float Dot(const Vector3& value)const;

	// ŠOÏ
	Vector3 Cross(const Vector3& value)const;

	// ƒxƒNƒgƒ‹’·‚Ì2æ
	float LengthSq(void)const;

	// ƒxƒNƒgƒ‹’·
	float Length(void)const;

	// ³‹K‰»i–ß‚è’lj
	Vector3 Normalized(void)const;

	// ³‹K‰»i©g‚ğj
	void Normalize(void);

#pragma endregion


#pragma region s—ñ•ÏŠ·

	// w’è‚Ìs—ñ‚Å•ÏŠ·i–ß‚è’lj
	Vector3 TransMat(const MATRIX& mat)const;

	// w’è‚Ìs—ñ‚Å•ÏŠ·i©g‚ğj
	void TransMatOwn(const MATRIX& mat);

#pragma endregion


#pragma region ‚»‚Ì‘¼

	// Še—v‘f‚ğâ‘Î’l‚É•ÏŠ·
	Vector3 Abs(void)const;

	// Å‘å‚Ì—v‘f‚ğæ“¾
	float MaxElementF(void)const;

	// Å¬‚Ì—v‘f‚ğæ“¾
	float MinElementF(void)const;

	// Å‘å‚Ì—v‘f‚Ì‚İ‚ğ‚ÂVector3‚ğæ“¾
	Vector3 MaxElement(void)const;

	// Å¬‚Ì—v‘f‚Ì‚İ‚ğ‚ÂVector3‚ğæ“¾
	Vector3 MinElement(void)const;

#pragma endregion
};