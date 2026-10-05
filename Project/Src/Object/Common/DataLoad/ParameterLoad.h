#pragma once

#include <map>
#include <vector>
#include <string>

struct Vector3;
struct Vector2;
struct Vector2I;

class ParameterLoad
{
public:
    // フォルダパスを受け取る
    ParameterLoad(const std::string& directoryPath);

    ~ParameterLoad() = default;

    bool IsParameterExist(
        const std::string& fileName,
		const std::string& parameterName) const;

    // 配列ごと取得
    const std::vector<float>& GetParameterArray(
        const std::string& fileName,
        const std::string& parameterName) const;

    // そのままfloat型として取得
    float GetParameter(
        const std::string& fileName,
        const std::string& parameterName,
        int index = 0) const;

    /// int型にキャストして取得
    int GetParameterToInt(
        const std::string& fileName,
        const std::string& parameterName,
        int index = 0) const;

    /// Vector3型として取得
    Vector3 GetParameterToVector3(
        const std::string& fileName,
        const std::string& parameterName) const;

    /// Vector2型として取得
    Vector2 GetParameterToVector2(
        const std::string& fileName,
        const std::string& parameterName) const;

    /// Vector2I型として取得
    Vector2I GetParameterToVector2I(
        const std::string& fileName,
        const std::string& parameterName) const;

    void Release(void);

private:

    // CSVファイルを読み込む関数
    void LoadCsvFile(const std::string& filePath, const std::string& fileName);

    // パラメータ名の列
    static constexpr int PARAMETER_NAME_INDEX = 0;

	// パラメータを格納するマップ
    std::map<std::string, std::map<std::string, std::vector<float>>> parameterMap;
};