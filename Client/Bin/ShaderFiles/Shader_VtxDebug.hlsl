#include "Client_Shader_Defines.hlsl"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    In.vPosition.y -= 3.f;
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_WHITE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = float4(1.f, 1.f, 1.f, 1.f);

    return Out;
}

technique11 DefaultTechnique
{
    pass WHITE
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WHITE();
        ComputeShader = NULL;
    }
}




