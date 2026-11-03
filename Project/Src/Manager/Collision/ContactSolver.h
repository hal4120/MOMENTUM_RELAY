#pragma once
#include <array>
#include "../../Common/Vector3.h"
class ColliderBase;

// BからAへの法線。接触点はワールド座標。
struct ContactManifold
{
    ColliderBase* a = nullptr;
    ColliderBase* b = nullptr;
    Vector3 normal = Vector3(0.0f);
    float penetration = 0.0f;
    std::array<Vector3, 4> points{};
    // 接触点ごとのめり込み量。Boxの面接触では各頂点で値が異なる。
    std::array<float, 4> depths{};
    int count = 0;

    // depth未指定の場合は従来どおりマニフォールドの深さを使用。
    void AddPoint(const Vector3& point) {
        AddPoint(point, penetration);
    }
    void AddPoint(const Vector3& point, float depth) {
        if (count >= static_cast<int>(points.size())) return;
        points[count] = point;
        depths[count] = depth > 0.0f ? depth : 0.0f;
        ++count;
    }
};

// 全形状で共有する接触点インパルスソルバー。
class ContactSolver
{
public:
    // フレーム境界でキャッシュを更新。CollisionManager::Checkから呼び出す。
    static void BeginFrame();
    static void EndFrame();
    static void Clear();
    static void AddContact(const ContactManifold& contact);
    static void AddPositionCorrection(ColliderBase* a, ColliderBase* b, const Vector3& displacement);
    static void AddOneSideCorrection(ColliderBase* movable, ColliderBase* obstacle, const Vector3& displacement);
    // 直近のCheckで処理した接触数・回転補正数を確認するための診断情報。
    struct FrameStats {
        int manifolds = 0;
        int contactPoints = 0;
        int positionConstraints = 0;
        int angularCorrections = 0;
        double velocitySolveMs = 0.0;
        double sleepUpdateMs = 0.0;
        double positionSolveMs = 0.0;
    };
    static FrameStats GetLastFrameStats();
    static void SolveAll();
};
