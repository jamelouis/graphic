
struct Light
{
    float4 vLightDir;
    float4 vLightColor;
};

cbuffer cbCommon : register(b0) {
  matrix worldViewProj      : packoffset(c0);
  Light  light              : packoffset(c4);
  matrix shadowWVP          : packoffset(c6);
  matrix storageTransorm    : packoffset(c10);
  float  voxelScale         : packoffset(c14.x);
  uint   dim                : packoffset(c14.y);
};
