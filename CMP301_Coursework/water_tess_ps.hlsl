// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

#include "utils.h"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

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
};


struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(ldiffuse * intensity);
    return colour;
}

float4 main(InputType input) : SV_TARGET
{

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    float3 lightVector[LIGHTCOUNT];
    float4 lightColour = float4(0, 0, 0, 0);

//loop for all the lights in the scene
    for (int i = 0; i < LIGHTCOUNT; i++)
    {
        float4 calcLight = float4(0, 0, 0, 1);
    // rPosition = mul(rPosition, viewMatrix).xyz;
     //check if there's a direction, for this week, the only light with a direction will be the directional light
        if (coneAngle[i].x > 0.f)
        {
        //if no direction, calculate the lighting based on nomal point light calculation and add it to the lightColour vector
            lightVector[i] = normalize(lightPosition[i].xyz - input.worldPosition);

        //get the distance of the lit point to the light source
            float dist = length(lightPosition[i].xyz - input.worldPosition);

        //calculate the attenuation
            float attenuation = 1 / (factors[i].x + (factors[i].y * dist) + (factors[i].z * pow(dist, 2)));

        //calculate the lighting of the point
            calcLight += saturate(calculateLighting(lightVector[i], input.normal, diffuse[i]) * attenuation);
            
            calcLight *= pow(max(dot(-lightVector[i], direction[i].xyz), 0.0f), coneAngle[i].x);
            
            lightColour += calcLight;
        }
        else
        {
        //calculate the lighting intensity for the directional light
            float intensity = saturate(dot(input.normal, -direction[i].xyz));
            calcLight += saturate(diffuse[i] * intensity);
            
            lightColour += calcLight;
        }
    }

//saturate the light Colour
    lightColour = saturate(lightColour);

//add all the ambient light values to the light colour
    for (int i = 0; i < LIGHTCOUNT; i++)
    {
        lightColour += ambient[i];
    }

//saturate the light Colour
    lightColour = saturate(lightColour);

//multiply the light colour and the texture colour
    return saturate(lightColour * textureColour);
//return float4(input.normal.xyz,1);
}



