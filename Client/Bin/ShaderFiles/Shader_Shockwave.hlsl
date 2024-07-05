#include "Engine_Shader_Defines.hlsl"

matrix		g_WorldMatrix;
matrix		g_ViewMatrix, g_ProjMatrix;

Texture2D	g_Texture;

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

VS_OUT VS_MAIN(/* Á¤Á¡ */VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;
	
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vTexcoord = In.vTexcoord;

	return Out;
}

struct PS_IN
{
    float4 vPosition	: SV_POSITION;
	float2 vTexcoord	: TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor		: SV_TARGET0;
	float4 vDistortion	: SV_TARGET1;
};

PS_OUT PS_SHOCKWAVE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vSourColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    if (vSourColor.r == 0.f)
        discard;

    Out.vDistortion = vSourColor;

    return Out;
}

technique11 DefaultTechnique
{
	pass Shockwave
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SHOCKWAVE_MAIN();
        ComputeShader = NULL;
    }
}




