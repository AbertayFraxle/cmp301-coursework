// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

#define WAVESCALE 1
#define TIMESCALE 0.025
#define TILING 64
#define STEPAMOUNT 0.1

Texture2D heightMap0 : register(t0);
Texture2D heightMap1 : register(t1);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer TimeBuffer : register(b1)
{
    float time;
}


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

    float2 tex0 = input.tex * TILING + float2(time * TIMESCALE, -time * TIMESCALE);
    float2 tex1 = input.tex * TILING + float2(-time * TIMESCALE , -time * TIMESCALE);
    
    float4 sample0 = heightMap0.SampleLevel(sampler0, tex0, 0) * WAVESCALE;
    float4 sample1 = heightMap1.SampleLevel(sampler0, tex1, 0) * WAVESCALE;
    
    
    
    float2 offsets0[4] , offsets1[4];
    offsets0[0] = tex0 + float2(-STEPAMOUNT, 0);
    offsets0[1] = tex0 + float2(STEPAMOUNT, 0);
    offsets0[2] = tex0 + float2(0, -STEPAMOUNT);
    offsets0[3] = tex0 + float2(0, STEPAMOUNT);
    
    offsets1[0] = tex1 + float2(-STEPAMOUNT, 0);
    offsets1[1] = tex1  + float2(STEPAMOUNT, 0);
    offsets1[2] = tex1  + float2(0, -STEPAMOUNT);
    offsets1[3] = tex1  + float2(0, STEPAMOUNT);
    
    float heights0[4], heights1[4];
    
    for (int i = 0; i < 4; i++)
    {
        heights0[i] = heightMap0.SampleLevel(sampler0, offsets0[i], 0).x;
        heights1[i] = heightMap1.SampleLevel(sampler0, offsets1[i], 0).x;
    }

    float2 step = float2(STEPAMOUNT, 0.f);
    
    float3 va0 = normalize(float3(step.xy, heights0[1] - heights0[0]));
    float3 vb0 = normalize(float3(step.yx, heights0[3] - heights0[2]));
    
    float3 va1 = normalize(float3(step.xy, heights1[1] - heights1[0]));
    float3 vb1 = normalize(float3(step.yx, heights1[3] - heights1[2]));

    output.normal = normalize(cross(va0, vb0).xzy + cross(va1, vb1).xzy);
    
    input.position.y = (sample0.x + sample0.y + sample0.z + sample1.x + sample1.y + sample1.z);

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex * TILING;

    // Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(output.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    output.worldPosition = mul(input.position, worldMatrix).xyz;

    return output;
}