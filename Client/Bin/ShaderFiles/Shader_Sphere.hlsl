#include "Client_Shader_Defines.hlsl"
#include "Shader_Lighting_Functions.hlsl"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float   g_fFrameTime;
float   g_fEmissivePower = 1.f;

vector g_vCamPosition;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{	
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* mul : 모든(곱하기가 가능한) 행렬의 곱하기를 수행한다. */
    matrix matWV, matWVP;

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

VS_OUT VS_BUBBLE_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* mul : 모든(곱하기가 가능한) 행렬의 곱하기를 수행한다. */
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    In.vPosition *= (30.f * g_fFrameTime);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct VS_BOLTS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_BOLTS_OUT VS_BOLTS_MAIN(VS_IN In)
{
    VS_BOLTS_OUT Out = (VS_BOLTS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vDistortion : SV_TARGET1;
};
    
struct PS_WAVE_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vDistortion : SV_TARGET1;
};

PS_OUT PS_BUBBLE_MAIN(PS_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_OUT Out = (PS_OUT) 0;
	
    float4 vRedColor = float4(1.f, 0.2f, 0.25f, 1.f);
    
    vector vLook = In.vWorldPos - g_vCamPosition;
    float3 E = normalize(-vLook);
    
    float value = saturate(dot(E, float3(In.vNormal.xyz)));
    float fEmissive = 1.0f - value;
    
    fEmissive = smoothstep(0.0f, 1.0f, fEmissive);
    fEmissive = pow(fEmissive, 2 + 10.f * g_fFrameTime);
    
    Out.vColor = vRedColor * fEmissive;
    Out.vDistortion = 5.f * Out.vColor.x;
    
    return Out;
}

sampler Sampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    //AddressV = wrap;
};

PS_WAVE_OUT PS_WAVERING_MAIN(PS_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_WAVE_OUT Out = (PS_WAVE_OUT) 0;
    
    In.vTexcoord.x *= 5.f;
    In.vTexcoord.y += (3.f * g_fFrameTime - 0.7f);
    float3 vColor = g_DiffuseTexture.Sample(Sampler, In.vTexcoord);
    
    if (vColor.r == 0.f)
        discard;
    
    vColor = 1.f - vColor;
    //vColor = pow(vColor, 4.f);
    //vColor.bg = 1.f - (0.8f * vColor.r);
    vColor.r = 1.f;
    vColor.b += 0.1f;
    //vColor.b *= 1.2f;
    
    clamp(vColor.b, 0.f, 1.f);
    
    Out.vColor = float4(vColor, (0.55f - 2.f * g_fFrameTime) * vColor.g);
    Out.vDistortion = 5.f * Out.vColor.x;
    
    return Out;
}

struct PS_BOLTS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_BOLTS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vEmissive : SV_TARGET3;
};

PS_BOLTS_OUT PS_WATERBOLTS_MAIN(PS_BOLTS_IN In)
{
    PS_BOLTS_OUT Out = (PS_BOLTS_OUT) 0;
    
    float2 vNewUV = float2(In.vTexcoord.x, In.vTexcoord.y + g_fFrameTime);
    float3 vColor = g_DiffuseTexture.Sample(LinearSampler, vNewUV);
    
    if (vColor.b == 0.f)
        discard;
    
    if (vColor.b * g_fEmissivePower * 1.6f < 0.5f)
        discard;
    
    vColor = 1.f - vColor;
    vColor.rg = 1.f - (0.8f * vColor.b);
    vColor.b = 1.5f;
    vColor.g *= 0.7f;
    vColor.r *= 0.3f;
    
    Out.vDiffuse = float4(vColor, 1.f);
    Out.vEmissive = float4(g_fEmissivePower * vColor, 1.f);
    
    return Out;
}

PS_BOLTS_OUT PS_LIGHTNINGSPARK_MAIN(PS_BOLTS_IN In)
{
    PS_BOLTS_OUT Out = (PS_BOLTS_OUT) 0;
    
    float3 vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vColor.b == 0.f)
        discard;
    
    //if (vColor.b * g_fEmissivePower * 1.6f < 0.3f)
    //    discard;
    
    vColor = 1.f - vColor;
    vColor.rg = 1.f - (0.8f * vColor.b);
    vColor.b = 1.2f;
    vColor.rg *= 1.9f;
    
    Out.vDiffuse = float4(vColor, 1.f);
    Out.vEmissive = float4(g_fEmissivePower * vColor, 1.f);
    
    return Out;
}

struct PS_ORB_OUT
{
    float4 vDiffuse : SV_TARGET0;
    //float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vEmissive : SV_TARGET3;
    float4 vSunMask : SV_TARGET5;
};

PS_ORB_OUT PS_ORB_MAIN(PS_IN In)
{
    PS_ORB_OUT Out = (PS_ORB_OUT) 0;
    
    float2 vNewUV = float2(In.vTexcoord.x - g_fFrameTime, In.vTexcoord.y + 0.73f * g_fFrameTime);
    
    float3 vColor = g_DiffuseTexture.Sample(LinearSampler, vNewUV);
    
    Out.vDiffuse = float4(0.f, 0.f, 0.03f, 1.f);
   
    if (vColor.r > 0.77f)
        Out.vEmissive = float4(vColor.b * float3(0.5f, 0.93f, 2.f), 1.f);
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_ORB_OUT PS_WISP_MAIN(PS_IN In)
{
    PS_ORB_OUT Out = (PS_ORB_OUT) 0;
   
    float2 vNewUV = In.vTexcoord + 2.f * g_fFrameTime;
    
    float3 vNoiseSample = g_DiffuseTexture.Sample(LinearSampler, vNewUV);
   
    if (vNoiseSample.r > 0.67f + 0.5f * g_fFrameTime)
    {
        Out.vDiffuse = float4(0.f, 0.77f, 1.f, 1.f);
        Out.vEmissive = float4(0.5f, 0.93f, 2.f, 1.f);
    }
    else
        discard;
    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_SPHERESWIRL_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    float2 vNewUV = In.vTexcoord + g_fFrameTime;
    
    float4 vNoiseSample = g_DiffuseTexture.Sample(LinearSampler, vNewUV);
   
    Out.vColor = vNoiseSample;
    Out.vDistortion = Out.vColor;
    Out.vColor.a = 0.15f;
    
    return Out;
}

struct PS_ORBCHARGE_OUT
{
    float4 vDistortion : SV_TARGET1;
};

PS_ORBCHARGE_OUT PS_ORBCHARGE_MAIN(PS_IN In)
{
    PS_ORBCHARGE_OUT Out = (PS_ORBCHARGE_OUT) 0;
    
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
    
    float4 vColor = float4(0.1f, 0.1f, 0.1f, 1.f);
    
    vector vLook = In.vWorldPos - g_vCamPosition;
    float3 E = normalize(-vLook);
    
    float value = saturate(dot(E, float3(In.vNormal.xyz)));
    float fEmissive = 1.0f - value;
    
    fEmissive = smoothstep(0.0f, 1.0f, fEmissive);
    //fEmissive = pow(fEmissive, 2.f);
    
    Out.vDistortion = vColor * fEmissive;
    
    return Out;
}

technique11 DefaultTechnique
{
	pass Bubble
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        //SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_BUBBLE_MAIN();
        GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BUBBLE_MAIN();
        ComputeShader = NULL;
    }

    pass WaveRing
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WAVERING_MAIN();
        ComputeShader = NULL;
    }

    pass WaterBolts // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_BOLTS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WATERBOLTS_MAIN();
        ComputeShader = NULL;
    }

    pass LightningSpark // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_BOLTS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_LIGHTNINGSPARK_MAIN();
        ComputeShader = NULL;
    }

    pass Orb // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ORB_MAIN();
        ComputeShader = NULL;
    }

    pass Wisp // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WISP_MAIN();
        ComputeShader = NULL;
    }

    pass SphereSwirl // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SPHERESWIRL_MAIN();
        ComputeShader = NULL;
    }

    pass OrbCharge // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ORBCHARGE_MAIN();
        ComputeShader = NULL;
    }
}




