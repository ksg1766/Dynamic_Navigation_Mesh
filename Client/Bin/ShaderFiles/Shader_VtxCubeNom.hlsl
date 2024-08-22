#include "Client_Shader_Defines.hlsl"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float       g_fFar = 2000.0f;

struct VS_SPHERE_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_SPHERE_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_SPHERE_OUT VS_SPHERE(VS_SPHERE_IN In)
{
    VS_SPHERE_OUT Out = (VS_SPHERE_OUT) 0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = float4(In.vNormal, 0.f);
    Out.vProjPos = Out.vPosition;
	
	return Out;
}

struct PS_SPHERE_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_SPHERE_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_SPHERE_OUT PS_SPHERE(PS_SPHERE_IN In)
{
    PS_SPHERE_OUT Out = (PS_SPHERE_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vNormal = 0.5f + 0.5f * In.vNormal;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Sphere
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SPHERE();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SPHERE();
        ComputeShader = NULL;
    }
}