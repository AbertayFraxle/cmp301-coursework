// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
#define STEPAMOUNT 0.01f
#define LIGHTCOUNT 4

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


struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[LIGHTCOUNT] : TEXCOORD2;
};

OutputType main(InputType input)
{
    OutputType output;
    
    float2 offsets[4];
    offsets[0] = input.tex + float2(-STEPAMOUNT, 0);
    offsets[1] = input.tex + float2(STEPAMOUNT, 0);
    offsets[2] = input.tex + float2(0, -STEPAMOUNT);
    offsets[3] = input.tex + float2(0, STEPAMOUNT);
    
    float heights[4];
    
    for (int i = 0; i < 4; i++)
    {
        heights[i] = heightMap.SampleLevel(sampler0, offsets[i], 0).x;
    }

    float2 step = float2(STEPAMOUNT, 0.f);
    
    float3 va = normalize(float3(step.xy, heights[1] - heights[0]));
    float3 vb = normalize(float3(step.yx, heights[3] - heights[2]));

    output.normal = cross(va,vb).xzy;
    
    float4 sample = heightMap.SampleLevel(sampler0, input.tex, 0);
    

    input.position.y = (sample.x + sample.y + sample.z) * 7.5f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    for (int i = 0; i < LIGHTCOUNT; i++)
    {
        output.lightViewPos[i] = mul(input.position, worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
        
    }
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex*20;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(output.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    
    
    
    output.worldPosition = mul(input.position, worldMatrix).xyz;

    return output;
}