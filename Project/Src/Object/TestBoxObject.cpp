#include "TestBoxObject.h"

#include "Common/Collider/BoxCollider.h"
#include "Common/PhysicsBody/PhysicsBody.h"

int TestBoxObject::modelHandleOriginal = -1;

TestBoxObject::TestBoxObject(const Vector3& size, const Vector3& pos, TestMode mode) :
    ActorBase(), testMode(mode), boxSize(size)
{
    trans.pos = pos;

    // 物理Bodyの有無と独立して、動くColliderとして登録する。
    SetColliderMobility(COLLIDER_MOBILITY::Movable);

    // 物理挙動を生成
    EnablePhysics();

    // 重力
    GetPhysicsBody()->SetGravityEnabled(true);

    // 押し出し
    SetContactResponseEnabled(true);
}

void TestBoxObject::Load(void)
{
    if (modelHandleOriginal == -1) { modelHandleOriginal = MV1LoadModel("Data/Model/Blocks/Block_Crate.mv1"); }
    //trans.LoadModel("Blocks/Block_Crate");
    trans.model = modelHandleOriginal;

    trans.scale = boxSize / Vector3(200);

    // BoxColliderはActorBaseが管理・解放する。
    AddCollider(new BoxCollider(COLLIDER_TAG::DebugObject, boxSize));

    // SpinInAirでも角速度検証にPhysicsBodyが必要。
    EnablePhysics();
    PhysicsBody* body = GetPhysicsBody();
    if (body == nullptr) { return; }

    body->SetMode(PhysicsBody::MotionMode::Dynamic);
    body->SetMass(0.2f);
    body->SetRotationEnabled(true);
    body->SetShapeInertiaBox(boxSize);
    body->SetRestitution(0.05f);
    body->SetFriction(0.6f);
    body->SetAngularDragPerFrame(0.005f);
    body->SetHorizontalDragPerFrame(0.03f);
    body->SetGravityEnabled(testMode != TestMode::SpinInAir);

    if (testMode == TestMode::SpinInAir) {
        // 外部からの力なしで回転減衰を検証する。
        body->SetAngularVelocity(Vector3(0.0f, 0.025f, 0.0f));
    }
}

void TestBoxObject::SubRelease(void)
{
    if (modelHandleOriginal != -1) {
        MV1DeleteModel(modelHandleOriginal);
        modelHandleOriginal = -1;
    }
}