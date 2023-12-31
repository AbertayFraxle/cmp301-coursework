// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"
#include "utils.h"

using namespace std;
using namespace DirectX;


class WaterTessellationShader : public BaseShader
{

public:

	WaterTessellationShader(ID3D11Device* device, HWND hwnd);
	~WaterTessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* waterTex, ID3D11ShaderResourceView* heightmap1, ID3D11ShaderResourceView* heightmap2, float time, Light* lights[LIGHTCOUNT], Camera* camera, ShadowMap* shadowMap[LIGHTCOUNT], XMINT4 tessValues);

private:

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[LIGHTCOUNT];
		XMMATRIX lightProjection[LIGHTCOUNT];
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[LIGHTCOUNT];
		XMFLOAT4 diffuse[LIGHTCOUNT];
		XMFLOAT4 position[LIGHTCOUNT];
		XMFLOAT4 direction[LIGHTCOUNT];
		XMFLOAT4 factors[LIGHTCOUNT];
		XMFLOAT4 coneangle[LIGHTCOUNT];
		XMFLOAT4 specular[LIGHTCOUNT];
		XMFLOAT4 specularPower[LIGHTCOUNT];
	};

	struct TimeBufferType
	{
		float time;
		XMFLOAT3 cameraPos;
	};

	struct TesselationBufferType {
		XMINT4 tessDistances;
		XMMATRIX world;
		XMMATRIX view;
	};
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* shadowSampleState;
	ID3D11Buffer* tesselationBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
};
