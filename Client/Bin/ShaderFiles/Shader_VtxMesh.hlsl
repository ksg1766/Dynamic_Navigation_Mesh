#include "Client_Shader_Defines.hlsl"
#include "Shader_Lighting_Functions.hlsl"

/* 상수테이블. */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector			g_vLightPos = vector(0.f, 0.f, 0.f, 1.f);
float			g_fLightRange = 0.f;
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightEmissive = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vMtrlEmissive = vector(1.f, 0.f, 0.f, 1.f);

vector			g_vCamPosition;

Texture2D       g_EmissiveTexture;

Texture2D       g_NoiseTexture;
Texture2D       g_MaskTexture;

float2          g_UVoffset;

// For Water
float4          g_vClipPlane;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	/* float4 : w값을 반드시 남겨야하는 이유는 w에 뷰스페이스 상의 깊이(z)를 보관하기 위해서다. */
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
};

struct VS_REFRACT_OUT
{
	/* float4 : w값을 반드시 남겨야하는 이유는 w에 뷰스페이스 상의 깊이(z)를 보관하기 위해서다. */
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    //float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
    
    float fClip : SV_ClipDistance0;
};

/* 버텍스에 대한 변환작업을 거친다.  */
/* 변환작업 : 정점의 위치에 월드, 뷰, 투영행렬을 곱한다. and 필요한 변환에 대해서 자유롭게 추가해도 상관없다 .*/
/* 버텍스의 구성 정보를 변경한다. */
VS_OUT VS_MAIN(/* 정점 */VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	/* mul : 모든(곱하기가 가능한) 행렬의 곱하기를 수행한다. */
	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vProjPos = Out.vPosition;
    
	return Out;	
}

VS_REFRACT_OUT VS_REFRACT_MAIN( /* 정점 */VS_IN In)
{
    VS_REFRACT_OUT Out = (VS_REFRACT_OUT) 0;

	/* mul : 모든(곱하기가 가능한) 행렬의 곱하기를 수행한다. */
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    //Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vProjPos = Out.vPosition;
    
    Out.fClip = dot(mul(In.vPosition, (float3x4)g_WorldMatrix), g_vClipPlane);
    
    return Out;
}

/* w나누기 연산. 진정한 투영변환. */
/* 뷰포트스페이스(윈도우좌표)로 위치를 변환한다. */
/* 래스터라이즈 : 정점에 둘러쌓인 픽셀의 정보를 생성한다. */
/* 픽셀정보는 정점정보에 기반한다. */

struct PS_IN
{
	/* Viewport */
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
};

struct PS_REFRACT_IN
{
	/* Viewport */
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    //float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
    
    float fClip : SV_ClipDistance0;
};

/* 받아온 픽셀의 정보를 바탕으로 하여 화면에 그려질 픽셀의 최종적인 색을 결정하낟. */
struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vEmissive : SV_TARGET3;
    float4 vSunMask : SV_TARGET5;
};

/* 전달받은 픽셀의 정보를 이용하여 픽셀의 최종적인 색을 결정하자. */
PS_OUT PS_MAIN(PS_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
	PS_OUT			Out = (PS_OUT)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;
	
    vector vMtrlEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); 

    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    
    Out.vEmissive.r = vMtrlEmissive.g * vMtrlEmissive.b + 0.1f;
    Out.vEmissive.gb += 0.1f;
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
	return Out;
}

PS_OUT PS_REFRACT_MAIN(PS_REFRACT_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
	PS_OUT			Out = (PS_OUT)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;
	
    vector vMtrlEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    
    Out.vEmissive.r = vMtrlEmissive.g * vMtrlEmissive.b;
    
	return Out;
}

PS_OUT PS_RIM_MAIN(PS_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    
    // Temp
    vector vLook = In.vWorldPos - g_vCamPosition;
    float3 E = normalize(-vLook);
    
    float value = saturate(dot(E, float3(In.vNormal.xyz)));
    float fEmissive = 1.0f - value;
    
    fEmissive = smoothstep(0.0f, 1.0f, fEmissive);
    fEmissive = pow(fEmissive, 3.f);
    
    Out.vEmissive = g_vMtrlEmissive * fEmissive;
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_DISSOLVE_MAIN(PS_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    //  Dissolve    
    ComputeDissolveColor(Out.vDiffuse, In.vTexcoord);
    
    return Out;
}

sampler TempSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    //AddressU = wrap;
    //AddressV = wrap;
};

PS_OUT PS_EFFECT_MAIN(PS_IN In)
{
    float2 newUVH = In.vTexcoord + float2(g_UVoffset.x, 0.f);
    float2 newUVV = In.vTexcoord + float2(g_UVoffset.y, 0.f);
    
    vector vNoise = g_NoiseTexture.Sample(LinearSampler, 2.f * newUVV);
    newUVV += vNoise;
    
    ComputeNormalMapping(In.vNormal, In.vTangent, newUVV);
	
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(TempSampler, newUVV);
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vMask = g_MaskTexture.Sample(TempSampler, In.vTexcoord + g_UVoffset);
    if (vMask.r < 0.01f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse * vMask;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    //  Dissolve    
    //ComputeDissolveColor(Out.vDiffuse, In.vTexcoord);
    clip(vNoise - g_UVoffset.y);
    
    return Out;
}

struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_SHADOW_MAIN(PS_IN In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDepth = vector(In.vProjPos.w / 2000.0f, In.vProjPos.w / 2000.0f, In.vProjPos.w / 2000.0f, 1.f);
    
    return Out;
}

PS_OUT PS_WATERFALLFOAR_MAIN(PS_IN In)
{
    float2 vNewUV = In.vTexcoord + g_UVoffset;
    //float2 vNewUV = In.vTexcoord * 0.5f + g_UVoffset;
    //vNewUV.y *= 0.5f;
    ComputeNormalMapping(In.vNormal, In.vTangent, vNewUV);
	
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vNewUV);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse += 0.2f;
    //Out.vDiffuse.b *= 1.05f;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vEmissive = 0.5f * Out.vDiffuse;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

struct PS_ALPHA_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vDistortion : SV_TARGET1;
};

PS_ALPHA_OUT PS_FLATWAVE_MAIN(PS_IN In)
{
    float2 vNewUV = In.vTexcoord + g_UVoffset;
    //float2 vNewUV = In.vTexcoord * 0.5f + g_UVoffset;
    //vNewUV.y *= 0.5f;
	
    PS_ALPHA_OUT Out = (PS_ALPHA_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, 2.f * vNewUV);

    if (vMtrlDiffuse.r < 0.01f)
        discard;
    
    Out.vColor = vMtrlDiffuse;
    Out.vColor.a = 0.5f;
    
    return Out;
}

PS_ALPHA_OUT PS_WATERSHIELD_MAIN(PS_IN In)
{
    float2 vNewUV = In.vTexcoord + g_UVoffset;
    //float2 vNewUV = In.vTexcoord * 0.5f + g_UVoffset;
    //vNewUV.y *= 0.5f;
	
    PS_ALPHA_OUT Out = (PS_ALPHA_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, 3.f * vNewUV);

    if (vMtrlDiffuse.r < 0.01f)
        discard;
    
    Out.vColor = vMtrlDiffuse;
    Out.vColor.a = 0.7f;
    Out.vDistortion = Out.vColor * 0.005f;
    
    return Out;
}

technique11 DefaultTechnique
{
	/* */
	pass Mesh
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass RimMesh
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RIM_MAIN();
        ComputeShader = NULL;
    }

    pass DissolveMesh
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_MAIN();
        ComputeShader = NULL;
    }

    pass EffectMesh
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_MAIN();
        ComputeShader = NULL;
    }

    pass RefractMesh
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
        VertexShader = compile vs_5_0 VS_REFRACT_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REFRACT_MAIN();
        ComputeShader = NULL;
    }

    pass ShadowMesh // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SHADOW_MAIN();
        ComputeShader = NULL;
    }

    pass WaterfallFoar // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WATERFALLFOAR_MAIN();
        ComputeShader = NULL;
    }

    pass FlatWave // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_FLATWAVE_MAIN();
        ComputeShader = NULL;
    }
    
    pass WaterShield // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WATERSHIELD_MAIN();
        ComputeShader = NULL;
    }
}