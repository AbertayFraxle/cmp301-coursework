// Tessellation Hull Shader
// Prepares control points for tessellation

#include "utils.h"

SamplerState shadowSampler : register(s0);


struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 colour : COLOR;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float4 colour : COLOR;
};

cbuffer TesselationBuffer : register(b0)
{
    int4 tessellationFactor;
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float4 cameraPos;
};

float2 getProjectiveCoords(float4 lightViewPosition)
{
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    
    return projTex;
}

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;

    int tessFactor;
    
   // float4 rPosition;
    
   // rPosition = mul(float4(inputPatch[patchId].position, 1), worldMatrix);
   // rPosition = mul(rPosition, viewMatrix);
   // rPosition = mul(rPosition, projectionMatrix);
    
    
   // float2 diff = (cameraPos-rPosition).xz;
    
   // float len = length(diff);
   // if (len >= 10)
  //  {
        tessFactor = tessellationFactor.x;
  //  }
  //  else
  //  {
   //     tessFactor = 2;

  //  }
    
        
        
    // Set the tessellation factors for the three edges of the triangle.
    output.edges[0] = tessFactor;
    output.edges[1] = tessFactor;
    output.edges[2] = tessFactor;
    output.edges[3] = tessFactor;

    // Set the tessellation factor for tessallating inside the triangle.
    output.inside[0] = tessFactor;
    output.inside[1] = tessFactor;

    return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;
    
    output.tex = patch[pointId].tex;

    // Set the input colour as the output colour.
    output.colour = patch[pointId].colour;

    return output;
}