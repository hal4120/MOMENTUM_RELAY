#define DX_D3D11_COMMON_CONST_LIGHT_NUM (6)
#define DX_VERTEXLIGHTING_LIGHT_NUM (3)
#define DX_PIXELLIGHTING_LIGHT_NUM (6)
#define DX_LIGHTTYPE_POINT (1)
#define DX_LIGHTTYPE_SPOT (2)
#define DX_LIGHTTYPE_DIRECTIONAL (3)
struct Material { float4 diffuse; float4 specular; float4 ambientEmissive; float power; float typeParam0; float typeParam1; float typeParam2; };
struct VsFog { float linearAdd; float linearDiv; float density; float e; float4 color; };
struct Light
{
    int type; int3 padding1;
    float3 position; float rangePow2;
    float3 direction; float fallOff;
    float3 diffuse; float spotParam0;
    float3 specular; float spotParam1;
    float4 ambient;
    float attenuation0; float attenuation1; float attenuation2; float padding2;
};
struct Common { Light light[DX_D3D11_COMMON_CONST_LIGHT_NUM]; Material material; VsFog fog; };
