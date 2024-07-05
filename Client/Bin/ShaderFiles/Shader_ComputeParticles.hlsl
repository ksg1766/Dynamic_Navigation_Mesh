
struct InputDesc
{
    row_major matrix input;
};

struct OutputDesc
{
    row_major matrix result;
};

StructuredBuffer<InputDesc> Input;
RWStructuredBuffer<OutputDesc> Output;

[numthreads(128, 1, 1)]
void CS_MAIN(uint id : SV_GroupIndex)
{
    float fTimeDelta = 0.008f;
    
    float3 vLinearVelocity = Input[id].input._31_32_33;
    vLinearVelocity.y = vLinearVelocity.y + (-9.81f * fTimeDelta * 0.5f);

    //float fLinearResistance = 0.011f; // drag0.01f + material drag0.001f

    vLinearVelocity = vLinearVelocity * 0.989f;
   
    //(fLinearResistance < 1.f) ? (vLinearVelocity = vLinearVelocity * (1.f - fLinearResistance)) : (vLinearVelocity = float3(0.f, 0.f, 0.f));
    //vLinearVelocity = vLinearVelocity * (1.f - fLinearResistance);
   
    row_major matrix result = Input[id].input;
    result._31_32_33 = vLinearVelocity;
    result._41_42_43 = result._41_42_43 + vLinearVelocity * fTimeDelta;
    
    Output[id].result = result;
    
    GroupMemoryBarrierWithGroupSync();
}

technique11 T0
{
    pass P0
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_MAIN();
    }
};
