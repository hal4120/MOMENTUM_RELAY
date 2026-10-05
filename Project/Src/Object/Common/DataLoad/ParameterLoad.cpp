#include "ParameterLoad.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "../../../Utility/Utility.h"

namespace fs = std::filesystem;

ParameterLoad::ParameterLoad(const std::string& directoryPath)
{
    // フォルダ内を走査
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        // ファイルじゃなければスキップ
        if (!entry.is_regular_file()) { continue; }

        // 拡張子取得
        const fs::path& path = entry.path();

        // .csvじゃなければスキップ
        if (path.extension() != ".csv") { continue; }

        // ファイル名のみ取得（拡張子なし）
        std::string fileName = path.stem().string();

        // CSV読み込み
        LoadCsvFile(path.string(), fileName);
    }
}

void ParameterLoad::LoadCsvFile(const std::string& filePath, const std::string& fileName)
{
    std::ifstream ifs(filePath);

    if (!ifs) {
        std::cerr << "CSVファイルを開けませんでした : " << filePath << std::endl;
        return;
    }

    std::string line;

    while (getline(ifs, line)) {
        // カンマ区切り
        std::vector<std::string> parameterStringArray = Split(line, ',');

        // データ不足ならスキップ
        if (parameterStringArray.empty()) { continue; }

        std::string parameterName;
        std::vector<float> values;

        for (size_t i = 0; i < parameterStringArray.size(); i++) {
            if (i == PARAMETER_NAME_INDEX) { parameterName = parameterStringArray[i]; }
            else { values.emplace_back(std::stof(parameterStringArray[i])); }
        }

        // 保存
        parameterMap[fileName][parameterName] = values;
    }
}

bool ParameterLoad::IsParameterExist(const std::string& fileName, const std::string& parameterName) const
{
    auto fileIt = parameterMap.find(fileName);
    if (fileIt == parameterMap.end()) { return false; }
    const auto& parameterMapForFile = fileIt->second;
	return parameterMapForFile.find(parameterName) != parameterMapForFile.end();
}

const std::vector<float>& ParameterLoad::GetParameterArray(const std::string& fileName, const std::string& parameterName) const
{
    return parameterMap.at(fileName).at(parameterName);
}

float ParameterLoad::GetParameter(const std::string& fileName, const std::string& parameterName, int index) const
{
    return parameterMap.at(fileName).at(parameterName).at(index);
}

int ParameterLoad::GetParameterToInt(const std::string& fileName, const std::string& parameterName, int index) const
{
    return static_cast<int>(GetParameter(fileName, parameterName, index));
}

Vector3 ParameterLoad::GetParameterToVector3(const std::string& fileName, const std::string& parameterName) const 
{
    const std::vector<float>& param = GetParameterArray(fileName, parameterName);

    // 要素数不一致
    if (param.size() != 3) { return Vector3(); }

    return Vector3(param[0], param[1], param[2]);
}

Vector2 ParameterLoad::GetParameterToVector2(const std::string& fileName, const std::string& parameterName) const
{
    const std::vector<float>& param = GetParameterArray(fileName, parameterName);

    // 要素数不一致
    if (param.size() != 2) { return Vector2(); }

    return Vector2(param[0], param[1]);
}

Vector2I ParameterLoad::GetParameterToVector2I(const std::string& fileName, const std::string& parameterName) const
{
    const std::vector<float>& param = GetParameterArray(fileName, parameterName);

    // 要素数不一致
    if (param.size() != 2) { return Vector2I(); }

    return Vector2I(param[0], param[1]);
}

void ParameterLoad::Release(void) { parameterMap.clear(); }