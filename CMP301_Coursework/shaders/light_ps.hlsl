// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

#include "..\utils.h"

Texture2D texture0 : register(t0);
Texture2D normal : register(t1);

Texture2D depthMapTexture[LIGHTCOUNT] : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 ambient[LIGHTCOUNT];
    float4 diffuse[LIGHTCOUNT];
    float4 position[LIGHTCOUNT];
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

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(ldiffuse * intensity);
	return colour;
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
    
    //if the texture colour is pure white, return colour of emissive light source
    if (all(textureColour == float4(1, 1, 1,1)))
    {
        return float4(1.0f, 0.6f, 0.0f, 1.0f);

    }
    
    float3 lightVector[LIGHTCOUNT];
    float4 lightColour = float4(0, 0, 0, 0);
    float shadowMapBias = 0.001f;
    
    //apply a normal modifier determined by sampling the provided normalmap
    float4 newNorm = normal.Sample(sampler0, input.tex);
    float3 backup = input.normal;
    
    input.normal = normalize(input.normal + newNorm);
    

	//loop for all the lights in the scene
    for (int i = 0; i < LIGHTCOUNT; i++)
    {
        
        //project point to light view and get tex coords
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[i]);
        
        //initialise calculated light at point as black
        float4 calcLight = float4(0, 0, 0, 1);
        
         //check if theres depth data for tex coord
        if (hasDepthData(pTexCoord))
        {
       
            
            
                
            if (!isInShadow(depthMapTexture[i], pTexCoord, input.lightViewPos[i], shadowMapBias))
            {
        
                 //if not in shadow, light the point
                float4 calcLight = float4(0, 0, 0, 1);
		//if theres a coneangle, treat as a spotlight
                if (coneAngle[i].x > 0.f)
                {
                    float3 rPosition = position[i].xyz;
                    rPosition = mul(rPosition, worldMatrix);
            

                //get unit vector of direction of light to point
                    lightVector[i] = normalize(rPosition - input.worldPosition);

                //get the distance of the lit point to the light source
                    float dist = length(rPosition - input.worldPosition);

                //calculate the attenuation
                    float attenuation = 1 / (factors[i].x + (factors[i].y * dist) + (factors[i].z * pow(dist, 2)));


                    //if the face direction isn't facing away from the light
                    if (dot(backup.xyz, -lightVector[i].xyz) < 0)
                    {
            
                        //calculate the lighting of the point
                        calcLight = saturate(calculateLighting(lightVector[i], input.normal, diffuse[i]) * attenuation);
            
            

                        calcLight *= pow(max(dot(-lightVector[i], direction[i].xyz), 0.0f), coneAngle[i].x);
            
                        lightColour += calcLight;
                    }
                }
                else
                {
                //calculate the lighting intensity for the directional light
                    float intensity = saturate(dot(input.normal, -direction[i].xyz));
                    calcLight += saturate(diffuse[i] * intensity);
            
                    lightColour += calcLight;
                }
            }
        }
    }
	
    //saturate the light Colour
    lightColour= saturate(lightColour);
    
    //add all the ambient light values to the light colour
    for (int i = 0; i < LIGHTCOUNT; i++)
    {
        lightColour += ambient[i];
    }

 
    
    //multiply the light colour and the texture colour
    float4 retColour = lightColour * textureColour;
    
    return retColour;
}



