// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

#define LIGHTCOUNT 3
#define TEXTURECHANGE 8
#define BLENDREGION 2

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);

Texture2D depthMapTexture : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);


cbuffer LightBuffer : register(b0)
{
    float4 ambient[LIGHTCOUNT];
    float4 diffuse[LIGHTCOUNT];
    float4 lightPosition[LIGHTCOUNT];
    float4 direction[LIGHTCOUNT];
    float4 factors[LIGHTCOUNT];
};

cbuffer MatrixBuffer : register(b1)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};


struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos : TEXCOORD2;
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
    
    float4 textureColour;
    float diff = abs(input.worldPosition.y - TEXTURECHANGE);
    
    float2 pTexCoord = getProjectiveCoords(input.lightViewPos);
    
    float shadowMapBias = 0.005f;

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    if (input.worldPosition.y < TEXTURECHANGE)
    {
        if (diff < BLENDREGION) {

            float bias1 = diff / BLENDREGION;
            float bias2 = 1 - bias1;

            textureColour = (texture0.Sample(sampler0, input.tex)*bias1) + (texture1.Sample(sampler0, input.tex)*bias2);

        }
        else {

            textureColour = texture0.Sample(sampler0, input.tex);
        }
    }
    else 
    {
       
       textureColour = texture1.Sample(sampler0, input.tex);
  
    }

    
    float3 lightVector[LIGHTCOUNT];
    float4 lightColour = float4(0, 0, 0, 0);

	//loop for all the lights in the scene
    for (int i = 0; i < LIGHTCOUNT; i++)
    {
       
        
        
        if (hasDepthData(pTexCoord))
        {
        
            
            if (!isInShadow(depthMapTexture, pTexCoord, input.lightViewPos, shadowMapBias))
            {
            
        
             // rPosition = mul(rPosition, viewMatrix).xyz;
		    //check if there's a direction, for this week, the only light with a direction will be the directional light
                if (length(direction[i]) <= 0)
                {
            //if no direction, calculate the lighting based on nomal point light calculation and add it to the lightColour vector
                    lightVector[i] = normalize(lightPosition[i].xyz - input.worldPosition);

            //get the distance of the lit point to the light source
                    float dist = length(lightPosition[i].xyz - input.worldPosition);

            //calculate the attenuation
                    float attenuation = 1 / (factors[i].x + (factors[i].y * dist) + (factors[i].z * pow(dist, 2)));

            //calculate the lighting of the point
                    lightColour += saturate(calculateLighting(lightVector[i], input.normal, diffuse[i]) * attenuation);
                }
                else
                {
            //calculate the lighting intensity for the directional light
                    float intensity = saturate(dot(input.normal, -direction[i].xyz));
                    lightColour += saturate(diffuse[i] * intensity);
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

    //multiply the light colour and the texture colour
    return lightColour * textureColour;
}



