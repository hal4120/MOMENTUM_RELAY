#pragma once

#include "../Common/ActorBase/ActorBase.h"
#include "PendulumDefinition.h"

class Pendulum : public ActorBase
{
public:
    explicit Pendulum(const PendulumDefinition& definition);
    void Load(void) override;
    void VelGive(const Vector3& velocity) override;

private:
    void SubUpdate(void) override;
    void UpdatePose(void);

    PendulumDefinition definition;
    float angle;
    float angularVelocity;
};
