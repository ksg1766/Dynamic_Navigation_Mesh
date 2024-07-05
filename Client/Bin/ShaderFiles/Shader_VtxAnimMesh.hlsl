#include "Client_Shader_Defines.hlsl"
#include "Shader_Lighting_Functions.hlsl"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector	g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector	g_vLightPos = vector(0.f, 0.f, 0.f, 1.f);
float	g_fLightRange = 0.f;
vector	g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector	g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector	g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);
vector	g_vLightEmissive = vector(1.f, 1.f, 1.f, 1.f);

vector	g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector	g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
vector	g_vMtrlEmissive = vector(1.f, 0.f, 0.f, 1.f);

vector	g_vCamPosition;

struct tagBoneMatrices
{
	matrix		BoneMatrix[250];
};

tagBoneMatrices     g_BoneMatrices;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
    float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
    float2		vTexcoord : TEXCOORD0;
    float4		vWorldPos : TEXCOORD1;
    float3		vTangent : TANGENT;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    float4x4 BoneMatrix = g_BoneMatrices.BoneMatrix[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.BoneMatrix[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.BoneMatrix[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.BoneMatrix[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);


	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
    float4		vWorldPos : TEXCOORD1;
    float3		vTangent : TANGENT;
};

struct PS_OUT
{
    float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
	PS_OUT		Out = (PS_OUT)0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    vector vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) +
		g_vLightAmbient * g_vMtrlAmbient;

    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade) +
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	return Out;	
}

PS_OUT PS_RIM_MAIN(PS_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    vector vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) +
		g_vLightAmbient * g_vMtrlAmbient;

    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);
	
    // Rim Light
    float3 E = normalize(-vLook);

    float value = saturate(dot(E, float3(In.vNormal.xyz)));
    float fEmissive = 1.0f - value;

	// min, max, x
    fEmissive = smoothstep(0.0f, 1.0f, fEmissive);
    fEmissive = pow(fEmissive, 2);
	//
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade) +
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular +
	(g_vLightEmissive * g_vMtrlEmissive) * fEmissive;
    
    return Out;
}

PS_OUT PS_DISSOLVE_MAIN(PS_IN In)
{
    ComputeNormalMapping(In.vNormal, In.vTangent, In.vTexcoord);
	
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
	
    vector vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f) +
		g_vLightAmbient * g_vMtrlAmbient;

    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade) +
		(g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    ComputeDissolveColor(Out.vColor, In.vTexcoord);
    
    return Out;
}

technique11 DefaultTechnique
{
	pass Mesh
	{
        SetRasterizerState(RS_Default);

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

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE_MAIN();
        ComputeShader = NULL;
    }
}