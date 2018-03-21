
struct Light
{
    float4 vLightDir;
    float4 vLightColor;
};

cbuffer cbCommon : register(b0) {
  matrix worldViewProj  : packoffset(c0);
  Light  light          : packoffset(c4);
  matrix shadowWVP      : packoffset(c6);
};

cbuffer cbMaterial : register(b1) {
    float3 ka;
    float3 kd;
    float3 ks;
};

Texture2D                g_txShadow : register(t0);
SamplerComparisonState   g_samShadow : register(s0);

struct VS_Input
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    
};

struct VS_Output
{
    float4 position : SV_POSITION;
    float4 normal   : TEXCOORD0;
    float4 vPos     : POSITION0;
};

VS_Output VSMain( VS_Input In) 
{
    VS_Output Out = (VS_Output)0;

    float4 pos = In.position;
 
    Out.position = mul(pos, worldViewProj);
    Out.normal = In.normal;

    Out.vPos = In.position;
    return Out;
}

float CalcShadows(float4 vPos)
{
    float4 vNDC = mul(vPos, shadowWVP);
    vNDC /= vNDC.w;
    float2 vTexCoord = float2(vNDC.x*0.5 + 0.5, 0.5 - 0.5*vNDC.y);
    return g_txShadow.SampleCmpLevelZero(g_samShadow, vTexCoord, vNDC.z - 0.01f);
}

float4 PSMain(VS_Output InData) : SV_TARGET 
{
    float shadow = CalcShadows(InData.vPos);

    float NdotL = dot(InData.normal.xyz, light.vLightDir);
    
    float3 diffuse = light.vLightColor.rgb * NdotL * kd.rgb * shadow;

   // return float4(kd.rgb,1.0);
   return float4(diffuse,1.0);
}
