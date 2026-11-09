#pragma once
#include "../Common/ActorBase/ActorBase.h"
#include "CityMap.h"

// Owns all imported static map colliders. Registered by SceneBase::AddActor.
class CityCollisionActor final : public ActorBase {
public:
    explicit CityCollisionActor(const CityMap& map);
    void Load(void) override {}
};
