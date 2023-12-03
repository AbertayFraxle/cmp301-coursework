// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"
#include "utils.h"

using namespace std;
using namespace DirectX;


class TerrainTessellationShader : public BaseShader
{

public:

	TerrainTessellationShader(ID3D11Device* device, HWND hwnd);
	~TerrainTessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, int tessAmount, ID3D11ShaderResourceView* lowTex, ID3D11ShaderResourceView* highTex, ID3D11ShaderResourceView* lowNorm, ID3D11ShaderResourceView* highNorm, ID3D11ShaderResourceView* heightmap, ShadowMap* shadowMap[LIGHTCOUNT], Light* lights[LIGHTCOUNT], Camera* camera);

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
	};


	struct TesselationBufferType {
		XMINT4 tesselationAmount;
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT4 camPos;
	};

	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* shadowSampleState;
	ID3D11Buffer* tesselationBuffer;
	ID3D11Buffer* lightBuffer;
};
