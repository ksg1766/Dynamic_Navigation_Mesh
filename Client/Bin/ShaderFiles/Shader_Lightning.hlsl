#include "Client_Shader_Defines.hlsl"
#include "Shader_Lighting_Functions.hlsl"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float   g_fFrameTime;
float4 g_vLightningEmissive = float4(1.f, 0.f, 0.f, 1.f);

Texture2D g_LightningTexture;
Texture2D g_LightningHeadTexture;

float g_fEmissivePower = 1.f;

vector g_vCamPosition;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

struct VS_OUT
{	
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    vector vRight = g_WorldMatrix._11_12_13_14;
    vector vUp = g_WorldMatrix._21_22_23_24;
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPSize = float2(In.vPSize.x * length(vRight), In.vPSize.y * length(vUp));
    
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    
    float4 vNormal : NORMAL;
    float4 vProjPos : TEXCOORD1;
};

[maxvertexcount(20)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];

    vector vLook = g_vCamPosition - In[0].vPosition;
    vLook.y = 0.f;
    
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight.xyz)) * In[0].vPSize.y * 0.5f;

    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = vector(In[0].vPosition.xyz + vRight + 20.f * vUp, 1.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.0f, 0.f);
    Out[0].vNormal = vLook;
    Out[0].vProjPos = Out[0].vPosition;
    
    Out[1].vPosition = vector(In[0].vPosition.xyz - vRight + 20.f * vUp, 1.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.0f, 0.f);
    Out[1].vNormal = vLook;
    Out[1].vProjPos = Out[1].vPosition;
    
    Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.0f, 1.0f);
    Out[2].vNormal = vLook;
    Out[2].vProjPos = Out[2].vPosition;
    
    Out[3].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.0f, 1.0f);
    Out[3].vNormal = vLook;
    Out[3].vProjPos = Out[3].vPosition;
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();

    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

[maxvertexcount(20)]
void GS_SAPRK_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];

    vector vLook = g_vCamPosition - In[0].vPosition;
    vLook.y = 0.f;
    
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight.xyz)) * In[0].vPSize.y * 0.5f;

    vRight *= 2.7f;
    
    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = vector(In[0].vPosition.xyz + vRight + 6.f * vUp, 1.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.0f, 0.f);
    Out[0].vNormal = vLook;
    Out[0].vProjPos = Out[0].vPosition;
    
    Out[1].vPosition = vector(In[0].vPosition.xyz - vRight + 6.f * vUp, 1.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.0f, 0.f);
    Out[1].vNormal = vLook;
    Out[1].vProjPos = Out[1].vPosition;
    
    Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.0f, 1.0f);
    Out[2].vNormal = vLook;
    Out[2].vProjPos = Out[2].vPosition;
    
    Out[3].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.0f, 1.0f);
    Out[3].vNormal = vLook;
    Out[3].vProjPos = Out[3].vPosition;
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();

    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    
    float4 vNormal : NORMAL;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vEmissive : SV_TARGET3;
    
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vNewUV = float2(In.vTexcoord.x, In.vTexcoord.y + g_fFrameTime);
    float3 vColor = g_LightningTexture.Sample(LinearSampler, vNewUV);
    
    if(vColor.r < 0.5f)
        discard;
    
    vColor = 1.f - vColor;
    vColor.bg = 1.f - (0.8f * vColor.r);
    vColor.r = 1.f;
    vColor.b *= 1.2f;
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    
    Out.vDiffuse = float4(vColor, 0.7f);
    Out.vEmissive = float4(vColor, 1.f);
    
    return Out;
}

PS_OUT PS_SPARK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    float3 vColor = g_LightningTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vColor.r < 0.4f)
        discard;
    
    vColor = 1.f - vColor;
    vColor.bg = 1.f - (0.8f * vColor.r);
    vColor.r = 1.f;
    vColor.g = 0.83f;
    vColor.b = 0.83f;
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    
    Out.vDiffuse = float4(vColor, 1.f);
    Out.vEmissive = float4(1.1f * vColor, 1.f);
    
    return Out;
}

PS_OUT PS_WATERLIGHTNING_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vNewUV = float2(In.vTexcoord.x, In.vTexcoord.y + g_fFrameTime);
    float3 vColor = g_LightningTexture.Sample(LinearSampler, vNewUV);
    
    if (vColor.b * g_fEmissivePower * 1.6f < 0.5f)
        discard;
    
    vColor = 1.f - vColor;
    vColor.rg = 1.f - (0.8f * vColor.b);
    vColor.b = 1.5f;
    vColor.g *= 0.7f;
    vColor.r *= 0.3f;
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    
    Out.vDiffuse = float4(vColor, 1.f);
    Out.vEmissive = float4(g_fEmissivePower * vColor, 1.f);
    
    return Out;
}

PS_OUT PS_WATERSPARK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    float3 vColor = g_LightningTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vColor.b < 0.35f)
        discard;
    
    vColor = 1.f - vColor;
    vColor.rg = 1.f - (0.8f * vColor.b);
    vColor.b = 1.5f;
    vColor.g *= 0.7f;
    vColor.r *= 0.3f;
    //vColor.r = 0.7f;
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    
    Out.vDiffuse = float4(vColor, 1.f);
    Out.vEmissive = float4(g_fEmissivePower * vColor, 1.f);
    
    return Out;
} 

technique11 DefaultTechnique
{
	pass Lightning // 0
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass LightningSpark // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_SAPRK_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SPARK_MAIN();
        ComputeShader = NULL;
    }

    pass WaterLightning // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WATERLIGHTNING_MAIN();
        ComputeShader = NULL;
    }

    pass WaterLightningSpark // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_SAPRK_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WATERSPARK_MAIN();
        ComputeShader = NULL;
    }
}




