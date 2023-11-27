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
    
    float2 newCoords = float2(input.tex.x + cos(time+input.tex.y/0.25)*0.1, input.tex.y + (sin(time + input.tex.x/0.25) *0.1));
    
    float weight0, weight1, weight2;
    float4 colour;

	// Create the weights that each neighbor pixel will contribute to the blur.
    weight0 = 0.2;
    weight1 = 0.4f;
    weight2 = 0.4f;

    // Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float texelSize = 8.0f / 1080;
    // Add the vertical pixels to the colour by the specific weight of each.
    colour += texture0.Sample(Sampler0, newCoords + float2(0.0f, texelSize * -2.0f)) * weight2;
    colour += texture0.Sample(Sampler0, newCoords + float2(0.0f, texelSize * -1.0f)) * weight1;
    colour += texture0.Sample(Sampler0, newCoords) * weight0;
    colour += texture0.Sample(Sampler0, newCoords + float2(0.0f, texelSize * 1.0f)) * weight1;
    colour += texture0.Sample(Sampler0, newCoords + float2(0.0f, texelSize * 2.0f)) * weight2;
 
    // Set the alpha channel to one.
    colour.a = 1.0f;

    return saturate(colour);
    
}