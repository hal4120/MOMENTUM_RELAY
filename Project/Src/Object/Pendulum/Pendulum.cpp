#include "Pendulum.h"

#include <cmath>
#include <stdexcept>
#include "../Common/Collider/SphereCollider.h"

Pendulum::Pendulum(const PendulumDefinition& definition) :
    definition(definition),
    angle(definition.initialAngle * DX_PI_F / 180.0f),
    angularVelocity(0.0f)
{
    if (definition.length <= definition.ballRadius || definition.ballRadius <= 0.0f) {
        throw std::invalid_argument("Invalid pendulum dimensions.");
    }
    trans.scale = definition.scale;
    trans.centerDiff = Vector3::Yonly(definition.length);
    UpdatePose();
    trans.prevPos = trans.pos;
}

void Pendulum::Load(void)
{
    const int previousAsync = GetUseASyncLoadFlag();
    SetUseASyncLoadFlag(FALSE);
    trans.model = MV1LoadModel(definition.modelPath.c_str());
    SetUseASyncLoadFlag(previousAsync);
    if (trans.model < 0) {
        throw std::runtime_error("Failed to load pendulum model: " + definition.modelPath);
    }

    SetColliderMobility(COLLIDER_MOBILITY::Movable);
    EnablePhysics();
    GetPhysicsBody()->SetMode(PhysicsBody::MotionMode::Kinematic);
    GetPhysicsBody()->SetGravityEnabled(false);
    GetPhysicsBody()->SetRotationEnabled(false);
    GetPhysicsBody()->SetRestitution(0.0f);
    AddCollider(new SphereCollider(COLLIDER_TAG::Pendulum, definition.ballRadius));
}

void Pendulum::SubUpdate(void)
{
    if (GetPhysicsBody()->IsFrozen()) { return; }
    const float timeScale = TimeScale::Get();
    if (timeScale <= 0.0f) { return; }

    // Same world-unit gravity and frame convention as the other physics bodies.
    angularVelocity -= (0.9f / definition.length) * std::sin(angle) * timeScale;
    angle += angularVelocity * timeScale;
    UpdatePose();

    const Vector3 tangent(std::cos(angle), std::sin(angle), 0.0f);
    GetPhysicsBody()->SetVelocity(definition.rotation.Rotate(tangent) *
        (definition.length * angularVelocity));
}

void Pendulum::VelGive(const Vector3& velocity)
{
    const Vector3 tangent = definition.rotation.Rotate(
        Vector3(std::cos(angle), std::sin(angle), 0.0f));
    angularVelocity = velocity.Dot(tangent) / definition.length;
    // The hinge absorbs the component that cannot move along the swing arc.
    GetPhysicsBody()->GiveVelocity(tangent * velocity.Dot(tangent));
}

void Pendulum::UpdatePose(void)
{
    trans.rotation = definition.rotation * Quaternion::FromRotationZ(angle);
    trans.pos = definition.pivot + trans.rotation.Rotate(Vector3::Yonly(-definition.length));
}
