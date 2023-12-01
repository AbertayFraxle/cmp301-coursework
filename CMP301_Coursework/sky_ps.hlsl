// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

#include "utils.h"

Texture2D texture0 : register(t0);

SamplerState sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    
    //multiply the light colour and the texture colour
    return textureColour;

}



