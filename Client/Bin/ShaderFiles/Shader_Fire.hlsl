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

    c0 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 0, currFrame[0], animIndex[0], 0)); // indices[i] ��ġ�� SocketBoneNumber�� ������
    c1 = g_TransformMap.Load(int4(g_iSocketBoneIndex * 4 + 1, currFrame[0], animIndex[0], 0)); // for�� ��ȸ �ʿ�� ����. ������ �޸� ������ �ѻ����� ������ �޴´�.
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
    
    // ������ ������ �ؽ��ĸ� ���� �ٸ� �� �ؽ��� ��ǥ�� ����Ͽ� �� ���� �ٸ� ũ���� ����� ����ϴ�.
    vNoise1 = g_NoiseTexture.Sample(LinearSampler, In.vNoiseUV1);
    vNoise2 = g_NoiseTexture.Sample(LinearSampler, In.vNoiseUV2);
    vNoise3 = g_NoiseTexture.Sample(LinearSampler, In.vNoiseUV3);

    // ������ ���� ������ (0, 1)���� (-1, +1)�� �ǵ��� �մϴ�.
    vNoise1 = (vNoise1 - 0.5f) * 2.0f;
    vNoise2 = (vNoise2 - 0.5f) * 2.0f;
    vNoise3 = (vNoise3 - 0.5f) * 2.0f;
    
     // �������� x�� y���� �� ���� �ٸ� �ְ� x�� y��ǥ�� ��߸��ϴ�.
    vNoise1.xy = vNoise1.xy * g_vDistortion1.xy;
    vNoise2.xy = vNoise2.xy * g_vDistortion2.xy;
    vNoise3.xy = vNoise3.xy * g_vDistortion3.xy;

    // �ְ�� �� ������ ������ �ϳ��� ������� �Լ��մϴ�.
    vFinalNoise = vNoise1 + vNoise2 + vNoise3;
    
     // �Է����� ���� �ؽ����� Y��ǥ�� �ְ� ũ��� ���̾ ������ ������ŵ�ϴ�.
    // �� ������ �ؽ����� �������� ������ �������� �� ���ʿ��� �����̴� ȿ���� �������ϴ�.
    fPerturb = ((1.0f - In.vTexcoord.y) * g_fDistortionScale) + g_fDistortionBias;

    // �Ҳ� ���� �ؽ��ĸ� ���ø��ϴµ� ���� �ְ� �� ������ �ؽ��� ��ǥ�� ����ϴ�.
    vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTexcoord.xy;
    
     // �ְ�ǰ� ������ �ؽ��� ��ǥ�� �̿��Ͽ� �Ҳ� �ؽ��Ŀ��� ������ ���ø��մϴ�.
    // wrap�� ����ϴ� ������Ʈ ��� clamp�� ����ϴ� ������Ʈ�� ����Ͽ� �Ҳ� �ؽ��İ� ���εǴ� ���� �����մϴ�.
    vFireColor = g_FireTexture.Sample(Sampler, vNoiseCoords.xy);

    // �ְ�ǰ� ������ �ؽ��� ��ǥ�� �̿��Ͽ� ���� �ؽ��Ŀ��� ���İ��� ���ø��մϴ�.
    // �Ҳ��� ������ �����ϴ� �� ���� ���Դϴ�.
    // wrap�� ����ϴ� ������Ʈ ��� clamp�� ����ϴ� ������Ʈ�� ����Ͽ� �Ҳ� �ؽ��İ� ���εǴ� ���� �����մϴ�.
    vAlphaColor = g_AlphaTexture.Sample(Sampler, vNoiseCoords.xy);
    if (vAlphaColor.r == 0.f)
        discard;
    
    // �ְ� �� ������ ���� �ؽ��� ���� ���ĺ����� ����մϴ�.
        
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




