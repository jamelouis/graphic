
#include "common.h"

struct VS_Input
{
    float4 wPos     : POSITION;
    float4 texPos   : TEXCOORD;
    
};

struct GS_Input
{
    float4 wPos     : POSITION;
    float4 texPos   : TEXCOORD;
};

struct PS_Input
{
    float4 position : SV_POSITION;
    float4 texPos   : TEXCOORD;
};

Texture3D<float4>     g_txVoxel0 : register(t1);
Texture3D<float4>     g_txVoxel1 : register(t2);
Texture3D<float4>     g_txVoxel2 : register(t3);
Texture3D<float4>     g_txVoxel3 : register(t4);
Texture3D<float4>     g_txVoxel4 : register(t5);
Texture3D<float4>     g_txVoxel5 : register(t6);

SamplerState          g_samVoxel : register(s1);

GS_Input vsmain( VS_Input In)
{
    GS_Input Out = (GS_Input)0;

    Out.wPos = In.wPos;
    Out.texPos = In.texPos;

    return Out;
}

[maxvertexcount(16)]
void gsmain(point GS_Input In[1], inout LineStream<PS_Input> triStream)
{
    PS_Input Out;

    float offset = voxelScale/2.0;

    float4 pos0 = mul(In[0].wPos + float4(-offset, -offset, -offset, 0.0), worldViewProj);
    float4 pos1 = mul(In[0].wPos + float4( offset, -offset, -offset, 0.0), worldViewProj);
    float4 pos2 = mul(In[0].wPos + float4( offset,  offset, -offset, 0.0), worldViewProj);
    float4 pos3 = mul(In[0].wPos + float4(-offset,  offset, -offset, 0.0), worldViewProj);
    float4 pos4 = mul(In[0].wPos + float4(-offset, -offset,  offset, 0.0), worldViewProj);
    float4 pos5 = mul(In[0].wPos + float4( offset, -offset,  offset, 0.0), worldViewProj);
    float4 pos6 = mul(In[0].wPos + float4( offset,  offset,  offset, 0.0), worldViewProj);
    float4 pos7 = mul(In[0].wPos + float4(-offset,  offset,  offset, 0.0), worldViewProj);

    Out.texPos = In[0].texPos;

    Out.position = pos0;
    triStream.Append(Out);
    Out.position = pos1;
    triStream.Append(Out);
    Out.position = pos2;
    triStream.Append(Out);
    Out.position = pos3;
    triStream.Append(Out);
    Out.position = pos0;
    triStream.Append(Out);

    
#if 1
    Out.position = pos4;
    triStream.Append(Out);
    Out.position = pos5;
    triStream.Append(Out);
    Out.position = pos6;
    triStream.Append(Out);
    Out.position = pos7;
    triStream.Append(Out);
    Out.position = pos4;
    triStream.Append(Out);
#endif

    Out.position = pos1;
    triStream.Append(Out);
    Out.position = pos5;
    triStream.Append(Out);

    Out.position = pos2;
    triStream.Append(Out);
    Out.position = pos6;
    triStream.Append(Out);

    Out.position = pos3;
    triStream.Append(Out);
    Out.position = pos7;
    triStream.Append(Out);

}


float4 psmain(PS_Input In) : SV_TARGET
{
    float3 v0 = g_txVoxel0.SampleLevel(g_samVoxel, In.texPos.xyz, 0);
    float3 v1 = g_txVoxel1.SampleLevel(g_samVoxel, In.texPos.xyz, 0);
    float3 v2 = g_txVoxel2.SampleLevel(g_samVoxel, In.texPos.xyz, 0);
    float3 v3 = g_txVoxel3.SampleLevel(g_samVoxel, In.texPos.xyz, 0);
    float3 v4 = g_txVoxel4.SampleLevel(g_samVoxel, In.texPos.xyz, 0);
    float3 v5 = g_txVoxel5.SampleLevel(g_samVoxel, In.texPos.xyz, 0);

    float sum = v0.x + v0.y + v0.z
        + v1.x + v1.y + v1.z
        + v2.x + v2.y + v2.z
        + v3.x + v3.y + v3.z
        + v4.x + v4.y + v4.z
        + v5.x + v5.y + v5.z;
        
    if (sum < 0.01) discard;

    return 1.0f;
}

