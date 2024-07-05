#include "Client_Shader_Defines.hlsl"
#include "Shader_Lighting_Functions.hlsl"

/* ������̺�. */
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
	/* float4 : w���� �ݵ�� ���ܾ��ϴ� ������ w�� �佺���̽� ���� ����(z)�� �����ϱ� ���ؼ���. */
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
};

struct VS_REFRACT_OUT
{
	/* float4 : w���� �ݵ�� ���ܾ��ϴ� ������ w�� �佺���̽� ���� ����(z)�� �����ϱ� ���ؼ���. */
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    //float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
    
    float fClip : SV_ClipDistance0;
};

/* ���ؽ��� ���� ��ȯ�۾��� ��ģ��.  */
/* ��ȯ�۾� : ������ ��ġ�� ����, ��, ��������� ���Ѵ�. and �ʿ��� ��ȯ�� ���ؼ� �����Ӱ� �߰��ص� ������� .*/
/* ���ؽ��� ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN(/* ���� */VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	/* mul : ���(���ϱⰡ ������) ����� ���ϱ⸦ �����Ѵ�. */
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

VS_REFRACT_OUT VS_REFRACT_MAIN( /* ���� */VS_IN In)
{
    VS_REFRACT_OUT Out = (VS_REFRACT_OUT) 0;

	/* mul : ���(���ϱⰡ ������) ����� ���ϱ⸦ �����Ѵ�. */
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

/* w������ ����. ������ ������ȯ. */
/* ����Ʈ�����̽�(��������ǥ)�� ��ġ�� ��ȯ�Ѵ�. */
/* �����Ͷ����� : ������ �ѷ����� �ȼ��� ������ �����Ѵ�. */
/* �ȼ������� ���������� ����Ѵ�. */

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

/* �޾ƿ� �ȼ��� ������ �������� �Ͽ� ȭ�鿡 �׷��� �ȼ��� �������� ���� �����ϳ�. */
struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vEmissive : SV_TARGET3;
    float4 vSunMask : SV_TARGET5;
};

/* ���޹��� �ȼ��� ������ �̿��Ͽ� �ȼ��� �������� ���� ��������. */
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

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
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

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
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

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
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

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
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

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
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

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
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

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
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

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
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

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WATERSHIELD_MAIN();
        ComputeShader = NULL;
    }
}