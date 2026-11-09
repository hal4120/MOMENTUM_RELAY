#pragma once

#include "../Common/ActorBase/ActorBase.h"

class BoxCollider;

// Box•¨—‚ÌŒŸØ—pBDebugObjectŒn‚ğ‰î‚³‚¸ActorBase‚ğ’¼ÚŒp³‚·‚éB
class TestGoal : public ActorBase
{
public:

    TestGoal(const Vector3& size, const Vector3& pos);
    ~TestGoal() override = default;

    void Load(void) override;

private:

    Vector3 boxSize;

    void SubRelease(void)override;

    static int modelHandleOriginal;
};
