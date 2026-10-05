struct ModelLightingResult { float3 ambient; float3 diffuse; float3 specular; };
float CalcDistanceAttenuation(Light l,float d){return 1.0/(l.attenuation0+l.attenuation1*d+l.attenuation2*d*d);}
ModelLightingResult CalcModelLighting(float3 viewPos,float3 viewNormal)
{
    ModelLightingResult r=(ModelLightingResult)0;
    float3 n=normalize(viewNormal), toCamera=normalize(-viewPos);
    [unroll] for(int i=0;i<DX_D3D11_COMMON_CONST_LIGHT_NUM;++i)
    {
        Light l=g_common.light[i]; if(l.type==0) continue;
        r.ambient+=l.ambient.rgb;
        float3 toLight=0; float att=1;
        if(l.type==DX_LIGHTTYPE_DIRECTIONAL) toLight=-normalize(l.direction);
        else if(l.type==DX_LIGHTTYPE_POINT)
        { float3 v=l.position-viewPos; float d=length(v); toLight=normalize(v); att=CalcDistanceAttenuation(l,d); }
        else if(l.type==DX_LIGHTTYPE_SPOT)
        { float3 v=l.position-viewPos; float d=length(v); toLight=normalize(v); float da=CalcDistanceAttenuation(l,d); float sc=dot(normalize(l.direction),-toLight); float spot=saturate((sc-l.spotParam0)*l.spotParam1); spot=pow(spot,l.fallOff); att=da*spot; }
        float brightness=max(dot(n,toLight),0);
        r.diffuse+=l.diffuse*brightness*att;
        float3 reflected=reflect(-toLight,n);
        float sp=pow(max(dot(reflected,toCamera),0),g_common.material.power);
        r.specular+=l.specular*g_common.material.specular.rgb*sp*att;
    }
    return r;
}
