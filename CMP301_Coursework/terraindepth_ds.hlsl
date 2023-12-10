Texture2D heightMap : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 colour : COLOR;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

//lifted code from normal terrain shader
//this returns the right positions to get correct depth values when doing the pass

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    OutputType output;

    float3 vertexPosition;
    float2 texPosition;
    

    float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);

    vertexPosition = lerp(v1, v2, uvwCoord.x);
    
    float2 t1 = lerp(patch[0].tex , patch[1].tex , uvwCoord.y);
    float2 t2 = lerp(patch[3].tex , patch[2].tex, uvwCoord.y);
    
    texPosition = lerp(t1, t2, uvwCoord.x);
        
    float4 sample = heightMap.SampleLevel(sampler0, texPosition, 0);

    vertexPosition.y = (sample.x + sample.y + sample.z) * 7.5f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    
    // Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;

    return output;
}