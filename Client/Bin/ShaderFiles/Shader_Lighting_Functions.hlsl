#ifndef _SHADER_LIGHTING_FUNCTIONS_HLSL_
#define _SHADER_LIGHTING_FUNCTIONS_HLSL_

// Temp function file

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
	// [0,255] 범위에서 [0,1]로 변환
    float4 map = g_NormalTexture.Sample(LinearSampler, texcoord);
    if (any(map.rgb) == false)
        return;

    float3 N = normalize(normal.xyz); // z
    float3 T = normalize(tangent); // x
    float3 B = normalize(cross(N, T)); // y
    float3x3 TBN = float3x3(T, B, N); // TS -> WS

	// [0,1] 범위에서 [-1,1] 범위로 변환
    float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
    float3 worldNormal = mul(tangentSpaceNormal, TBN);

    normal = float4(worldNormal, 0.f);
};

Texture2D g_DissolveTexture;
float g_fDissolveAmount;

void ComputeDissolveColor(inout float4 color, float2 texcoord)
{
    float4 deffuseCol = g_DiffuseTexture.Sample(LinearSampler, texcoord);
    float dissolveSample = g_DissolveTexture.Sample(AlphaSampler, texcoord).x;
   
	//Discard the pixel if the value is below zero
    clip(dissolveSample - g_fDissolveAmount);
    float4 emissive = { 0.f, 0.f, 0.f, 0.f };
	//Make the pixel emissive if the value is below ~f
    if (dissolveSample - g_fDissolveAmount < 0.25f)/*0.08f*/ 
    {
        emissive = float4(1.f, 0.5f, 0.f, 1.f);
    }
    
    color = (color + emissive) * deffuseCol;
};

#endif