#pragma once

struct Vector2;
struct Vector3;

struct Vector2I
{
	int x, y;

#pragma region ƒRƒ“ƒXƒgƒ‰ƒNƒ^
	// Vector2I¶¬
	Vector2I(void);

	// Vector2I¶¬
	Vector2I(int x, int y);

	// Vector2I¶¬
	Vector2I(int value);

	// Vector2I¶¬
	Vector2I(float x, float y);

	// Vector2I¶¬
	Vector2I(float value);

	// Vector2I¶¬
	Vector2I(const Vector2& value);
#pragma endregion


#pragma region Vector2I¶¬(“Áê)
	// X‚Ì‚İ
	static Vector2I Xonly(int value);

	// Y‚Ì‚İ
	static Vector2I Yonly(int value);
#pragma endregion


#pragma region ‰‰Z

	// ‘ã“ü
	void operator=(const float value);

	// ‰ÁZ`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector2I)Œ^‚Æ‚Ì‰ÁZ
	Vector2I operator+(const Vector2I value)const;

	// “¯ˆê\‘¢‘Ì(Vector2I)Œ^‚Æ‚Ì‰ÁZi’¼Ú‘ã“üj
	void operator+=(const Vector2I value);


	// ®”(int)Œ^‚Æ‚Ì‰ÁZi‘S—v‘f‚Ö‰ÁZj
	Vector2I operator+(int value)const;

	// ®”(int)Œ^‚Æ‚Ì‰ÁZi‘S—v‘f‚Ö‰ÁZji’¼Ú‘ã“üj
	void operator+=(int value);


	// •‚“®¬”(float)Œ^‚Æ‚Ì‰ÁZi‘S—v‘f‚Ö‰ÁZj
	Vector2I operator+(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚Ì‰ÁZi‘S—v‘f‚Ö‰ÁZji’¼Ú‘ã“üj
	void operator+=(float value);

	// `````````````````````````‰ÁZ


	// Œ¸Z`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector2I)Œ^‚Æ‚ÌŒ¸Z
	Vector2I operator-(const Vector2I value)const;

	// “¯ˆê\‘¢‘Ì(Vector2I)Œ^‚Æ‚ÌŒ¸Zi’¼Ú‘ã“üj
	void operator-=(const Vector2I value);


	// ®”(int)Œ^‚Æ‚ÌŒ¸Zi‘S—v‘f‚ÖŒ¸Zj
	Vector2I operator-(int value)const;

	// ®”(int)Œ^‚Æ‚ÌŒ¸Zi‘S—v‘f‚ÖŒ¸Zji’¼Ú‘ã“üj
	void operator-=(int value);


	// •‚“®¬”(float)Œ^‚Æ‚ÌŒ¸Zi‘S—v‘f‚ÖŒ¸Zj
	Vector2I operator-(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌŒ¸Zi‘S—v‘f‚ÖŒ¸Zji’¼Ú‘ã“üj
	void operator-=(float value);

	// `````````````````````````Œ¸Z


	// æZ`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector2I)Œ^‚Æ‚ÌæZ
	Vector2I operator*(const Vector2I value)const;

	// “¯ˆê\‘¢‘Ì(Vector2I)Œ^‚Æ‚ÌæZi’¼Ú‘ã“üj
	void operator*=(const Vector2I value);


	// ®”(int)Œ^‚Æ‚ÌæZi‘S—v‘f‚ÖæZj
	Vector2I operator*(int value)const;

	// ®”(int)Œ^‚Æ‚ÌæZi‘S—v‘f‚ÖæZji’¼Ú‘ã“üj
	void operator*=(int value);


	// •‚“®¬”(float)Œ^‚Æ‚ÌæZi‘S—v‘f‚ÖæZj
	Vector2I operator*(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌæZi‘S—v‘f‚ÖæZji’¼Ú‘ã“üj
	void operator*=(float value);

	// `````````````````````````æZ


	// œZ`````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector2I)Œ^‚Æ‚ÌœZ
	Vector2I operator/(const Vector2I value)const;

	// “¯ˆê\‘¢‘Ì(Vector2I)Œ^‚Æ‚ÌœZi’¼Ú‘ã“üj
	void operator/=(const Vector2I value);


	// ®”(int)Œ^‚Æ‚ÌœZi‘S—v‘f‚ÖœZj
	Vector2I operator/(int value)const;

	// ®”(int)Œ^‚Æ‚ÌœZi‘S—v‘f‚ÖœZji’¼Ú‘ã“üj
	void operator/=(int value);


	// •‚“®¬”(float)Œ^‚Æ‚ÌœZi‘S—v‘f‚ÖœZj
	Vector2I operator/(float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚ÌœZi‘S—v‘f‚ÖœZji’¼Ú‘ã“üj
	void operator/=(float value);

	// `````````````````````````œZ


	// ”»’è``````````````````````````````````````````````````

	// “¯ˆê\‘¢‘Ì(Vector2I)Œ^‚Æ‚Ì”»’èiŠe—v‘f“¯m‚ªŠ®‘Sˆê’v =utruevA‚»‚êˆÈŠO =ufalsevj
	bool operator==(const Vector2I value)const;

	// “¯ˆê\‘¢‘Ì(Vector2I)Œ^‚Æ‚Ì‹t”»’èiŠe—v‘f“¯m‚ªŠ®‘Sˆê’v =ufalsevA‚»‚êˆÈŠO =utruevj
	bool operator!=(const Vector2I value)const;


	// ®”(int)Œ^‚Æ‚Ì”»’èiw’è”’l(value)‚ÆŠe—v‘f‚·‚×‚Ä‚ªŠ®‘Sˆê’v =utruevA‚»‚êˆÈŠO =ufalsevj
	bool operator==(const int value)const;

	// ®”(int)Œ^‚Æ‚Ì‹t”»’èiw’è”’l(value)‚ÆŠe—v‘f‚·‚×‚Ä‚ªŠ®‘Sˆê’v =ufalsevA‚»‚êˆÈŠO =utruevj
	bool operator!=(const int value)const;


	// •‚“®¬”(float)Œ^‚Æ‚Ì”»’èiw’è”’l(value)‚ÆŠe—v‘f‚·‚×‚Ä‚ªŠ®‘Sˆê’v =utruevA‚»‚êˆÈŠO =ufalsevj
	bool operator==(const float value)const;

	// •‚“®¬”(float)Œ^‚Æ‚Ì‹t”»’èiw’è”’l(value)‚ÆŠe—v‘f‚·‚×‚Ä‚ªŠ®‘Sˆê’v =ufalsevA‚»‚êˆÈŠO =utruevj
	bool operator!=(const float value)const;

	// ``````````````````````````````````````````````````”»’è

#pragma endregion

#pragma region Œ^•ÏŠ·

	// Vector2Œ^iVector2I‚Ì•‚“®¬”Œ^”Åj‚Ö‚Ì•ÏŠ·iŠe—v‘f‚ğƒLƒƒƒXƒg‚µ‚Ä•ÏŠ·j
	Vector2 ToVector2(void)const;

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
};