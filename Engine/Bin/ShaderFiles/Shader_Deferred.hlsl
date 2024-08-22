#include "Engine_Shader_Defines.hlsl"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ProjMatrixInv;
float4x4 g_ViewMatrixInv;

float4	g_vCamPosition;
float4	g_vLightDir;

float4	g_vLightDiffuse;
float4	g_vLightAmbient;
float4	g_vLightSpecular;

float4  g_vMtrlAmbient = float4(0.5f, 0.5f, 0.5f, 1.f);
float4  g_vMtrlSpecular = float4(0.3f, 0.3f, 0.3f, 1.f);

Texture2D	g_NormalTarget;
Texture2D	g_DiffuseTarget;
Texture2D	g_DepthTarget;

Texture2D	g_ShadeTarget;
Texture2D	g_SpecularTarget;

Texture2D	g_Texture;
float       g_fFar = 2000.0f;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{	
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;
	
	float4x4	matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vTexcoord = In.vTexcoord;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4	vColor : SV_TARGET0;
};
    
struct PS_REFRACTION_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_REFLECTION_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
	float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;
    
	float4		vNormalDesc = g_NormalTarget.Sample(PointSampler, In.vTexcoord);
    float4		vDepthDesc = g_DepthTarget.Sample(PointSampler, In.vTexcoord);
    float		fViewZ = vDepthDesc.y * g_fFar;

	float4		vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));
    
	float4 vReflect = reflect(normalize(g_vLightDir), vNormal);
    
	float4 vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float4 vLook = vWorldPos - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 20.f);
		
	return Out;
}

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vDiffuse = g_DiffuseTarget.Sample(PointSampler, In.vTexcoord);
    if (vDiffuse.a < 0.1f)
    {
        discard;
    }

    float4 vShade = saturate(g_ShadeTarget.Sample(LinearSampler, In.vTexcoord));
    
    float4 vSpecular = g_SpecularTarget.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = (vDiffuse * vShade) + vSpecular;
    
    return Out;
}

technique11 DefaultTechnique
{
	pass Target_Debug   // 0
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
        ComputeShader = NULL;
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
        ComputeShader = NULL;
    }

    pass Deferred // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
        ComputeShader = NULL;
    }
}




