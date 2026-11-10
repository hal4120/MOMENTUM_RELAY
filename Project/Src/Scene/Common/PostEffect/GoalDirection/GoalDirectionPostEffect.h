#pragma once
#include "../PostEffectBase.h"

// GameStartCameraEvent の待機中だけ表示する文字演出。
class GoalDirectionPostEffect : public PostEffectBase
{
public:
    GoalDirectionPostEffect();
    ~GoalDirectionPostEffect() override = default;

    void Init(void) override;
    void Update(void) override;
    void Release(void) override;
    bool IsFinished(void) const override { return finished; }
    void Finish(void) { finished = true; }

protected:

    void ApplyParameter(void) override;
    void ResetParameter(void) override;

private:

    struct ConstBuffer { float time; float intensity; float flashSpeed; float padding; };

    ConstBuffer constBufferData;

    int constBufferHandle;

    int textTextureHandle;

    bool finished;
};
