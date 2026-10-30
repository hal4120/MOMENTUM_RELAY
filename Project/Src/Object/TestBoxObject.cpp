#include "TestBoxObject.h"

#include "Common/Collider/BoxCollider.h"
#include "Common/PhysicsBody/PhysicsBody.h"

TestBoxObject::TestBoxObject(const Vector3& size, const Vector3& pos, TestMode mode) :
    ActorBase(), testMode(mode), boxSize(size)
{
    trans.pos = pos;

    // 物理Bodyの有無と独立して、動くColliderとして登録する。
    SetDynamicFlg(true);
    SetPushFlg(true);
    SetPushWeight(0);
    SetGravityFlg(mode != TestMode::SpinInAir);
}

void TestBoxObject::Load(void)
{
    // BoxColliderはActorBaseが管理・解放する。
    boxCollider = new BoxCollider(COLLIDER_TAG::DebugObject, boxSize);
    AddCollider(boxCollider);

    // SpinInAirでも角速度検証にPhysicsBodyが必要。
    CreatePhysicsBody();
    PhysicsBody* body = GetPhysicsBody();
    if (body == nullptr) { return; }

    body->SetMode(PhysicsBody::MotionMode::Dynamic);
    body->SetMass(10.0f);
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

void TestBoxObject::SubDraw(void)
{
    if (boxCollider == nullptr) { return; }

    // Colliderと同じ回転後の頂点を使って描画する。
    const auto vertices = boxCollider->GetVertices();
    static constexpr int faces[6][4] = {
        {0, 2, 3, 1}, {4, 5, 7, 6},
        {0, 4, 6, 2}, {1, 3, 7, 5},
        {0, 1, 5, 4}, {2, 6, 7, 3}
    };

    for (const auto& face : faces) {
        DrawTriangle3D(vertices[face[0]].ToVECTOR(), vertices[face[1]].ToVECTOR(),
            vertices[face[2]].ToVECTOR(), 0x777777, true);
        DrawTriangle3D(vertices[face[0]].ToVECTOR(), vertices[face[2]].ToVECTOR(),
            vertices[face[3]].ToVECTOR(), 0x777777, true);
    }

    // 角度が分かりやすいように輪郭線を描画する。
    static constexpr int edges[12][2] = {
        {0, 1}, {1, 3}, {3, 2}, {2, 0},
        {4, 5}, {5, 7}, {7, 6}, {6, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    for (const auto& edge : edges) {
        DrawLine3D(vertices[edge[0]].ToVECTOR(), vertices[edge[1]].ToVECTOR(), 0x111111);
    }
}
