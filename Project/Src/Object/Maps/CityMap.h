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

    struct BoxCollision {
        Vector3 center;
        Vector3 size;
        Quaternion rotation;
    };
    const std::vector<BoxCollision>& GetBoxCollisions() const { return boxCollisions; }
    struct MeshCollision {
        std::vector<Vector3> vertices; // game world coordinates
        std::vector<unsigned int> indices;
    };
    const std::vector<MeshCollision>& GetMeshCollisions() const { return meshCollisions; }

private:
    struct Instance {
        std::string meshName;
        int handle = -1;
        Transform transform;
    };
    std::unordered_map<std::string, int> models;
    std::vector<Instance> objects;
    std::vector<BoxCollision> boxCollisions;
    std::vector<MeshCollision> meshCollisions;
};
