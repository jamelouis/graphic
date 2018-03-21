
struct Light
{
    float4 vLightDir;
    float4 vLightColor;
};

cbuffer cbCommon        : register(b0) {
  matrix worldViewProj  : packoffset(c0);
  Light  light          : packoffset(c4);
  matrix shadowWVP      : packoffset(c6);
};

struct VS_Input
{
    float4 position : POSITION;
    float4 normal : NORMAL;
};

struct VS_Output
{
    float4 position : SV_POSITION;
};

VS_Output VSMain( VS_Input In) 
{
    VS_Output Out = (VS_Output)0;

    float4 pos = In.position;
 
    Out.position = mul(pos, worldViewProj);

    return Out;
}