// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

cbuffer TimeBuffer : register(b0)
{
    float time;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    
    float2 newCoords = float2(input.tex.x, input.tex.y + (sin(time + input.tex.x/0.25) *0.1));
    
    return texture0.Sample(Sampler0, newCoords);
}