#pragma once

#include "DXF.h"
#include "utils.h"

using namespace std;
using namespace DirectX;

class TerrainShader : public BaseShader
{
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




public:
	TerrainShader(ID3D11Device* device, HWND hwnd);
	~TerrainShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* lowTex, ID3D11ShaderResourceView* highTex, ID3D11ShaderResourceView* heightmap, ShadowMap* shadowMap[LIGHTCOUNT], Light* lights[LIGHTCOUNT]);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* shadowSampleState;
	ID3D11Buffer* lightBuffer;
};

