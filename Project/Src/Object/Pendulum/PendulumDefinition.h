#pragma once

#include <string>
#include "../../Common/Vector3.h"
#include "../../Common/Quaternion.h"

struct PendulumDefinition
{
    std::string modelPath;
    Vector3 pivot;
    Quaternion rotation;
    Vector3 scale;
    float length = 0.0f;
    float ballRadius = 0.0f;
    float initialAngle = 30.0f;
};
