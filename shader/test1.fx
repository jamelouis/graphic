
struct Light
{
    float4 vLightDir;
    float4 vLightColor;
};

cbuffer cbCommon : register(b0) {
  matrix worldViewProj  : packoffset(c0);
  Light  light          : packoffset(c4);
};

cbuffer cbMaterial : register(b1) {
    float3 ka;
    float3 kd;
    float3 ks;
};

struct VS_Input
{
    float4 position : POSITION;
    float4 normal : NORMAL;
};

struct VS_Output
{
    float4 position : SV_POSITION;
    float4 normal   : TEXCOORD0;
};

VS_Output VSMain( VS_Input In) 
{
    VS_Output Out = (VS_Output)0;

    float4 pos = In.position;
 
    Out.position = mul(pos, worldViewProj);
    Out.normal = In.normal;

    return Out;
}

float4 PSMain(VS_Output InData) : SV_TARGET 
{
      
    float NdotL = dot(InData.normal.xyz, light.vLightDir);
    
    float3 diffuse = light.vLightColor.rgb * NdotL * kd.rgb;

   return float4(ka.rgb+diffuse,1.0);
}
