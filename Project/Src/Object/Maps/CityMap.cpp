#include "CityMap.h"

#include "../../pch.h"

#include <array>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>

namespace {

    // Minimal reader for the known Stage-1 exporter schema (not a general JSON library).
    // Each object is a flat JSON object with scalar strings and numeric arrays.
    size_t SkipWs(const std::string& s, size_t p) {
        while (p < s.size() && std::isspace(static_cast<unsigned char>(s[p]))) ++p;
        return p;
    }

    bool FindValue(const std::string& s, const std::string& key, size_t& p) {
        const std::string needle = "\"" + key + "\"";
        p = s.find(needle);
        if (p == std::string::npos) return false;
        p = s.find(':', p + needle.size());
        if (p == std::string::npos) return false;
        p = SkipWs(s, p + 1);
        return p < s.size();
    }

    bool ReadString(const std::string& s, const std::string& key, std::string& out) {
        size_t p;
        if (!FindValue(s, key, p) || s[p] != '"') return false;
        ++p;
        out.clear();
        while (p < s.size()) {
            const char c = s[p++];
            if (c == '"') return true;
            if (c == '\\' && p < s.size()) {
                const char escaped = s[p++];
                if (escaped == '"' || escaped == '\\' || escaped == '/') out.push_back(escaped);
                else return false; // paths in this exporter use forward slashes
            }
            else out.push_back(c);
        }
        return false;
    }

    template<size_t N>
    bool ReadNumbers(const std::string& s, const std::string& key, std::array<float, N>& out) {
        size_t p;
        if (!FindValue(s, key, p) || s[p++] != '[') return false;
        for (size_t i = 0; i < N; ++i) {
            p = SkipWs(s, p);
            if (p >= s.size()) return false;
            const char* begin = s.c_str() + p;
            char* end = nullptr;
            out[i] = std::strtof(begin, &end);
            if (end == begin) return false;
            p = static_cast<size_t>(end - s.c_str());
            p = SkipWs(s, p);
            if (i + 1 < N) { if (p >= s.size() || s[p++] != ',') return false; }
        }
        p = SkipWs(s, p);
        return p < s.size() && s[p] == ']';
    }

    std::string Directory(const std::string& path) {
        const auto pos = path.find_last_of("/\\");
        return pos == std::string::npos ? std::string() : path.substr(0, pos + 1);
    }

}

bool CityMap::Load(const std::string& jsonPath, const std::string& assetRoot) {
    Release();
    std::ifstream file(jsonPath, std::ios::binary);
    if (!file) { printfDx("CityMap: cannot open %s\n", jsonPath.c_str()); return false; }
    const std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    size_t arrayPos;
    if (!FindValue(json, "objects", arrayPos) || json[arrayPos] != '[') return false;
    const std::string root = assetRoot.empty() ? Directory(jsonPath) : assetRoot;
    size_t p = arrayPos + 1;
    size_t errors = 0;
    while (true) {
        p = SkipWs(json, p);
        if (p >= json.size()) { Release(); return false; }
        if (json[p] == ']') break;
        if (json[p] == ',') { ++p; continue; }
        if (json[p] != '{') { Release(); return false; }
        const size_t start = p;
        // Strings may contain braces, so track quoted sections as well.
        bool quoted = false, escaped = false;
        int depth = 0;
        do {
            const char c = json[p++];
            if (quoted) {
                if (escaped) escaped = false;
                else if (c == '\\') escaped = true;
                else if (c == '"') quoted = false;
            }
            else if (c == '"') quoted = true;
            else if (c == '{') ++depth;
            else if (c == '}') --depth;
        } while (p < json.size() && depth > 0);
        if (depth != 0) { Release(); return false; }
        const std::string item = json.substr(start, p - start);
        std::string path, mesh;
        std::array<float, 3> pos{}, scale{};
        std::array<float, 4> rot{};
        if (!ReadString(item, "modelFile", path) || !ReadString(item, "meshName", mesh) ||
            !ReadNumbers(item, "position", pos) || !ReadNumbers(item, "rotation", rot) ||
            !ReadNumbers(item, "scale", scale)) {
            ++errors; continue;
        }
        // Never allow exported paths to escape the asset directory.
        if (path.find("..") != std::string::npos || path.find(':') != std::string::npos ||
            (!path.empty() && (path[0] == '/' || path[0] == '\\'))) {
            ++errors; continue;
        }
        int handle = -1;
        const auto it = models.find(path);
        if (it == models.end()) {
            std::string full = root;
            if (!full.empty() && full.back() != '/' && full.back() != '\\') full += '/';
            full += path;
            // The exported JSON points directly to a .x model. DxLib supports it.
            // Keep the original extension and relative path.
            std::ifstream modelFile(full, std::ios::binary);
            if (!modelFile) {
                printfDx("CityMap: missing model: %s\n", full.c_str());
                ++errors;
                continue;
            }
            modelFile.close();

            // Preserve the original DxLib async setting even when load fails.
            const int previousAsync = GetUseASyncLoadFlag();
            SetUseASyncLoadFlag(FALSE);
            handle = MV1LoadModel(full.c_str());
            SetUseASyncLoadFlag(previousAsync);

            if (handle < 0) {
                printfDx("CityMap: model load failed: %s\n", full.c_str());
                ++errors;
                continue;
            }

            models.emplace(path, handle);
        }
        else handle = it->second;
        Instance instance;
        instance.handle = handle;
        instance.meshName = std::move(mesh);
        instance.transform.pos = Vector3(pos[0], pos[1], pos[2]);
        instance.transform.rotation = Quaternion(rot[0], rot[1], rot[2], rot[3]);
        instance.transform.scale = Vector3(scale[0], scale[1], scale[2]);
        objects.push_back(std::move(instance));
    }
    printfDx("CityMap: %d instances, %d models, %d skipped\n",
        static_cast<int>(objects.size()), static_cast<int>(models.size()), static_cast<int>(errors));
    // Optional collision array (older exports without it remain valid).
    size_t collisionArray;
    if (FindValue(json, "colliders", collisionArray) && json[collisionArray] == '[') {
        size_t cursor = collisionArray + 1;
        while (cursor < json.size()) {
            cursor = SkipWs(json, cursor);
            if (cursor >= json.size() || json[cursor] == ']') break;
            if (json[cursor] == ',') { ++cursor; continue; }
            if (json[cursor] != '{') break;
            const size_t begin = cursor;
            bool inString = false, escapedChar = false;
            int nesting = 0;
            do {
                const char c = json[cursor++];
                if (inString) {
                    if (escapedChar) escapedChar = false;
                    else if (c == '\\') escapedChar = true;
                    else if (c == '"') inString = false;
                }
                else {
                    if (c == '"') inString = true;
                    else if (c == '{') ++nesting;
                    else if (c == '}') --nesting;
                }
            } while (cursor < json.size() && nesting > 0);
            if (nesting != 0) break;
            const std::string entry = json.substr(begin, cursor - begin);
            std::string type;
            ReadString(entry, "type", type);
            if (type == "Mesh") {
                std::string path;
                if (!ReadString(entry, "meshFile", path) || path.empty() ||
                    path.find("..") != std::string::npos || path.find(':') != std::string::npos ||
                    path[0] == '/' || path[0] == '\\') {
                    ++errors; continue;
                }
                std::string full = root;
                if (!full.empty() && full.back() != '/' && full.back() != '\\') full += '/';
                full += path;
                std::ifstream meshFile(full);
                if (!meshFile) { printfDx("CityMap: missing collision mesh: %s\n", full.c_str()); ++errors; continue; }
                size_t vertexCount = 0, indexCount = 0;
                if (!(meshFile >> vertexCount >> indexCount) || vertexCount > 2000000 ||
                    indexCount > 6000000 || indexCount % 3 != 0) {
                    ++errors; continue;
                }
                MeshCollision mesh;
                mesh.vertices.reserve(vertexCount);
                mesh.indices.reserve(indexCount);
                bool valid = true;
                for (size_t i = 0; i < vertexCount; ++i) {
                    float x, y, z;
                    if (!(meshFile >> x >> y >> z)) { valid = false; break; }
                    mesh.vertices.emplace_back(
                        (x - 125.0f) * 150.0f - 264.0f,
                        y * 150.0f,
                        (z - 100.0f) * 150.0f + 712.0f);
                }
                for (size_t i = 0; valid && i < indexCount; ++i) {
                    unsigned int index;
                    if (!(meshFile >> index) || index >= vertexCount) { valid = false; break; }
                    mesh.indices.push_back(index);
                }
                if (!valid || mesh.vertices.empty() || mesh.indices.empty()) { ++errors; continue; }
                meshCollisions.push_back(std::move(mesh));
                continue;
            }
            std::array<float, 3> center{}, size{};
            std::array<float, 4> rotation{};
            if (!ReadNumbers(entry, "center", center) ||
                !ReadNumbers(entry, "size", size) ||
                !ReadNumbers(entry, "rotation", rotation)) continue;
            if (size[0] <= 0.0f || size[1] <= 0.0f || size[2] <= 0.0f) continue;
            BoxCollision box;
            constexpr float MAP_SCALE = 150.0f;
            box.center = Vector3(
                (center[0] - 125.0f) * MAP_SCALE - 264.0f,
                center[1] * MAP_SCALE,
                (center[2] - 100.0f) * MAP_SCALE + 712.0f);
            box.size = Vector3(size[0] * MAP_SCALE, size[1] * MAP_SCALE, size[2] * MAP_SCALE);
            box.rotation = Quaternion(rotation[0], rotation[1], rotation[2], rotation[3]);
            boxCollisions.push_back(box);
        }
    }
    printfDx("CityMap: %d box colliders, %d mesh colliders\n", static_cast<int>(boxCollisions.size()), static_cast<int>(meshCollisions.size()));
    return !objects.empty() && errors == 0;
}

void CityMap::Draw() const
{
    // マップ描画時だけライティングを無効化
    SetUseLighting(FALSE);

    const float MAP_SCALE = 150.0f;

    // Unityマップの中心を原点付近へ移動
    const float MAP_CENTER_X = 125.0f;
    const float MAP_CENTER_Z = 100.0f;

    // ゲーム内での配置基準
    const Vector3 MAP_ORIGIN(-264.0f, 0.0f, 712.0f);

    for (const Instance& obj : objects) {
        if (obj.handle < 0) continue;

        MATRIX world = obj.transform.GetWorldMatrix();

        // モデルのサイズと配置座標を拡大
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 3; ++col) {
                world.m[row][col] *= MAP_SCALE;
            }
        }

        // マップ全体の配置を調整
        world.m[3][0] += MAP_ORIGIN.x - MAP_CENTER_X * MAP_SCALE;
        world.m[3][1] += MAP_ORIGIN.y;
        world.m[3][2] += MAP_ORIGIN.z - MAP_CENTER_Z * MAP_SCALE;

        MV1SetMatrix(obj.handle, world);
        MV1DrawModel(obj.handle);
    }


    // 他のモデルに影響しないよう戻す
    SetUseLighting(TRUE);
}

void CityMap::Release() {
    boxCollisions.clear();
    meshCollisions.clear();
    objects.clear();
    for (auto& entry : models) if (entry.second >= 0) MV1DeleteModel(entry.second);
    models.clear();
}
