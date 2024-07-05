#include "Client_Shader_Defines.hlsl"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
};

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN( /* 정점 */VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

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

PS_OUT PS_RED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = float4(1.f, 0.f, 0.f, 1.f);

    return Out;
}

PS_OUT PS_GREEN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = float4(0.f, 1.f, 0.f, 1.f);

    return Out;
}

PS_OUT PS_BLUE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = float4(0.f, 0.f, 1.f, 1.f);

    return Out;
}

PS_OUT PS_WHITE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = float4(1.f, 1.f, 1.f, 1.f);

    return Out;
}

technique11 DefaultTechnique
{
    // ...요렇게 하지말고 g_vColor값 받아서 하자.
    pass RED
    {
        SetRasterizerState(RS_Wireframe);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_RED();
        ComputeShader = NULL;
    }

    pass GREEN
    {
        SetRasterizerState(RS_Wireframe);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_GREEN();
        ComputeShader = NULL;
    }

    pass BLUE
    {
        SetRasterizerState(RS_Wireframe);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUE();
        ComputeShader = NULL;
    }

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




