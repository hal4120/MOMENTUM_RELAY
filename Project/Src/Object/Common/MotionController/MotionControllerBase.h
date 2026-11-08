#pragma once

#include "../../../Manager/TimeScale/TimeScale.h"

#include "../PhysicsBody/PhysicsBody.h"

// 移動操作の共通基底。PhysicsBodyの所有権は持たない。
class MotionControllerBase
{
public:
    virtual ~MotionControllerBase() = default;

    // CharacterBase::AddMotionControllerから一度だけ設定する。
    void Init(PhysicsBody* body) { physicsBody = body; }

    // 時間倍率は1フレーム分。接地判定は派生Controllerが管理する。
    virtual void ApplyInput(const Vector3& direction, float timeScale = TimeScale::Get()) = 0;

    // 入力がないステートでも毎フレーム呼び出す移動制御。
    virtual void Update(float timeScale) {}

protected:

    // 物理挙動制御の参照
    PhysicsBody* physicsBody = nullptr;
};
