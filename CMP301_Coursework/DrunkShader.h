#pragma once

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class DrunkShader : public BaseShader
{
public:
	DrunkShader(ID3D11Device* device, HWND hwnd);
	~DrunkShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture,float time);

private:
	struct TimeBufferType
	{
		float time;
		XMFLOAT3 padding;
	};



	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* timeBuffer;
};

