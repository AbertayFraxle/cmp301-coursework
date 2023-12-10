// Tessellation Hull Shader
// Prepares control points for tessellation
struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 colour : COLOR;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 colour : COLOR;
};

cbuffer TesselationBuffer : register(b0)
{
    int4 tessDistances;
    matrix worldMatrix;
    matrix viewMatrix;
};


//dynamic tesselation adapted from guide at https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation by Dr Jeffrey Paone

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;

    const float MIN_DISTANCE = tessDistances.x;
    const float MAX_DISTANCE = tessDistances.y;
    const int MIN_TESS = tessDistances.z;
    const int MAX_TESS = tessDistances.w;
    
    //get positions of the 4 control points and multiply them to be in view space
    float4 rPos00 = mul(float4(inputPatch[0].position, 1), worldMatrix);
    rPos00 = mul(rPos00, viewMatrix);

    float4 rPos01 = mul(float4(inputPatch[3].position, 1), worldMatrix);
    rPos01 = mul(rPos01, viewMatrix);

    float4 rPos10 = mul(float4(inputPatch[1].position, 1), worldMatrix);
    rPos10 = mul(rPos10, viewMatrix);

    float4 rPos11 = mul(float4(inputPatch[2].position, 1), worldMatrix);
    rPos11 = mul(rPos11, viewMatrix);

    //access the z values of the screen space positions and clamp them between 0 and 1 
    float dist00 = clamp((abs(rPos00.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
    float dist01 = clamp((abs(rPos01.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
    float dist10 = clamp((abs(rPos10.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
    float dist11 = clamp((abs(rPos11.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

    //interpolate between the maximum and minimum tesselation values dependent on the smallest distance on the edge
    float tessLevel0 = lerp(MAX_TESS, MIN_TESS, min(dist10, dist00));
    float tessLevel1 = lerp(MAX_TESS, MIN_TESS, min(dist00, dist01));
    float tessLevel2 = lerp(MAX_TESS, MIN_TESS, min(dist01, dist11));
    float tessLevel3 = lerp(MAX_TESS, MIN_TESS, min(dist11, dist10));

    // Set the tessellation factors for the edges of the patch
    output.edges[0] = tessLevel0;
    output.edges[1] = tessLevel1;
    output.edges[2] = tessLevel2;
    output.edges[3] = tessLevel3;

    // Set the tessellation factor for tessallating inside the patch
    output.inside[0] = max(tessLevel1, tessLevel3);
    output.inside[1] = max(tessLevel0, tessLevel2);

    return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;


    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;
    
    output.tex = patch[pointId].tex;

    // Set the input colour as the output colour.
    output.colour = patch[pointId].colour;

    return output;
}