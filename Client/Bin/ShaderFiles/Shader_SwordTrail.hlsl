#include "Client_Shader_Defines.hlsl"
#include "Shader_Lighting_Functions.hlsl"

matrix g_WorldMatrix;
matrix g_PreWorldMatrix;

matrix g_ViewMatrix, g_ProjMatrix;
Texture2D g_AlphaTexture;

Texture2DArray g_TransformMap;

matrix g_matOffsetTop;
matrix g_matOffsetBottom;
int g_iSocketBoneIndex;

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
tagTweenFrameDesc g_preTweenframes;

matrix GetSocketMatrix(tagTweenFrameDesc tTweenFrames)
{
    int animIndex[2];
    int currFrame[2];
    int nextFrame[2];
    float ratio[2];
    
    animIndex[0] = tTweenFrames.curr.animIndex;
    currFrame[0] = tTweenFrames.curr.currFrame;
    nextFrame[0] = tTweenFrames.curr.nextFrame;
    ratio[0] = tTweenFrames.curr.ratio;
    
    animIndex[1] = tTweenFrames.next.animIndex;
    currFrame[1] = tTweenFrames.next.currFrame;
    nextFrame[1] = tTweenFrames.next.nextFrame;
    ratio[1] = tTweenFrames.next.ratio;
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix curr = 0;
    matrix next = 0;

    c0 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 0, currFrame[0], animIndex[0], 0)); // indices[i] 위치에 SocketBoneNumber를 던지자
    c1 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 1, currFrame[0], animIndex[0], 0)); // for문 순회 필요는 없다. 정점과 달리 소켓은 한뼈에만 영향을 받는다.
    c2 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 2, currFrame[0], animIndex[0], 0));
    c3 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 3, currFrame[0], animIndex[0], 0));
    curr = matrix(c0, c1, c2, c3);
    
    n0 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 0, nextFrame[0], animIndex[0], 0));
    n1 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 1, nextFrame[0], animIndex[0], 0));
    n2 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 2, nextFrame[0], animIndex[0], 0));
    n3 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 3, nextFrame[0], animIndex[0], 0));
    next = matrix(n0, n1, n2, n3);
    
    matrix result = lerp(curr, next, ratio[0]);
    
    if (tTweenFrames.next.animIndex >= 0)
    {
        c0 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 0, currFrame[1], animIndex[1], 0));
        c1 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 1, currFrame[1], animIndex[1], 0));
        c2 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 2, currFrame[1], animIndex[1], 0));
        c3 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 3, currFrame[1], animIndex[1], 0));
        curr = matrix(c0, c1, c2, c3);

        n0 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 0, nextFrame[1], animIndex[1], 0));
        n1 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 1, nextFrame[1], animIndex[1], 0));
        n2 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 2, nextFrame[1], animIndex[1], 0));
        n3 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 3, nextFrame[1], animIndex[1], 0));
        next = matrix(n0, n1, n2, n3);

        matrix nextResult = lerp(curr, next, ratio[1]);
        result = lerp(result, nextResult, tTweenFrames.tweenRatio);
    }

    return result;
}

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = float4(In.vPosition, 1.f);

    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

[maxvertexcount(20)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix matPreWV, matPreWVP;

    matPreWV = mul(g_PreWorldMatrix, g_ViewMatrix);
    matPreWVP = mul(matPreWV, g_ProjMatrix);

    Matrix mCur = GetSocketMatrix(g_Tweenframes);
    Matrix mCurTop = mul(g_matOffsetTop, mCur);
    Matrix mCurBottom = mul(g_matOffsetBottom, mCur);
    
    Matrix mPre = GetSocketMatrix(g_preTweenframes);
    Matrix mPreTop = mul(g_matOffsetTop, mPre);
    Matrix mPreBottom = mul(g_matOffsetBottom, mPre);
    
    Out[0].vPosition = mul(In[0].vPosition, mPreTop);
    Out[0].vPosition = mul(Out[0].vPosition, matPreWVP);
    Out[0].vTexcoord = float2(0.0f, 0.0f);

    Out[1].vPosition = mul(In[0].vPosition, mCurTop);
    Out[1].vPosition = mul(Out[1].vPosition, matWVP);
    Out[1].vTexcoord = float2(1.0f, 0.0f);
    
    Out[2].vPosition = mul(In[0].vPosition, mCurBottom);
    Out[2].vPosition = mul(Out[2].vPosition, matWVP);
    Out[2].vTexcoord = float2(1.0f, 1.0f);
    
    Out[3].vPosition = mul(In[0].vPosition, mPreBottom);
    Out[3].vPosition = mul(Out[3].vPosition, matPreWVP);
    Out[3].vTexcoord = float2(0.0f, 1.f);
    
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
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vDistortion : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    In.vTexcoord.x = 0.645f;
    float4 vAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord);
    //Out.vDiffuse.r = 0.7f;
    //Out.vDiffuse.a = 0.3f;
    
    //if (vDiffuse.r < 0.5f)
    //    discard;
    // vector vAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord);
    
    // if (vTextureAlpha.r <= 0.01f || (vTextureDiffuse.r <= 0.001f && vTextureDiffuse.g <= 0.001f && vTextureDiffuse.b <= 0.001f))
    //      discard;
    
    //if (vAlpha.r < 0.01f)
    //    discard;
    Out.vDiffuse.r = 0.5f * vAlpha;
    Out.vDistortion = vAlpha;
    Out.vDiffuse.a = 0.7f;
    
    return Out;
};

RasterizerState RS_Custom
{
    FillMode = Solid;

	/* 앞면을 컬링하겠다. == 후면을 보여주겠다. */
    CullMode = None;

	/* 앞면을 시계방향으로 쓰겠다. */
    FrontCounterClockwise = false;
};

technique11 DefaultTechnique
{
    pass SwordTrail
    {
        SetRasterizerState(RS_Custom);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }
}