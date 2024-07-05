#include "Client_Shader_Defines.hlsl"
#include "Shader_Lighting_Functions.hlsl"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightPos = vector(0.f, 0.f, 0.f, 1.f);
float g_fLightRange = 0.f;
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightEmissive = vector(1.f, 1.f, 1.f, 1.f);

vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
//vector g_vMtrlEmissive = vector(1.f, 0.f, 0.f, 1.f);
vector g_vMtrlEmissive = vector(1.f, 0.843137324f, 0.f, 1.f);

float g_fFrameTime;

vector g_vCamPosition;

vector g_ColorOffset;

Texture2D g_DepthTarget;

struct tagKeyframeDesc
{
    int animIndex;
    uint currFrame;
    uint nextFrame;
    float ratio;
    float sumTime;
    float speed;
    float2 padding;
};

struct tagTweenFrameDesc
{
    float tweenDuration;
    float tweenRatio;
    float tweenSumTime;
    float padding;
    tagKeyframeDesc curr;
    tagKeyframeDesc next;
};

tagTweenFrameDesc g_Tweenframes;

Texture2DArray g_TransformMap;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
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

matrix GetAnimationMatrix(VS_IN input)
{
    float indices[4] = { input.vBlendIndex.x, input.vBlendIndex.y, input.vBlendIndex.z, input.vBlendIndex.w };
    float weights[4] = { input.vBlendWeight.x, input.vBlendWeight.y, input.vBlendWeight.z, input.vBlendWeight.w };

    int animIndex[2];
    int currFrame[2];
    int nextFrame[2];
    float ratio[2];
    
    animIndex[0] = g_Tweenframes.curr.animIndex;
    currFrame[0] = g_Tweenframes.curr.currFrame;
    nextFrame[0] = g_Tweenframes.curr.nextFrame;
    ratio[0] = g_Tweenframes.curr.ratio;
    
    animIndex[1] = g_Tweenframes.next.animIndex;
    currFrame[1] = g_Tweenframes.next.currFrame;
    nextFrame[1] = g_Tweenframes.next.nextFrame;
    ratio[1] = g_Tweenframes.next.ratio;
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix curr = 0;
    matrix next = 0;
    matrix transform = 0;

    for (int i = 0; i < 4; i++)
    {
        c0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
        c1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
        c2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));
        c3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0));
        curr = matrix(c0, c1, c2, c3);
        
        n0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
        n1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
        n2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
        n3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
        next = matrix(n0, n1, n2, n3);
        
        matrix result = lerp(curr, next, ratio[0]);
        
        if (g_Tweenframes.next.animIndex >= 0)
        {
            c0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
            c1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
            c2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
            c3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
            curr = matrix(c0, c1, c2, c3);

            n0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
            n1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
            n2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
            n3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
            next = matrix(n0, n1, n2, n3);

            matrix nextResult = lerp(curr, next, ratio[1]);
            result = lerp(result, nextResult, g_Tweenframes.tweenRatio);
        }
        
        transform += mul(weights[i], result);
    }

    return transform;
}

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    matrix m = GetAnimationMatrix(In);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), m);
    vector vNormal = mul(vector(In.vNormal, 0.f), m);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vProjPos = Out.vPosition;
    
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
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vEmissive : SV_TARGET3;
    float4 vSunMask : SV_TARGET5;
};

struct PS_OUT_BLUE
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vEmissive : SV_TARGET3;
    float4 vBlue : SV_TARGET4;
    float4 vSunMask : SV_TARGET5;
};
    
struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
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
     // Rim Light
    float3 E = normalize(-vLook);

    float value = saturate(dot(E, float3(In.vNormal.xyz)));
    float fEmissive = 1.0f - value;

	// min, max, x
    fEmissive = smoothstep(0.0f, 1.0f, fEmissive);
    fEmissive = pow(fEmissive, 2);
	//
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
    
    ComputeDissolveColor(Out.vDiffuse, In.vTexcoord);

    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

struct PS_ALPHA_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vDistortion : SV_TARGET1;
};

PS_ALPHA_OUT PS_ALPHA_MAIN(PS_IN In)
{
    PS_ALPHA_OUT Out = (PS_ALPHA_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    // 뭐야 이게 왜 있어
    //vector vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) +
	//	g_vLightAmbient * g_vMtrlAmbient;

    Out.vDiffuse = g_vLightDiffuse * vMtrlDiffuse;
    
    Out.vDiffuse += g_ColorOffset;
    Out.vDiffuse = pow(Out.vDiffuse, 2.2f);
    Out.vDiffuse.a = 0.75f;
    
    return Out;
}

PS_OUT_BLUE PS_BLUE_MAIN(PS_IN In)
{
    PS_OUT_BLUE Out = (PS_OUT_BLUE) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    Out.vBlue = vector(0.f, In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f);
    
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_SHADOW PS_SHADOW_MAIN(PS_IN In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
	
    //vector vMtrlDiffuse = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    //if (vMtrlDiffuse.a < 0.3f)
    //    discard;
    
    Out.vDepth = vector(In.vProjPos.w / 2000.0f, In.vProjPos.w / 2000.0f, In.vProjPos.w / 2000.0f, 1.f);
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.z / In.vProjPos.w, In.vProjPos.z / In.vProjPos.w, 1.f);
    
    return Out;
}

PS_OUT PS_WAVE_MAIN(PS_IN In)
{
    float2 vNewUV = float2(In.vTexcoord.x/* - g_fFrameTime*/, In.vTexcoord.y + 0.25f * g_fFrameTime);
    float4 vColor = g_DiffuseTexture.Sample(LinearSampler, vNewUV);
    
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_OUT Out = (PS_OUT) 0;

    //if (vColor.a < 0.001f)
        //discard;
    
    //Out.vDiffuse = vColor * 1.5f;    
    Out.vDiffuse.rgb = vColor;
    Out.vDiffuse.a = 0.7f;
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_PLAYERRIM_MAIN(PS_IN In)
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
     // Rim Light
    float3 E = normalize(-vLook);

    float value = saturate(dot(E, float3(In.vNormal.xyz)));
    float fEmissive = 1.0f - value;

	// min, max, x
    fEmissive = smoothstep(0.0f, 1.0f, fEmissive);
    fEmissive = pow(fEmissive, 2);
	//
    Out.vEmissive = float4(1.f, 0.f, 0.f, 1.f) * fEmissive;
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Mesh // 0
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

    pass RimMesh // 1
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

    pass DissolveMesh // 2
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

    pass AlphaMesh // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_ALPHA_MAIN();
    }

    pass BlueMesh // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUE_MAIN();
        ComputeShader = NULL;
    }

    pass ShadowMesh // 5
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

    pass WaveMesh // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        //SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WAVE_MAIN();
        ComputeShader = NULL;
    }

    pass PlayerRim // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        //SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PLAYERRIM_MAIN();
        ComputeShader = NULL;
    }
}