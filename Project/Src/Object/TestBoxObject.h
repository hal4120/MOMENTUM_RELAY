#pragma once

#include "Common/ActorBase/ActorBase.h"

class BoxCollider;

// Box物理の検証用。DebugObject系を介さずActorBaseを直接継承する。
class TestBoxObject : public ActorBase
{
public:
    enum class TestMode {
        FreeFall,       // 重力で落とす
        SpinInAir,      // 重力なしで角速度の減衰を確認
        PushByPlayer    // プレイヤーのCapsuleで押す
    };

    TestBoxObject(const Vector3& size, const Vector3& pos,
        TestMode mode = TestMode::FreeFall);
    ~TestBoxObject() override = default;

    void Load(void) override;

private:
    void SubDraw(void) override;

    TestMode testMode;
    Vector3 boxSize;
    // ActorBaseが所有するColliderへの非所有参照
    BoxCollider* boxCollider = nullptr;
};
