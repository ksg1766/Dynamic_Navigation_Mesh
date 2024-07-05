#include "Client_Shader_Defines.hlsl"
#include "Shader_Lighting_Functions.hlsl"

#define MAX_INSTANCE 500

Texture2D g_EmissiveTexture;

matrix g_WorldMatrix;
matrix g_ViewMatrix, g_ProjMatrix;

vector g_vLightEmissive = vector(1.f, 1.f, 1.f, 1.f);

vector g_vMtrlEmissive = vector(1.f, 0.843137324f, 0.f, 1.f);

vector g_vCamPosition;

float2 g_UVoffset = float2(0.f, 0.f);
Texture2D g_NoiseTexture;

// For Water
float4 g_vClipPlane;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    // Instancing
    row_major matrix matWorld : INST;
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

struct VS_REFRACT_OUT
{
	/* float4 : w값을 반드시 남겨야하는 이유는 w에 뷰스페이스 상의 깊이(z)를 보관하기 위해서다. */
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
    
    float fClip : SV_ClipDistance0;
};
    
struct VS_REFLECT_OUT
{
	/* float4 : w값을 반드시 남겨야하는 이유는 w에 뷰스페이스 상의 깊이(z)를 보관하기 위해서다. */
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
    
    float fClip : SV_ClipDistance0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(In.matWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), In.matWorld));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), In.matWorld);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), In.matWorld)).xyz;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

//VS_OUT VS_SHADOW_MAIN(VS_IN In)
//{
//    VS_OUT Out = (VS_OUT) 0;

//    matrix matWV, matWVP;

//    matWV = mul(g_WorldMatrix, g_ViewMatrix);
//    matWVP = mul(matWV, g_ProjMatrix);

//    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
//    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
//    Out.vTexcoord = In.vTexcoord;
//    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
//    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
//    Out.vProjPos = Out.vPosition;
    
//    return Out;
//}

VS_REFRACT_OUT VS_REFRACT_MAIN(VS_IN In)
{
    VS_REFRACT_OUT Out = (VS_REFRACT_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(In.matWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), In.matWorld));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), In.matWorld);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), In.matWorld)).xyz;
    Out.vProjPos = Out.vPosition;
    
    Out.fClip = dot(mul(float4(In.vPosition, 1.f), In.matWorld), g_vClipPlane);
    
    return Out;
}

VS_REFLECT_OUT VS_REFLECT_MAIN(VS_IN In)
{
    VS_REFLECT_OUT Out = (VS_REFLECT_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(In.matWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), In.matWorld));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), In.matWorld);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), In.matWorld)).xyz;
    Out.vProjPos = Out.vPosition;
    
    Out.fClip = dot(mul(float4(In.vPosition, 1.f), In.matWorld), g_vClipPlane);
    
    return Out;
}

//struct VS_WATER_OUT
//{
//    float4 vPosition : SV_POSITION;
//    float4 vNormal : NORMAL;
//    float2 vTexcoord : TEXCOORD0;
//    float4 vWorldPos : TEXCOORD1;
//    float3 vTangent : TANGENT;
//    float4 vProjPos : TEXCOORD2;
    
    
//    float4 vReflectionPos : TEXCOORD3;
//    float4 vRefractionPos : TEXCOORD4;
//};

//VS_WATER_OUT VS_WATER_MAIN(VS_IN In)
//{
//    VS_WATER_OUT Out = (VS_WATER_OUT) 0;
    
//    matrix reflectProjectWorld;
//    matrix viewProjectWorld;
    
//    matrix matWV, matWVP;

//    matWV = mul(In.matWorld, g_ViewMatrix);
//    matWVP = mul(matWV, g_ProjMatrix);

//    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
//    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), In.matWorld));
//    Out.vTexcoord = In.vTexcoord;
//    Out.vWorldPos = mul(float4(In.vPosition, 1.f), In.matWorld);
//    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), In.matWorld)).xyz;
//    Out.vProjPos = Out.vPosition;
    
//    reflectProjectWorld = mul(g_ReflectionMatrix, g_ProjMatrix);
//    reflectProjectWorld = mul(In.matWorld, reflectProjectWorld);
    
//    Out.vReflectionPos = mul(In.vPosition, reflectProjectWorld);
//    Out.vRefractionPos = mul(In.vPosition, matWVP);
    
//    return Out;
//}

struct PS_IN
{
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
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
    
    float fClip : SV_ClipDistance0;
};
    
struct PS_REFLECT_IN
{
	/* Viewport */
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
    
    float fClip : SV_ClipDistance0;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vEmissive : SV_TARGET3;
    float4 vSunMask : SV_TARGET5;
};

PS_OUT PS_MAIN(PS_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;
	
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_REFRACT_MAIN(PS_REFRACT_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;
	
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_REFLECT_MAIN(PS_REFLECT_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);

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
    fEmissive = pow(fEmissive, 2);
    
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

    ComputeDissolveColor(Out.vDiffuse, In.vTexcoord);
    
    return Out;
}

PS_OUT PS_LAVA_MAIN(PS_IN In)
{
    float2 newUV = In.vTexcoord + g_UVoffset;
    
    vector vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord + 4.f * g_UVoffset);
    float2 noise = pow(vNoise.r, 4.f);
    newUV += noise;
    
    ComputeNormalMapping(In.vNormal, In.vTangent, newUV);
	
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, newUV);

    //if (vMtrlDiffuse.a < 0.3f)
    //    discard;
	
    vector vMtrlEmissive = g_EmissiveTexture.Sample(LinearSampler, newUV);
    //if (vMtrlEmissive.r < 0.01f)
    //    vMtrlEmissive = 0.f;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);    
    Out.vEmissive = vMtrlEmissive;
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_SHADOW_MAIN(PS_IN In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
	
    //vector vMtrlDiffuse = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    //if (vMtrlDiffuse.a < 0.3f)
    //    discard;
    
    Out.vDepth = vector(In.vProjPos.w / 2000.0f, In.vProjPos.w / 2000.0f, In.vProjPos.w / 2000.0f, 1.f);
    
    return Out;
}

PS_OUT PS_CRYSTAL_MAIN(PS_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    Out.vEmissive = vector(vMtrlDiffuse.r, 0.8f * vMtrlDiffuse.gb, vMtrlDiffuse.a);
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);

    return Out;
}

struct PS_WATER_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
    
    float4 vReflectionPos : TEXCOORD3;
    float4 vRefractionPos : TEXCOORD4;
};

technique11 DefaultTechnique
{
    pass Mesh
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

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

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_MAIN();
        ComputeShader = NULL;
    }

    pass Lava
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_LAVA_MAIN();
        ComputeShader = NULL;
    }

    pass Refract
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_REFRACT_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REFRACT_MAIN();
        ComputeShader = NULL;
    }

    pass Shadow // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SHADOW_MAIN();
        ComputeShader = NULL;
    }

    pass CrystalParticle // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_CRYSTAL_MAIN();
        ComputeShader = NULL;
    }

    pass Reflect // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_REFLECT_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_REFLECT_MAIN();
        ComputeShader = NULL;
    }
}