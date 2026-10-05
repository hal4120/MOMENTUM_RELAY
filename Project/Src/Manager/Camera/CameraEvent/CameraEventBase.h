#pragma once

class CameraBase;

class CameraEventBase
{
public:

    CameraEventBase() = default;

    virtual ~CameraEventBase() = default;

    // 開始
    virtual void Start(CameraBase& camera) {}

    // 更新
    virtual void Update(CameraBase& camera) = 0;

    // 終了
    virtual void End(CameraBase& camera) {}

    // 終了したか
    virtual bool IsEnd(void) const = 0;
};