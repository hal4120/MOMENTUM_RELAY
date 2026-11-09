#include "ModelLighting.hlsli"
#include "ModelFog.hlsli"

float3 CalcModelColor(float3 baseColor, float3 viewPos, float3 viewNormal)
{
    ModelLightingResult l = CalcModelLighting(viewPos, viewNormal);

    // Å’áŒÀ‚ÌŠÂ‹«Œõ‚ğ•ÛØ
    float3 ambient = max(l.ambient, float3(0.38f, 0.38f, 0.38f));

    return baseColor * (ambient + l.diffuse) + l.specular;
}

float3 FinalizeModelColor(float3 color,float3 viewPos){return ApplyModelFog(color,viewPos);}
