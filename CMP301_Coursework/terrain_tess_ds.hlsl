// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices

#include "utils.h"
#define STEPAMOUNT .1f

Texture2D heightMap : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[LIGHTCOUNT];
    matrix lightProjectionMatrix[LIGHTCOUNT];
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
    float2 tex: TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[LIGHTCOUNT] : TEXCOORD2;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    float2 texPosition;
    
    OutputType output;

    //determine the vertex position by interpolating by the patches positions and the provided uvw coord
    float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);

    vertexPosition = lerp(v1, v2, uvwCoord.x);
    
    //determine the texcoord position by interpolating by the patches positions and the provided uvw coord
    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    
    texPosition = lerp(t1, t2, uvwCoord.x);
    
    //determine the offsets to use to generate the normals of the terrain
    float2 offsets[4];
    offsets[0] = texPosition + float2(-STEPAMOUNT, 0);
    offsets[1] = texPosition + float2(STEPAMOUNT, 0);
    offsets[2] = texPosition + float2(0, -STEPAMOUNT);
    offsets[3] = texPosition + float2(0, STEPAMOUNT);
    
    //store the heights at these offsets to current point to help find normals
    float heights[4];
    
    for (int i = 0; i < 4; i++)
    {
        heights[i] = heightMap.SampleLevel(sampler0, offsets[i], 0).x;
    }

    float2 step = float2(STEPAMOUNT, 0.f);
    
    //do a cross product of the vectors going over current vertex to determine the normal
    float3 va = normalize(float3(step.xy, heights[1] - heights[0]));
    float3 vb = normalize(float3(step.yx, heights[3] - heights[2]));

    output.normal = cross(va, vb).xzy;
    
    //sample the heightmap and multiply this value by 7.5 to determine height
    float4 sample = heightMap.SampleLevel(sampler0, texPosition, 0);
    
    vertexPosition.y = (sample.x + sample.y + sample.z) * 7.5f;

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    //cast position to light view for shadow chicanery
    for (int i = 0; i < LIGHTCOUNT; i++)
    {
        output.lightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
        
    }
    
    //multiply the texcoord by 20 for our tiling
    output.tex = texPosition * 20;
    
    output.worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix).xyz;

    return output;
}

