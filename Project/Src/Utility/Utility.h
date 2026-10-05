#pragma once
#include "../pch.h"

#include <string>
#include <vector>

#include <fstream>
#include <sstream>


#include "../Common/Vector2.h"
#include "../Common/Vector2I.h"
#include "../Common/Vector3.h"

// ラジアン(rad)・度(deg)変換用
static constexpr float RAD2DEG = (180.0f / DX_PI_F);
static constexpr float DEG2RAD = (DX_PI_F / 180.0f);

static constexpr float kEpsilonNormalSqrt = 1e-15F;

// 四捨五入
static int Round(float v) { return static_cast<int>(roundf(v)); }

// 文字列の分割
static std::vector <std::string> Split(std::string& line, char delimiter) {
    std::istringstream stream(line);
	std::string field;
	std::vector<std::string> result;

	while (getline(stream, field, delimiter)) {
		result.push_back(field);
	}

	return result;
}

// ラジアン(rad)から度(deg)
static double Rad2Deg(double rad) { return rad * (180.0 / DX_PI); }
static float Rad2Deg(float rad) { return rad * (180.0f / DX_PI_F); }
static int Rad2Deg(int rad) { return rad * Round(180.0f / DX_PI_F); }

// 度(deg)からラジアン(rad)
static double Deg2Rad(double deg) { return deg * (DX_PI / 180.0); }
static float Deg2Rad(float deg) { return deg * (DX_PI_F / 180.0f); }
static int Deg2Rad(int deg) { return deg * Round(DX_PI_F / 180.0f); }
static VECTOR Deg2Rad(const VECTOR& deg) { return { Deg2Rad(deg.x),Deg2Rad(deg.y),Deg2Rad(deg.z) }; }
static Vector3 Deg2Rad(const Vector3& deg) { return { Deg2Rad(deg.x),Deg2Rad(deg.y),Deg2Rad(deg.z) }; }

// 0～360度の範囲に収める
static double DegIn360(double deg) {
	deg = fmod(deg, 360.0);
	if (deg < 0.0f)
	{
		deg += 360.0;
	}
	return deg;
}

// 0(0)～2π(360度)の範囲に収める
static double RadIn2PI(double rad) {
	rad = fmod(rad, DX_TWO_PI);
	if (rad < 0.0)
	{
		rad += DX_TWO_PI;
	}
	return rad;
}

// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
static int DirNearAroundRad(float from, float to) {
	float ret = 1.0f;

	float diff = to - from;

	if (diff >= 0.0f) {
		// 比較元よりも時計回りに位置する

		// でも、180度以上離れているので、反時計回りの方が近い
		if (diff > DX_PI_F) { ret = -1.0f; }
		// 時計回り
		else { ret = 1.0f; }

	}
	else {
		// 比較元よりも反時計回りに位置する

		// でも、180度以上離れているので、時計回りの方が近い
		if (diff < -DX_PI_F) { ret = 1.0f; }
		// 反時計回り
		else { ret = -1.0f; }
	}

	return static_cast<int>(ret);
}

// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
static int DirNearAroundDeg(float from, float to) {
	float ret = 1.0f;

	float diff = to - from;

	if (diff >= 0.0f) {
		// 比較元よりも時計回りに位置する

		// でも、180度以上離れているので、反時計回りの方が近い
		if (diff > 180.0f) { ret = -1.0f; }
		// 時計回り
		else { ret = 1.0f; }
	}
	else	{
		// 比較元よりも反時計回りに位置する

		// でも、180度以上離れているので、時計回りの方が近い
		if (diff < -180.0f) { ret = 1.0f; }
		// 反時計回り
		else { ret = -1.0f; }
	}

	return static_cast<int>(ret);
}

// 線形補間
static int Lerp(int start, int end, float t) {
	if (t >= 1.0f) { return end; }

	int ret = start;
	ret += Round(t * static_cast<float>(end - start));
	return ret;
}
static float Lerp(float start, float end, float t) {
	if (t >= 1.0f) { return end; }

	float ret = start;
	ret += t * (end - start);
	return ret;
}
static double Lerp(double start, double end, double t) {
	if (t >= 1.0) { return end; }

	double ret = start;
	ret += t * (end - start);
	return ret;
}
static Vector2 Lerp(const Vector2& start, const Vector2& end, float t) {
	if (t >= 1.0f) { return end; }

	Vector2 ret = start;
	ret.x += Round(t * static_cast<float>((end.x - start.x)));
	ret.y += Round(t * static_cast<float>((end.y - start.y)));
	return ret;
}

// 角度の線形補間
static double LerpDeg(double start, double end, double t) {
	double ret;

	double diff = end - start;
	if (diff < -180.0) {
		end += 360.0;
		ret = Lerp(start, end, t);
		if (ret >= 360.0) { ret -= 360.0; }
	}
	else if (diff > 180.0) {
		end -= 360.0;
		ret = Lerp(start, end, t);
		if (ret < 0.0) { ret += 360.0; }
	}
	else { ret = Lerp(start, end, t); }

	return ret;
}

// ベジェ曲線
static Vector2 Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t) {
	Vector2 a = Lerp(p1, p2, t);
	Vector2 b = Lerp(p2, p3, t);
	return Lerp(a, b, t);
}

static float Signed(float x) { return (x < 0) ? -1.0f : 1.0f; }

// 画像読み込み（エラーチェック付き）
static void LoadImg(int& handle, std::string path) {
	handle = LoadGraph(path.c_str());
	if (handle == -1) {
		printfDx("画像読み込みに失敗しました");
	}
}
// 画像読み込み（エラーチェック付き）
static int LoadImg(std::string path) {
	int ret = 0;

	ret = LoadGraph(path.c_str());
	if (ret == -1) {
		printfDx("画像読み込みに失敗しました");
	}

	return ret;
}

// スプライト画像読み込み（エラーチェック付き）
static void LoadArrayImg(std::string path, int AllNum, int XNum, int YNum, int XSize, int YSize, int* handleArray) {
	int err = 0;

	err = LoadDivGraph(path.c_str(), AllNum, XNum, YNum, XSize, YSize, handleArray);

	if (err == -1) {
		printfDx("画像読み込みに失敗しました");
	}
}

static void LoadArrayImg(std::string path, int AllNum, int XNum, int YNum, int XSize, int YSize, std::vector<int>& handleArray) {
	handleArray.resize(AllNum);

	int err = LoadDivGraph(path.c_str(), AllNum, XNum, YNum, XSize, YSize, handleArray.data());

	if (err == -1) {
		printfDx("画像読み込みに失敗しました: %s\n", path.c_str());
		return;
	}
}


#pragma region 各順番の（VECTOR or Vector3）構造体を行列に変換する関数（引数にいくつでも入れられる仕様 左から順に計算される）
    static MATRIX MatrixAllMultX(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) { m = MMult(m, MGetRotX(vec.x)); }
        return m;
    }

    static MATRIX MatrixAllMultX(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) { m = MMult(m, MGetRotX(vec.x)); }
        return m;
    }

    static MATRIX MatrixAllMultY(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) { m = MMult(m, MGetRotY(vec.y)); }
        return m;
    }

    static MATRIX MatrixAllMultY(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) { m = MMult(m, MGetRotY(vec.y)); }
        return m;
    }

    static MATRIX MatrixAllMultZ(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) { m = MMult(m, MGetRotZ(vec.z)); }
        return m;
    }

    static MATRIX MatrixAllMultZ(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) { m = MMult(m, MGetRotZ(vec.z)); }
        return m;
    }

    static MATRIX MatrixAllMultXY(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotY(vec.y));
        }
        return m;
    }

    static MATRIX MatrixAllMultXY(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotY(vec.y));
        }
        return m;
    }

    static MATRIX MatrixAllMultXZ(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotZ(vec.z));
        }
        return m;
    }

    static MATRIX MatrixAllMultXZ(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotZ(vec.z));
        }
        return m;
    }

    static MATRIX MatrixAllMultXYZ(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotZ(vec.z));
        }
        return m;
    }

    static MATRIX MatrixAllMultXYZ(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotZ(vec.z));
        }
        return m;
    }

    static MATRIX MatrixAllMultXZY(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotY(vec.y));
        }
        return m;
    }

    static MATRIX MatrixAllMultXZY(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotY(vec.y));
        }
        return m;
    }

    static MATRIX MatrixAllMultYX(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotX(vec.x));
        }
        return m;
    }

    static MATRIX MatrixAllMultYX(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotX(vec.x));
        }
        return m;
    }

    static MATRIX MatrixAllMultYZ(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotZ(vec.z));
        }
        return m;
    }

    static MATRIX MatrixAllMultYZ(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotZ(vec.z));
        }
        return m;
    }

    static MATRIX MatrixAllMultYXZ(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotZ(vec.z));
        }
        return m;
    }

    static MATRIX MatrixAllMultYXZ(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotZ(vec.z));
        }
        return m;
    }

    static MATRIX MatrixAllMultYZX(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotX(vec.x));
        }
        return m;
    }

    static MATRIX MatrixAllMultYZX(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotX(vec.x));
        }
        return m;
    }

    static MATRIX MatrixAllMultZX(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotX(vec.x));
        }
        return m;
    }

    static MATRIX MatrixAllMultZX(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotX(vec.x));
        }
        return m;
    }

    static MATRIX MatrixAllMultZY(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotY(vec.y));
        }
        return m;
    }

    static MATRIX MatrixAllMultZY(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotY(vec.y));
        }
        return m;
    }

    static MATRIX MatrixAllMultZXY(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotY(vec.y));
        }
        return m;
    }

    static MATRIX MatrixAllMultZXY(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotX(vec.x));
            m = MMult(m, MGetRotY(vec.y));
        }
        return m;
    }

    static MATRIX MatrixAllMultZYX(const std::initializer_list<VECTOR>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotX(vec.x));
        }
        return m;
    }

    static MATRIX MatrixAllMultZYX(const std::initializer_list<Vector3>& vecs) {
        MATRIX m = MGetIdent();
        for (const auto& vec : vecs) {
            m = MMult(m, MGetRotZ(vec.z));
            m = MMult(m, MGetRotY(vec.y));
            m = MMult(m, MGetRotX(vec.x));
        }
        return m;
    }
#pragma endregion

// 行列をつかったモデルの座標と向きのセット
static void MV1ModelMatrix(int& model, const VECTOR& pos, const std::initializer_list<VECTOR>& angle) {
    MATRIX m = MGetIdent();
    // 角度セットを順に合成
    for (const auto& a : angle) {
        m = MMult(m, MGetRotX(a.x));
        m = MMult(m, MGetRotY(a.y));
        m = MMult(m, MGetRotZ(a.z));
    }

    MV1SetRotationMatrix(model, m);
    MV1SetPosition(model, pos);
}
static void MV1ModelMatrix(int& model, const Vector3& pos, const std::initializer_list<Vector3>& angle) {
    MATRIX m = MGetIdent();
    // 角度セットを順に合成
    for (const auto& a : angle) {
        m = MMult(m, MGetRotX(a.x));
        m = MMult(m, MGetRotY(a.y));
        m = MMult(m, MGetRotZ(a.z));
    }

    MV1SetRotationMatrix(model, m);
    MV1SetPosition(model, pos.ToVECTOR());
}

static void MV1ModelMatrix(int& model, const VECTOR& scale, const VECTOR& pos, const std::initializer_list<VECTOR>& angle) {
    MATRIX m = MGetIdent();

    m.m[0][0] = scale.x;
    m.m[1][1] = scale.y;
    m.m[2][2] = scale.z;

    // 角度セットを順に合成
    for (const auto& a : angle) {
        m = MMult(m, MGetRotX(a.x));
        m = MMult(m, MGetRotY(a.y));
        m = MMult(m, MGetRotZ(a.z));
    }

    m.m[3][0] = pos.x;
    m.m[3][1] = pos.y;
    m.m[3][2] = pos.z;

    MV1SetMatrix(model, m);
}
static void MV1ModelMatrix(int& model, const Vector3& scale, const Vector3& pos, const std::initializer_list<Vector3>& angle) {
    MATRIX m = MGetIdent();

    m.m[0][0] = scale.x;
    m.m[1][1] = scale.y;
    m.m[2][2] = scale.z;

    // 角度セットを順に合成
    for (const auto& a : angle) {
        m = MMult(m, MGetRotX(a.x));
        m = MMult(m, MGetRotY(a.y));
        m = MMult(m, MGetRotZ(a.z));
    }

    m.m[3][0] = pos.x;
    m.m[3][1] = pos.y;
    m.m[3][2] = pos.z;

    MV1SetMatrix(model, m);
}

static Vector3 VTransform(const Vector3& v, const MATRIX& m) {
	if (v == 0.0f) { return Vector3(); }
	return Vector3(VTransform(v.ToVECTOR(), m));
}

static std::string WStringToString(const std::wstring& ws)
{
	int len = WideCharToMultiByte(932, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (len <= 0) return {};
	std::string s(len - 1, '\0');
	WideCharToMultiByte(932, 0, ws.c_str(), -1, &s[0], len, nullptr, nullptr);
	return s;
}

/// <summary>
/// 値を最小値と最大値の間で正規化する
/// </summary>
/// <param name="value">現在の値</param>
/// <param name="minValue">その値における最小値</param>
/// <param name="maxValue">その値における最大値</param>
/// <returns>現在の値がその値においてどこに位置しているか（ 割合 : 0.0f～1.0f ）</returns>
static float ValueNormalizeRatio(float value, float minValue, float maxValue) {
	// 0除算防止
	if (maxValue - minValue == 0.0f) { return 0.0f; }

	return (value - minValue) / (maxValue - minValue);
}

/// 割合を反転させる
static float RatioReverse(float ratio) { return (1.0f - ratio); }

// RGBの数値をカラーコードに変換
static unsigned int RGBToColorCode(unsigned char r, unsigned char g, unsigned char b) { return ((r << 16) | (g << 8) | b); }

// MagicaVoxelによって作成したCSVファイルの1行の情報の番号
enum CSV_ADDRESS { X, Z, Y, R, G, B, };

// MagicaVoxelによって作成したCSVファイルの1行から情報を抜き出す構造体
struct MagicaVoxelCSVRow {

	int x = -1, y = -1, z = -1;
	int number = 0;

	MagicaVoxelCSVRow(std::vector<std::string> line) {
		// XYZ
		x = std::stoi(line.at(CSV_ADDRESS::X));
		y = std::stoi(line.at(CSV_ADDRESS::Y));
		z = std::stoi(line.at(CSV_ADDRESS::Z));

		// RGBをカラーコードに変換してそれを番号として使う
		number = RGBToColorCode(
			std::stoi(line.at(CSV_ADDRESS::R)),
			std::stoi(line.at(CSV_ADDRESS::G)),
			std::stoi(line.at(CSV_ADDRESS::B))
		);
	}
};

/// <summary>
/// 配列全ての要素を変換する関数（例：std::vector<int>からstd::vector<float>へ）
/// </summary>
/// <typeparam name="before">変換元の型</typeparam>
/// <typeparam name="after">変換先の型</typeparam>
/// <param name="array">変換元の型の要素を持つ配列</param>
/// <returns>変換後の型の要素を持つ配列</returns>
template<typename before, typename after>
static std::vector<after> ArrayCast(const std::vector<before>& array) {

    // 配列数取得
    size_t size = array.size();

    // 変換後の型の配列を作成する（初期値は0）
    std::vector<after> ret(size, 0);

    // 変換前の型の配列を変換後の型の配列にコピーしていく
    for (size_t i = 0; i < size; i++) { ret.at(i) = (after)array.at(i); }

    // 変換後の型の配列を返す
    return ret;
}

// %変換
static float PercentConversion(int percent) { return ((float)percent * 0.01f); }

template<typename T>
constexpr T UtilityClamp(const T& value, const T& min, const T& max)
{
    if (value < min) return min;
    if (value > max) return max;

    return value;
}