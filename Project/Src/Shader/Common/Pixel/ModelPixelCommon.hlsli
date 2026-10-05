#include "ModelLighting.hlsli"
#include "ModelFog.hlsli"
float3 CalcModelColor(float3 baseColor,float3 viewPos,float3 viewNormal)
{
    ModelLightingResult l=CalcModelLighting(viewPos,viewNormal);
    return baseColor*(l.ambient+l.diffuse)+l.specular;
}
float3 FinalizeModelColor(float3 color,float3 viewPos){return ApplyModelFog(color,viewPos);}
