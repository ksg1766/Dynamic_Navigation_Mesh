#ifndef _CLIENT_SHADER_DEFINES_HLSL_
#define _CLIENT_SHADER_DEFINES_HLSL_

RasterizerState RS_Skybox
{
	FillMode = Solid;

	/* 앞면을 컬링하겠다. == 후면을 보여주겠다. */
	CullMode = None;

	/* 앞면을 시계방향으로 쓰겠다. */
	FrontCounterClockwise = false;
};

RasterizerState RS_Default
{
    FillMode = Solid;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_Skybox
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};

DepthStencilState DSS_NoDepthWrite
{
    DepthEnable = true;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

BlendState BS_OneBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};

#endif