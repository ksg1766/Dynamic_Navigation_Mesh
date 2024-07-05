#include "Client_Shader_Defines.hlsl"
#include "Shader_Lighting_Functions.hlsl"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float   g_fFrameTime;
float3 g_vScrollSpeeds = float3(1.3f, 2.1f, 2.3f);
float3 g_vScales = float3(1.0f, 2.0f, 3.0f);

Texture2D g_FireTexture;
Texture2D g_NoiseTexture;
Texture2D g_AlphaTexture;

float2 g_vDistortion1 = float2(0.1f, 0.2f);
float2 g_vDistortion2 = float2(0.1f, 0.3f);
float2 g_vDistortion3 = float2(0.1f, 0.1f);
float g_fDistortionScale = 0.8f;
float g_fDistortionBias = 0.5f;

vector g_vCamPosition;

sampler Sampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

// For.SocketFire
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
matrix g_matOffset;

int g_iSocketBoneIndex;

matrix GetSocketMatrix()
{
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
    
    if (g_Tweenframes.next.animIndex >= 0)
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
        result = lerp(result, nextResult, g_Tweenframes.tweenRatio);
    }

    return result;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    
    //row_major matrix matWorld : INST;
};

struct VS_OUT
{	
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Matrix m = GetSocketMatrix();

    vector vRight = g_WorldMatrix._11_12_13_14;
    vector vUp = g_WorldMatrix._21_22_23_24;
    //vector vRight = In.matWorld._11_12_13_14;
    //vector vUp = In.matWorld._21_22_23_24;

    In.vPosition = mul(float4(In.vPosition, 1.f), m);
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPSize = float2(In.vPSize.x * length(vRight), In.vPSize.y * length(vUp));
    
    return Out;
}

VS_OUT VS_SOCKET_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Matrix m = GetSocketMatrix();
    m = mul(g_matOffset, m);

    vector vRight = g_WorldMatrix._11_12_13_14;
    vector vUp = g_WorldMatrix._21_22_23_24;

    In.vPosition = mul(vector(In.vPosition, 1.f), m);
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPSize = (2.5f - 0.5f * g_matOffset._42) * float2(In.vPSize.x * length(vRight), In.vPSize.y * length(vUp));
    
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
    float2 vNoiseUV1 : TEXCOORD1;
    float2 vNoiseUV2 : TEXCOORD2;
    float2 vNoiseUV3 : TEXCOORD3;
};

[maxvertexcount(20)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];

    vector vLook = g_vCamPosition - In[0].vPosition;

    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight.xyz)) * In[0].vPSize.y * 0.5f;

    matrix matVP;

    matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = vector(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.0f, 0.f);

    Out[1].vPosition = vector(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.0f, 0.f);
    
    Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.0f, 1.0f);
    
    Out[3].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.0f, 1.0f);
    
    for (int i = 0; i < 4; ++i)
    {
        Out[i].vNoiseUV1 = (Out[i].vTexcoord * g_vScales.x);
        Out[i].vNoiseUV1.y = Out[i].vNoiseUV1.y + (g_fFrameTime * g_vScrollSpeeds.x * (1.f + g_matOffset._42 / 10.f));
    
        Out[i].vNoiseUV2 = (Out[i].vTexcoord * g_vScales.y);
        Out[i].vNoiseUV2.y = Out[i].vNoiseUV2.y + (g_fFrameTime * g_vScrollSpeeds.y * (1.f + g_matOffset._42 / 10.f));
    
        Out[i].vNoiseUV3 = (Out[i].vTexcoord * g_vScales.z);
        Out[i].vNoiseUV3.y = Out[i].vNoiseUV3.y + (g_fFrameTime * g_vScrollSpeeds.z * (1.f + g_matOffset._42 / 10.f));
    }
    
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
    float2 vNoiseUV1 : TEXCOORD1;
    float2 vNoiseUV2 : TEXCOORD2;
    float2 vNoiseUV3 : TEXCOORD3;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vNoise1;
    float4 vNoise2;
    float4 vNoise3;
    float4 vFinalNoise;
    float  fPerturb;
    float2 vNoiseCoords;
    float4 vFireColor;
    float4 vAlphaColor;
    
    // 동일한 노이즈 텍스쳐를 서로 다른 세 텍스쳐 좌표를 사용하여 세 개의 다른 크기의 노이즈를 얻습니다.
    vNoise1 = g_NoiseTexture.Sample(LinearSampler, In.vNoiseUV1);
    vNoise2 = g_NoiseTexture.Sample(LinearSampler, In.vNoiseUV2);
    vNoise3 = g_NoiseTexture.Sample(LinearSampler, In.vNoiseUV3);

    // 노이즈 값의 범위를 (0, 1)에서 (-1, +1)이 되도록 합니다.
    vNoise1 = (vNoise1 - 0.5f) * 2.0f;
    vNoise2 = (vNoise2 - 0.5f) * 2.0f;
    vNoise3 = (vNoise3 - 0.5f) * 2.0f;
    
     // 노이즈의 x와 y값을 세 개의 다른 왜곡 x및 y좌표로 흩뜨립니다.
    vNoise1.xy = vNoise1.xy * g_vDistortion1.xy;
    vNoise2.xy = vNoise2.xy * g_vDistortion2.xy;
    vNoise3.xy = vNoise3.xy * g_vDistortion3.xy;

    // 왜곡된 세 노이즈 값들을 하나의 노이즈로 함성합니다.
    vFinalNoise = vNoise1 + vNoise2 + vNoise3;
    
     // 입력으로 들어온 텍스쳐의 Y좌표를 왜곡 크기와 바이어스 값으로 교란시킵니다.
    // 이 교란은 텍스쳐의 위쪽으로 갈수록 강해져서 맨 위쪽에는 깜박이는 효과를 만들어냅니다.
    fPerturb = ((1.0f - In.vTexcoord.y) * g_fDistortionScale) + g_fDistortionBias;

    // 불꽃 색상 텍스쳐를 샘플링하는데 사용될 왜곡 및 교란된 텍스쳐 좌표를 만듭니다.
    vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTexcoord.xy;
    
     // 왜곡되고 교란된 텍스쳐 좌표를 이용하여 불꽃 텍스쳐에서 색상을 샘플링합니다.
    // wrap를 사용하는 스테이트 대신 clamp를 사용하는 스테이트를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지합니다.
    vFireColor = g_FireTexture.Sample(Sampler, vNoiseCoords.xy);

    // 왜곡되고 교란된 텍스쳐 좌표를 이용하여 알파 텍스쳐에서 알파값을 샘플링합니다.
    // 불꽃의 투명도를 지정하는 데 사용될 것입니다.
    // wrap를 사용하는 스테이트 대신 clamp를 사용하는 스테이트를 사용하여 불꽃 텍스쳐가 래핑되는 것을 방지합니다.
    vAlphaColor = g_AlphaTexture.Sample(Sampler, vNoiseCoords.xy);
    if (vAlphaColor.r == 0.f)
        discard;
    
    // 왜곡 및 교란된 알파 텍스쳐 값을 알파블렌딩에 사용합니다.
        
    vFireColor.a = vAlphaColor;
    
    Out.vColor = vFireColor;
    
    return Out;
}

technique11 DefaultTechnique
{
	pass Fire
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }

    pass SocketFire
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SOCKET_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;
    }
}




