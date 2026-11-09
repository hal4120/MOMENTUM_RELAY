#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "../../Object/Common/Transform/Transform.h"

// Static map renderer. Model handles are shared and owned by this class.
class CityMap {
public:
    CityMap() = default;

    bool Load(const std::string& jsonPath, const std::string& assetRoot);
    void Draw() const;
    void Release();
    size_t GetObjectCount() const { return objects.size(); }
    size_t GetModelCount() const { return models.size(); }

private:
    struct Instance {
        std::string meshName;
        int handle = -1;
        Transform transform;
    };
    std::unordered_map<std::string, int> models;
    std::vector<Instance> objects;
};
