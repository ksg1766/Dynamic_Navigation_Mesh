float3 gObstCenter;
float3 gObstExtents;
    
struct InputTri
{
    float3 vPoint0;
    float3 vPoint1;
    float3 vPoint2;
};

struct OutputResult
{
    bool isIntersected;
};

StructuredBuffer<InputTri> InputCell;
RWStructuredBuffer<OutputResult> Output;

float Max(float a, float b, float c)
{
    return max(max(a, b), c);
}

float Min(float a, float b, float c)
{
    return min(min(a, b), c);
}

bool IntersectTriangleAABB(float3 vT0, float3 vT1, float3 vT2, float3 vCenter, float3 vExtents)
{
    float3 vV0 = vT0 - vCenter;
    float3 vV1 = vT1 - vCenter;
    float3 vV2 = vT2 - vCenter;

    // Compute edge vectors for triangle
    float3 vEdge0 = vT1 - vT0;
    float3 vEdge1 = vT2 - vT1;
    float3 vEdge2 = vT0 - vT2;

    //// region Test axes vA00..vA22 (category 3)

    // Axis vA00
    float3 vA00 = float3(0.0f, -vEdge0.z, vEdge0.y);
    float fP0 = dot(vV0, vA00);
    float fP1 = dot(vV1, vA00);
    float fP2 = dot(vV2, vA00);
    float fR = vExtents.y * abs(vEdge0.z) + vExtents.z * abs(vEdge0.y);
    if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
       return false;

    // Axis vA01
    float3 vA01 = float3(0, -vEdge1.z, vEdge1.y);
    fP0 = dot(vV0, vA01);
    fP1 = dot(vV1, vA01);
    fP2 = dot(vV2, vA01);
    fR = vExtents.y * abs(vEdge1.z) + vExtents.z * abs(vEdge1.y);
    if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
        return false;

    // Axis vA02
    float3 vA02 = float3(0.0f, -vEdge2.z, vEdge2.y);
    fP0 = dot(vV0, vA02);
    fP1 = dot(vV1, vA02);
    fP2 = dot(vV2, vA02);
    fR = vExtents.y * abs(vEdge2.z) + vExtents.z * abs(vEdge2.y);
    if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
        return false;

    // Axis vA10
    float3 vA10 = float3(vEdge0.z, 0.0f, -vEdge0.x);
    fP0 = dot(vV0, vA10);
    fP1 = dot(vV1, vA10);
    fP2 = dot(vV2, vA10);
    fR = vExtents.x * abs(vEdge0.z) + vExtents.z * abs(vEdge0.x);
    if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
        return false;

    // Axis vA11
    float3 vA11 = float3(vEdge1.z, 0.0f, -vEdge1.x);
    fP0 = dot(vV0, vA11);
    fP1 = dot(vV1, vA11);
    fP2 = dot(vV2, vA11);
    fR = vExtents.x * abs(vEdge1.z) + vExtents.z * abs(vEdge1.x);
    if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
        return false;

    // Axis vA12
    float3 vA12 = float3(vEdge2.z, 0.0f, -vEdge2.x);
    fP0 = dot(vV0, vA12);
    fP1 = dot(vV1, vA12);
    fP2 = dot(vV2, vA12);
    fR = vExtents.x * abs(vEdge2.z) + vExtents.z * abs(vEdge2.x);
    if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
        return false;

    // Axis vA20
    float3 vA20 = float3(-vEdge0.y, vEdge0.x, 0.0f);
    fP0 = dot(vV0, vA20);
    fP1 = dot(vV1, vA20);
    fP2 = dot(vV2, vA20);
    fR = vExtents.x * abs(vEdge0.y) + vExtents.y * abs(vEdge0.x);
    if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
        return false;

    // Axis vA21
    float3 vA21 = float3(-vEdge1.y, vEdge1.x, 0.0f);
    fP0 = dot(vV0, vA21);
    fP1 = dot(vV1, vA21);
    fP2 = dot(vV2, vA21);
    fR = vExtents.x * abs(vEdge1.y) + vExtents.y * abs(vEdge1.x);
    if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
        return false;

    // Axis vA22
    float3 vA22 = float3(-vEdge2.y, vEdge2.x, 0.0f);
    fP0 = dot(vV0, vA22);
    fP1 = dot(vV1, vA22);
    fP2 = dot(vV2, vA22);
    fR = vExtents.x * abs(vEdge2.y) + vExtents.y * abs(vEdge2.x);
    if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
        return false;

    //// endregion

    //// region Test the three axes corresponding to the face normals of AABB b (category 1)

    // Exit if...
    // ... [-vExtents.x, vExtents.x] and [Min(vV0.x,vV1.x,vV2.x), Max(vV0.x,vV1.x,vV2.x)] do not overlap
    if (Max(vV0.x, vV1.x, vV2.x) < -vExtents.x || Min(vV0.x, vV1.x, vV2.x) > vExtents.x)
        return false;

    // ... [-vExtents.y, vExtents.y] and [Min(vV0.y,vV1.y,vV2.y), Max(vV0.y,vV1.y,vV2.y)] do not overlap
    if (Max(vV0.y, vV1.y, vV2.y) < -vExtents.y || Min(vV0.y, vV1.y, vV2.y) > vExtents.y)
        return false;

    // ... [-vExtents.z, vExtents.z] and [Min(vV0.z,vV1.z,vV2.z), Max(vV0.z,vV1.z,vV2.z)] do not overlap
    if (Max(vV0.z, vV1.z, vV2.z) < -vExtents.z || Min(vV0.z, vV1.z, vV2.z) > vExtents.z)
        return false;

    //// endregion

    //// region Test separating axis corresponding to triangle face normal (category 2)

    float3 vPlaneNormal = cross(vEdge0, vEdge1);
    float vPlaneDistance = abs(dot(vPlaneNormal, vV0));

    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    fR = vExtents.x * abs(vPlaneNormal.x) + vExtents.y * abs(vPlaneNormal.y) + vExtents.z * abs(vPlaneNormal.z);

    // Intersection occurs when plane distance falls within [-fR,+fR] interval
    if (vPlaneDistance > fR)
        return false;

    return true;
}

[numthreads(1024, 1, 1)]
void CS_MAIN(uint iId : SV_GroupIndex)
{
    float3 vP0 = InputCell[iId].vPoint0;
    float3 vP1 = InputCell[iId].vPoint1;
    float3 vP2 = InputCell[iId].vPoint2;
    
    Output[iId].isIntersected = IntersectTriangleAABB(vP0, vP1, vP2, gObstCenter, gObstExtents);
}

technique11 T0
{
    pass P0 // 8
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_MAIN();
    }
}




