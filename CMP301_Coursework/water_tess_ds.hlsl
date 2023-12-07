// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices

#include "utils.h"
#define STEPAMOUNT 0.25f
#define WAVESCALE 1.f
#define TIMESCALE 0.1f  
#define TILING 4



Texture2D heightMap0 : register(t0);
Texture2D heightMap1 : register(t1);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[LIGHTCOUNT];
    matrix lightProjectionMatrix[LIGHTCOUNT];
};

cbuffer TimeBuffer : register(b1)
{
    float time;
    float3 cameraPos;
}

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
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[LIGHTCOUNT] : TEXCOORD2;
    float3 viewVector : JEFF;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    float2 texPosition;

    OutputType output;

    float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);

    vertexPosition = lerp(v1, v2, uvwCoord.x);
    
    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    
    texPosition = lerp(t1, t2, uvwCoord.x);
    
    float2 tex0 = texPosition * (1.5 * TILING) + float2(time * TIMESCALE, -time * TIMESCALE);
    float2 tex1 = texPosition * TILING + float2(-time * TIMESCALE, -time * TIMESCALE);
    
    float4 sample0 = heightMap0.SampleLevel(sampler0, tex0, 0) * WAVESCALE;
    float4 sample1 = heightMap1.SampleLevel(sampler0, tex1, 0) * WAVESCALE;
    
    float2 offsets0[4], offsets1[4];
    offsets0[0] = tex0 + float2(-STEPAMOUNT, 0);
    offsets0[1] = tex0 + float2(STEPAMOUNT, 0);
    offsets0[2] = tex0 + float2(0, -STEPAMOUNT);
    offsets0[3] = tex0 + float2(0, STEPAMOUNT);
    
    offsets1[0] = tex1 + float2(-STEPAMOUNT, 0);
    offsets1[1] = tex1 + float2(STEPAMOUNT, 0);
    offsets1[2] = tex1 + float2(0, -STEPAMOUNT);
    offsets1[3] = tex1 + float2(0, STEPAMOUNT);
    
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
    
    vertexPosition.y = (sample0.x + sample0.y + sample0.z + sample1.x + sample1.y + sample1.z);

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

   for (int i = 0; i < LIGHTCOUNT; i++)
    {
        output.lightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
        
   }
    
    // Send the input color into the pixel shader.
    output.tex = texPosition * TILING;
    
    output.worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix).xyz;

    output.viewVector = cameraPos - output.worldPosition.xyz;
    output.viewVector = normalize(output.viewVector);
    
    return output;
}

