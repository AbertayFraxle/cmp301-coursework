// Colour shader.h
// Simple shader example.
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class TerrainDepthShader : public BaseShader
{

public:

	TerrainDepthShader(ID3D11Device* device, HWND hwnd);
	~TerrainDepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightmap, XMINT4 tessValues);

private:
	struct TesselationBufferType {
		XMINT4 tessDistances;
		XMMATRIX world;
		XMMATRIX view;
	};

	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tesselationBuffer;
	ID3D11SamplerState* sampleState;
};
