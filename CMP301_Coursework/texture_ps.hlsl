// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry
#include "utils.h"

// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 ambient[LIGHTCOUNT];
    float4 diffuse[LIGHTCOUNT];
    float4 lightPosition[LIGHTCOUNT];
    float4 direction[LIGHTCOUNT];
    float4 factors[LIGHTCOUNT];
    float4 coneAngle[LIGHTCOUNT];
};

cbuffer MatrixBuffer : register(b1)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[LIGHTCOUNT];
    matrix lightProjectionMatrix[LIGHTCOUNT];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[LIGHTCOUNT] : TEXCOORD2;
};


float4 main(InputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    return texture0.Sample(Sampler0, input.tex);
}