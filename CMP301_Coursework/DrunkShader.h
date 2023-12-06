#pragma once

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class DrunkShader : public BaseShader
{
public:
	DrunkShader(ID3D11Device* device, HWND hwnd);
	~DrunkShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, float time, float doubleVision, float intensity);

private:
	struct TimeBufferType
	{
		float time;
		float doubleVision;
		float intensity;
		float padding;
	};



	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* timeBuffer;
};

