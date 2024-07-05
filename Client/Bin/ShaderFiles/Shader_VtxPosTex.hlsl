#include "Client_Shader_Defines.hlsl"
#include "Shader_Lighting_Functions.hlsl"
/* ������̺�. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4 g_Color;
Texture2D g_Texture;
Texture2D g_Textures[2];

float2 g_UVoffset = float2(0.f, 0.f);
// textureCUBE


// for Water
matrix g_ReflectionMatrix;
Texture2D g_ReflectionTexture;
Texture2D g_RefractionTexture;

float g_fWaterTranslation;
float g_fReflectRefractScale;

vector g_vCamPosition;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;    
};

struct VS_OUT
{
	/* float4 : w���� �ݵ�� ���ܾ��ϴ� ������ w�� �佺���̽� ���� ����(z)�� �����ϱ� ���ؼ���. */
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
    
struct VS_GEYSER_OUT
{
	/* float4 : w���� �ݵ�� ���ܾ��ϴ� ������ w�� �佺���̽� ���� ����(z)�� �����ϱ� ���ؼ���. */
    float4 vPosition : SV_POSITION;
    //float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct VS_WATER_OUT
{
	/* float4 : w���� �ݵ�� ���ܾ��ϴ� ������ w�� �佺���̽� ���� ����(z)�� �����ϱ� ���ؼ���. */
    float4 vPosition : SV_POSITION;
    //float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    //float4 vWorldPos : TEXCOORD1;
    //float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
    
    float4 vRefractionPos : TEXCOORD3;
    float4 vReflectionPos : TEXCOORD4;
};

/* ���ؽ��� ���� ��ȯ�۾��� ��ģ��.  */
/* ��ȯ�۾� : ������ ��ġ�� ����, ��, ��������� ���Ѵ�. and �ʿ��� ��ȯ�� ���ؼ� �����Ӱ� �߰��ص� ������� .*/
/* ���ؽ��� ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN( /* ���� */VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* mul : ���(���ϱⰡ ������) ����� ���ϱ⸦ �����Ѵ�. */
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vTexcoord = In.vTexcoord;

    return Out;
}

VS_WATER_OUT VS_WATER_MAIN( /* ���� */VS_IN In)
{
    VS_WATER_OUT Out = (VS_WATER_OUT) 0;

	/* mul : ���(���ϱⰡ ������) ����� ���ϱ⸦ �����Ѵ�. */
    matrix matWV, matWVP;
    matrix matReflectPW;
    matrix matVPWorld;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    matReflectPW = mul(g_ReflectionMatrix, g_ProjMatrix);
    matReflectPW = mul(g_WorldMatrix, matReflectPW);
    Out.vReflectionPos = mul(float4(In.vPosition, 1.f), matReflectPW);
    
    matVPWorld = mul(g_ViewMatrix, g_ProjMatrix);
    matVPWorld = mul(g_WorldMatrix, matVPWorld);
    Out.vRefractionPos = mul(float4(In.vPosition, 1.f), matVPWorld);
    //Out.vRefractionPos = mul(In.vPosition, matWVP); // �̰Ŷ� ���� �ٸ���..
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    //Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    //Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    //Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

VS_GEYSER_OUT VS_GEYSER_MAIN( /* ���� */VS_IN In)
{
    VS_GEYSER_OUT Out = (VS_GEYSER_OUT) 0;

	/* mul : ���(���ϱⰡ ������) ����� ���ϱ⸦ �����Ѵ�. */
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    //Out.vNormal =  mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    //Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

/* w������ ����. ������ ������ȯ. */
/* ����Ʈ�����̽�(��������ǥ)�� ��ġ�� ��ȯ�Ѵ�. */
/* �����Ͷ����� : ������ �ѷ����� �ȼ��� ������ �����Ѵ�. */
/* �ȼ������� ���������� ����Ѵ�. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_GEYSER_IN
{
	/* float4 : w���� �ݵ�� ���ܾ��ϴ� ������ w�� �佺���̽� ���� ����(z)�� �����ϱ� ���ؼ���. */
    float4 vPosition : SV_POSITION;
    //float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_WATER_IN
{
	/* float4 : w���� �ݵ�� ���ܾ��ϴ� ������ w�� �佺���̽� ���� ����(z)�� �����ϱ� ���ؼ���. */
    float4 vPosition : SV_POSITION;
    //float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    //float4 vWorldPos : TEXCOORD1;
    //float3 vTangent : TANGENT;
    float4 vProjPos : TEXCOORD2;
    
    float4 vRefractionPos : TEXCOORD3;
    float4 vReflectionPos : TEXCOORD4;
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
    
struct PS_BEAM_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vDistortion : SV_TARGET1;
};

struct PS_GEYSER_OUT
{
	/* float4 : w���� �ݵ�� ���ܾ��ϴ� ������ w�� �佺���̽� ���� ����(z)�� �����ϱ� ���ؼ���. */
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float2 vDepth : SV_TARGET2;
    float4 vEmissive : SV_TARGET3;
    float4 vSunMask : SV_TARGET5;
};

struct PS_WATER_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vSunMask : SV_TARGET5;
};

/* ���޹��� �ȼ��� ������ �̿��Ͽ� �ȼ��� �������� ���� ��������. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = tex2D(DefaultSampler, In.vTexcoord);
	/*g_Texture�������� PointSampler�������  In.vTexcoord��ġ�� �ش��ϴ� ���� ���ø�(������)�ؿ�. */
	// Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);
	// Out.vColor = float4(1.f, 0.f, 0.f, 1.f);

    vector vSourColor = g_Textures[0].Sample(LinearSampler, In.vTexcoord);
    vector vDestColor = g_Textures[1].Sample(LinearSampler, In.vTexcoord);

    Out.vDiffuse = vSourColor + vDestColor;

    return Out;
}

PS_OUT PS_COLOR_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    vector vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (vColor.r < 0.01f)
        discard;

    Out.vDiffuse = g_Color;
    Out.vNormal = float4(0.f, 1.f, 0.f, 1.f);
    //Out.vDepth = float4(0.f, 1.f, 0.f, 1.f);
    Out.vEmissive = g_Color;
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_BEAM_OUT PS_BEAM_MAIN(PS_IN In)
{
    PS_BEAM_OUT Out = (PS_BEAM_OUT) 0;

    vector vSourColor = g_Textures[0].Sample(LinearSampler, In.vTexcoord + g_UVoffset);
    if (vSourColor.r < 0.2f)
        discard;
    
    vector vDestColor = g_Textures[1].Sample(LinearSampler, In.vTexcoord);
    
    vector vBeamColor = vSourColor * vDestColor;
    
    Out.vDiffuse = 0.8f * vBeamColor;
    Out.vDistortion = Out.vDiffuse;
    //Out.vNormal = float4(0.f, 1.f, 0.f, 1.f);
    ////Out.vDiffuse.a = 0.6f;
    //Out.vEmissive = Out.vDiffuse;
    //Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

bool g_bFresnel = true;

PS_WATER_OUT PS_WATER_MAIN(PS_WATER_IN In)
{
    PS_WATER_OUT Out = (PS_WATER_OUT) 0;

    float2 vReflectTexCoord;
    float2 vRefractTexCoord;
    float4 vNormalMap;
    float3 vNormal;
    float4 vReflectionColor;
    float4 vRefractionColor;
    
    In.vTexcoord.y += g_fWaterTranslation;
    
    vNormalMap = g_NormalTexture.Sample(LinearSampler, In.vTexcoord * 8.f);
    vNormal = (vNormalMap.xyz * 2.0f) - 1.0f;
    
    vReflectTexCoord.x = In.vReflectionPos.x / In.vReflectionPos.w / 2.0f + 0.5f;
    vReflectTexCoord.y = -In.vReflectionPos.y / In.vReflectionPos.w / 2.0f + 0.5f;
	
    vRefractTexCoord.x = In.vRefractionPos.x / In.vRefractionPos.w / 2.0f + 0.5f;
    vRefractTexCoord.y = -In.vRefractionPos.y / In.vRefractionPos.w / 2.0f + 0.5f;
    
    vReflectTexCoord = vReflectTexCoord + (vNormal.xy * g_fReflectRefractScale);
    vRefractTexCoord = vRefractTexCoord + (vNormal.xy * g_fReflectRefractScale);
    
    vReflectionColor = g_ReflectionTexture.Sample(LinearSampler, vReflectTexCoord);
    vRefractionColor = g_RefractionTexture.Sample(LinearSampler, vRefractTexCoord);

    float fFresnelTerm = 0.f;
    vector vLook = In.vProjPos - g_vCamPosition;
    vLook = normalize(vLook);
    
    if (true == g_bFresnel)
    {
        fFresnelTerm = 0.02f + 0.97f * pow((1.f - dot(vLook, float4(vNormal, 1.f))), 5.f);
        float4 vCombinedColor = vRefractionColor * (1 - fFresnelTerm) * vRefractionColor.a * vReflectionColor.a + vReflectionColor * fFresnelTerm * vReflectionColor.a * vRefractionColor.a;
        Out.vColor = saturate(vCombinedColor * float4(0.95f, 1.00f, 1.05f, 1.0f) + float4(0.15f, 0.15f, 0.15f, 0.0f));
        //Out.vColor = lerp(vReflectionColor, vRefractionColor, 0.6f) * float4(0.95f, 1.00f, 1.05f, 1.0f);
    }
    else
    {
        Out.vColor = lerp(vReflectionColor, vRefractionColor, 0.5f) * float4(0.90f, 1.00f, 1.10f, 1.0f) + float4(0.35f, 0.35f, 0.35f, 0.0f); //
    }
    
    Out.vNormal = float4(vNormal, 0.f);
    //Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_GEYSER_OUT PS_GEYSER_MAIN(PS_GEYSER_IN In)
{
    PS_GEYSER_OUT Out = (PS_GEYSER_OUT) 0;

    vector vSourColor = g_Textures[0].Sample(LinearSampler, In.vTexcoord);
    if (vSourColor.r < 0.49f)
        discard;
    
    float4  vNormalMap = g_Textures[1].Sample(LinearSampler, In.vTexcoord);
    float3  vNormal = (vNormalMap.xyz * 2.0f) - 1.0f;
	// Out.vColor = tex2D(DefaultSampler, In.vTexcoord);
	/*g_Texture�������� PointSampler�������  In.vTexcoord��ġ�� �ش��ϴ� ���� ���ø�(������)�ؿ�. */
	// Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);
	// Out.vColor = float4(1.f, 0.f, 0.f, 1.f);

    float4 vRedColor = float4(1.f, 0.6f, 0.75f, 1.f);
    vRedColor *= vSourColor;
    vRedColor.r = 1.f;
    //vRedColor *= pow(vSourColor.r, 2);
    Out.vDiffuse = vRedColor;
    Out.vNormal = float4(vNormal, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 2000.0f, 0.f, 0.f);
    Out.vEmissive = vRedColor;
    Out.vSunMask = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

//PS_OUT PS_CHAIN_MAIN(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    vector vLightning = g_TextureEx2.Sample(LinearSampler, In.vTexcoord);
//    vector vBolt = vector(0.f, 0.f, 0.f, 0.f);
//    vector vAir = vector(0.f, 0.f, 0.f, 0.f);
    
//    vector vColor;
    
//    if (vLightning.r < 0.01f)
//    {
//        vBolt = g_TextureEx1.Sample(LinearSampler, In.vTexcoord);
//        if (vBolt.r < 0.01f)
//            vAir = g_Texture.Sample(LinearSampler, In.vTexcoord + g_UVoffset);
//    }

//    vColor = vLightning + vBolt + vAir;

//    if (vColor.r < 0.01f)
//        discard;
    
//    Out.vColor = 1.f - g_Color * (1.f - vColor.r);
//    Out.vColor.a = 1.f;
    
//    return Out;
//}

technique11 DefaultTechnique
{
	/* */
    pass UI
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

    pass AMMO_DEFAULT
    {
        SetRasterizerState(RS_Skybox);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_MAIN();
        ComputeShader = NULL;
    }

    pass AMMO_BEAM
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_BEAM_MAIN();
        ComputeShader = NULL;
    }

    pass Water
    {
		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_WATER_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_WATER_MAIN();
        ComputeShader = NULL;
    }

    pass GeyserCrack
    {
		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_GEYSER_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_GEYSER_MAIN();
        ComputeShader = NULL;
    }
}