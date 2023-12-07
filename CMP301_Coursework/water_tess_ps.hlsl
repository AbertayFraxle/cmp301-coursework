// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

#include "utils.h"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

Texture2D depthMapTexture[LIGHTCOUNT] : register(t1);

cbuffer LightBuffer : register(b0)
{
    float4 ambient[LIGHTCOUNT];
    float4 diffuse[LIGHTCOUNT];
    float4 lightPosition[LIGHTCOUNT];
    float4 direction[LIGHTCOUNT];
    float4 factors[LIGHTCOUNT];
    float4 coneAngle[LIGHTCOUNT];
    float4 specular[LIGHTCOUNT];
    float4 specularPower[LIGHTCOUNT];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[LIGHTCOUNT] : TEXCOORD2;
    float3 viewVector : JEFF;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(ldiffuse * intensity);
    return colour;
}

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);
}

bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{


    float depthValue = sMap.Sample(shadowSampler, uv).r;
    
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    
    lightDepthValue -= bias;
    
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    float3 lightVector[LIGHTCOUNT];
    float4 specularColour[LIGHTCOUNT];
    float4 lightColour = float4(0, 0, 0, 0);

//loop for all the lights in the scene
    
    float shadowMapBias = 0.005f;
    
    for (int i = 0; i < LIGHTCOUNT; i++)
    {
        
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[i]);
    
        specularColour[i] = float4(0, 0, 0, 1);
        
            float4 calcLight = float4(0, 0, 0, 1);
        
        if (hasDepthData(pTexCoord))
        {
        
            
            
           if (!isInShadow(depthMapTexture[i], pTexCoord, input.lightViewPos[i], shadowMapBias))
            {
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
            
                    if (all(calcLight == float4(0, 0, 0, 1)))
                    {
            
                        specularColour[i] = calcSpecular(-lightVector[i], input.normal, input.viewVector, specular[i], specularPower[i].x);
                    }
            

                }
                else
                {
        //calculate the lighting intensity for the directional light
                    float intensity = saturate(dot(input.normal, -direction[i].xyz));
                    calcLight += saturate(diffuse[i] * intensity);
            
            
                    lightColour += calcLight;
            
                    specularColour[i] = calcSpecular(-direction[i].xyz, input.normal, input.viewVector, specular[i], specularPower[i].x);

                }
            }
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
    float4 returnColour = saturate(lightColour * textureColour) ;
    
    for (int i = 0; i < LIGHTCOUNT; i++)
    {
        returnColour += specularColour[i];
    }
    
    return returnColour;
//return float4(input.normal.xyz,1);
}



