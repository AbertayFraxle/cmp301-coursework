// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

Texture2D heightMap : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

OutputType main(InputType input)
{
    OutputType output;


    float4 sample = heightMap.SampleLevel(sampler0, input.tex, 0);

    input.position.y = (sample.x + sample.y + sample.z) * 7.5f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex * 20;

    // Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    output.worldPosition = mul(input.position, worldMatrix).xyz;

    return output;
}