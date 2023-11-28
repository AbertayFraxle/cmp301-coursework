#pragma once

#include "utils.h"

#include "DXF.h"


using namespace std;
using namespace DirectX;

class LightShader : public BaseShader
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
	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normal, ShadowMap* shadowMap[LIGHTCOUNT], Light* lights[LIGHTCOUNT]);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
};

