#pragma once

struct Vector2I;
struct Vector3;

struct Vector2
{
	float x, y;

#pragma region ƒRƒ“ƒXƒgƒ‰ƒNƒ^
	// Vector2¶¬
	Vector2(void);

	// Vector2¶¬
	Vector2(float x, float y);

	// Vector2¶¬
	Vector2(float value);

	// Vector2¶¬
	Vector2(int x, int y);

	// Vector2¶¬
	Vector2(int value);

	Vector2(const Vector2I& value);
#pragma endregion


#pragma region Vector2¶¬(“Áê)
	// X‚Ì‚İ
	static Vector2 Xonly(float value);

	// Y‚Ì‚İ
	static Vector2 Yonly(float value);
#pragma endregion


#pragma region ‰‰Z

	// ‘ã“ü
	void operator=(float value);
	void operator=(const Vector2I value);

	// ‰ÁZ`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector2)Œ^‚Æ‚Ì‰ÁZ
	Vector2 operator+(const Vector2 value)const;
	// “¯ˆê\‘¢‘Ì(Vector2)Œ^‚Æ‚Ì‰ÁZi’¼Ú‘ã“üj
	void operator+=(const Vector2 value);

	// •‚“®¬”(float)Œ^‚Æ‚Ì‰ÁZi‘S—v‘f‚Ö‰ÁZj
	Vector2 operator+(float value)const;
	// •‚“®¬”(float)Œ^‚Æ‚Ì‰ÁZi‘S—v‘f‚Ö‰ÁZji’¼Ú‘ã“üj
	void operator+=(float value);

	// ®”(int)Œ^‚Æ‚Ì‰ÁZi‘S—v‘f‚Ö‰ÁZj
	Vector2 operator+(int value)const;
	// ®”(int)Œ^‚Æ‚Ì‰ÁZi‘S—v‘f‚Ö‰ÁZji’¼Ú‘ã“üj
	void operator+=(int value);

	// `````````````````````````‰ÁZ


	// Œ¸Z`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector2)Œ^‚Æ‚ÌŒ¸Z
	Vector2 operator-(const Vector2 value)const;

	// “¯ˆê\‘¢‘Ì(Vector2)Œ^‚Æ‚ÌŒ¸Zi’¼Ú‘ã“üj
	void operator-=(const Vector2 value);


	// •‚“®¬”(float)Œ^‚Æ‚ÌŒ¸Zi‘S—v‘f‚ÖŒ¸Zj
	Vector2 operator-(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌŒ¸Zi‘S—v‘f‚ÖŒ¸Zji’¼Ú‘ã“üj
	void operator-=(float value);


	// ®”(int)Œ^‚Æ‚ÌŒ¸Zi‘S—v‘f‚ÖŒ¸Zj
	Vector2 operator-(int value)const;

	// ®”(int)Œ^‚Æ‚ÌŒ¸Zi‘S—v‘f‚ÖŒ¸Zji’¼Ú‘ã“üj
	void operator-=(int value);

	// `````````````````````````Œ¸Z


	// æZ`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector2)Œ^‚Æ‚ÌæZ
	Vector2 operator*(const Vector2 value)const;

	// “¯ˆê\‘¢‘Ì(Vector2)Œ^‚Æ‚ÌæZi’¼Ú‘ã“üj
	void operator*=(const Vector2 value);


	// •‚“®¬”(float)Œ^‚Æ‚ÌæZi‘S—v‘f‚ÖæZj
	Vector2 operator*(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌæZi‘S—v‘f‚ÖæZji’¼Ú‘ã“üj
	void operator*=(float value);


	// ®”(int)Œ^‚Æ‚ÌæZi‘S—v‘f‚ÖæZj
	Vector2 operator*(int value)const;

	// ®”(int)Œ^‚Æ‚ÌæZi‘S—v‘f‚ÖæZji’¼Ú‘ã“üj
	void operator*=(int value);

	// `````````````````````````æZ


	// œZ`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector2)Œ^‚Æ‚ÌœZ
	Vector2 operator/(const Vector2 value)const;

	// “¯ˆê\‘¢‘Ì(Vector2)Œ^‚Æ‚ÌœZi’¼Ú‘ã“üj
	void operator/=(const Vector2 value);


	// •‚“®¬”(float)Œ^‚Æ‚ÌœZi‘S—v‘f‚ÖœZj
	Vector2 operator/(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌœZi‘S—v‘f‚ÖœZji’¼Ú‘ã“üj
	void operator/=(float value);


	// ®”(int)Œ^‚Æ‚ÌœZi‘S—v‘f‚ÖœZj
	Vector2 operator/(int value)const;

	// ®”(int)Œ^‚Æ‚ÌœZi‘S—v‘f‚ÖœZji’¼Ú‘ã“üj
	void operator/=(int value);

	// `````````````````````````œZ


	// ”»’è``````````````````````````````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector2)Œ^‚Æ‚Ì”»’èiŠe—v‘f“¯m‚ªŠ®‘Sˆê’v =utruevA‚»‚êˆÈŠO =ufalsevj
	bool operator==(const Vector2 value)const;

	// “¯ˆê\‘¢‘Ì(Vector2)Œ^‚Æ‚Ì‹t”»’èiŠe—v‘f“¯m‚ªŠ®‘Sˆê’v =ufalsevA‚»‚êˆÈŠO =utruevj
	bool operator!=(const Vector2 value)const;


	// •‚“®¬”(float)Œ^‚Æ‚Ì”»’èiw’è”’l(value)‚ÆŠe—v‘f‚·‚×‚Ä‚ªŠ®‘Sˆê’v =utruevA‚»‚êˆÈŠO =ufalsevj
	bool operator==(const float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚Ì‹t”»’èiw’è”’l(value)‚ÆŠe—v‘f‚·‚×‚Ä‚ªŠ®‘Sˆê’v =ufalsevA‚»‚êˆÈŠO =utruevj
	bool operator!=(const float value)const;

	// ``````````````````````````````````````````````````”»’è

#pragma endregion

#pragma region Œ^•ÏŠ·

	// Vector2IŒ^iVector2‚Ì®”Œ^”Åj‚Ö‚Ì•ÏŠ·iŠe—v‘f‚ğlÌŒÜ“ü‚µ‚½‚Ì‚¿ƒLƒƒƒXƒg‚µ‚Ä•ÏŠ·j
	Vector2I ToVector2I(void)const;

	// Vector3Œ^‚Ö‚Ì•ÏŠ·iuxv->uxvAuyv->uzv‚Ì’Ê‚è‚Å‘ã“ü‚µ•ÏŠ·j
	Vector3 ToVector3XZ(void)const;

	// Vector3Œ^‚Ö‚Ì•ÏŠ·iuxv->uxvAu-yv->uzv‚Ì’Ê‚è‚Å‘ã“ü‚µ•ÏŠ·j
	Vector3 ToVector3XZInvertY(void)const;

	// Vector3Œ^‚Ö‚Ì•ÏŠ·iuxv->uyvAuyv->uxv‚Ì’Ê‚è‚Å‘ã“ü‚µ•ÏŠ·j
	Vector3 ToVector3YX(void)const;

#pragma endregion

	// ƒxƒNƒgƒ‹’·‚Ì2æ
	float LengthSq(void)const;
	// ƒxƒNƒgƒ‹’·
	float Length(void)const;

	// ³‹K‰»i–ß‚è’lj
	Vector2 Normalized(void)const;
	// ³‹K‰»i©g‚ğj
	void Normalize(void);

	// w’è‚ÌŠp“x‚Å‰ñ“]iw’èŠp“x‚Íƒ‰ƒWƒAƒ“ji–ß‚è’lj
	Vector2 TransMat(float rot) const;
	// w’è‚ÌŠp“x‚Å‰ñ“]iw’èŠp“x‚Íƒ‰ƒWƒAƒ“ji©g‚ğj
	void TransMatOwn(float rot);
};