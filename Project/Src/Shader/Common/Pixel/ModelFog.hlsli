float CalcLinearFogFactor(float3 viewPos){float d=length(viewPos);return saturate(g_common.fog.linearAdd+d*g_common.fog.linearDiv);}
float3 ApplyModelFog(float3 color,float3 viewPos){return lerp(g_common.fog.color.rgb,color,CalcLinearFogFactor(viewPos));}
