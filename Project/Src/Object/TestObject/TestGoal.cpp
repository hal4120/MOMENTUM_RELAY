#include "TestGoal.h"

#include "../Common/Collider/BoxCollider.h"
#include "../Common/PhysicsBody/PhysicsBody.h"

int TestGoal::modelHandleOriginal = -1;

TestGoal::TestGoal(const Vector3& size, const Vector3& pos) :
    ActorBase(), boxSize(size)
{
    trans.pos = pos;

    // 物理Bodyの有無と独立して、動くColliderとして登録する。
    SetColliderMobility(COLLIDER_MOBILITY::Static);

    // 押し出し
    SetContactResponseEnabled(true);
}

void TestGoal::Load(void)
{
    // 静的に定義したハンドル番号格納変数を使いまわす
    if (modelHandleOriginal == -1) { modelHandleOriginal = MV1LoadModel("Data/Model/Blocks/Block_Crate.mv1"); }

    // モデルのハンドルを設定
    trans.model = modelHandleOriginal;

    // モデルのスケールを設定
    trans.scale = boxSize / Vector3(200);

    // BoxColliderはActorBaseが管理・解放する。
    AddCollider(new BoxCollider(COLLIDER_TAG::DebugObject, boxSize));
}

void TestGoal::SubRelease(void)
{
    if (modelHandleOriginal != -1) {
        MV1DeleteModel(modelHandleOriginal);
        modelHandleOriginal = -1;
    }
}