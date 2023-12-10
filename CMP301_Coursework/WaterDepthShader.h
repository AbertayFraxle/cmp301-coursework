// Colour shader.h
// Simple shader example.
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class WaterDepthShader : public BaseShader
{

public:

	WaterDepthShader(ID3D11Device* device, HWND hwnd);
	~WaterDepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* heightmap1, ID3D11ShaderResourceView* heightmap2, float time, XMINT4 tessValues);

private:
	struct TesselationBufferType {
		XMINT4 tessDistances;
		XMMATRIX world;
		XMMATRIX view;
	};

	struct TimeBufferType
	{
		float time;
		XMFLOAT3 padding;
	};


	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tesselationBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* timeBuffer;
};
