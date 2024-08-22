#ifndef _SHADER_FUNCTIONS_HLSL_
#define _SHADER_FUNCTIONS_HLSL_

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler AlphaSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

void ComputeNormalMapping(inout float4 normal, float3 tangent, float2 texcoord)
{
    float4 map = g_NormalTexture.Sample(LinearSampler, texcoord);
    if (any(map.rgb) == false)
        return;

    float3 N = normalize(normal.xyz); // z
    float3 T = normalize(tangent); // x
    float3 B = normalize(cross(N, T)); // y
    float3x3 TBN = float3x3(T, B, N); // TS -> WS

    float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
    float3 worldNormal = mul(tangentSpaceNormal, TBN);

    normal = float4(worldNormal, 0.f);
};

#endif