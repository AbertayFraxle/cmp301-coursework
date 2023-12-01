// tessellation shader.cpp
#include "TerrainTessellationShader.h"


TerrainTessellationShader::TerrainTessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"terrain_tess_vs.cso", L"terrain_tess_hs.cso", L"terrain_tess_ds.cso", L"terrain_tess_ps.cso");
}


TerrainTessellationShader::~TerrainTessellationShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void TerrainTessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
	renderer->CreateSamplerState(&samplerDesc, &shadowSampleState);

	D3D11_BUFFER_DESC tesselationBufferDesc;
	tesselationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tesselationBufferDesc.ByteWidth = sizeof(TesselationBufferType);
	tesselationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tesselationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tesselationBufferDesc.MiscFlags = 0;
	tesselationBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&tesselationBufferDesc, NULL, &tesselationBuffer);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);


}

void TerrainTessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TerrainTessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, int tessAmount, ID3D11ShaderResourceView* lowTex, ID3D11ShaderResourceView* highTex, ID3D11ShaderResourceView* lowNorm, ID3D11ShaderResourceView* highNorm, ID3D11ShaderResourceView* heightmap, ShadowMap* shadowMap[LIGHTCOUNT],Light * lights[LIGHTCOUNT],ShadowMap* cameraDepth)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;




	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);



	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	for (int i = 0; i < LIGHTCOUNT; i++) {
		XMMATRIX tLightViewMatrix = XMMatrixTranspose(lights[i]->getViewMatrix());
		XMMATRIX tLightProjectionMatrix;
		if (lights[i]->getConeAngle() == 0) {
			tLightProjectionMatrix = XMMatrixTranspose(lights[i]->getOrthoMatrix());
		}
		else {
			tLightProjectionMatrix = XMMatrixTranspose(lights[i]->getProjectionMatrix());
		}
		dataPtr->lightView[i] = tLightViewMatrix;
		dataPtr->lightProjection[i] = tLightProjectionMatrix;
	}

	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &matrixBuffer);
	
	deviceContext->Map(tesselationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TesselationBufferType* tessPtr = (TesselationBufferType*)mappedResource.pData;
	tessPtr->tesselationAmount.x = tessAmount;
	tessPtr->world = tworld;
	tessPtr->view = tview;
	tessPtr->projection = tproj;
	deviceContext->Unmap(tesselationBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tesselationBuffer);


	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;

	for (int i = 0; i < LIGHTCOUNT; i++) {
		lightPtr->ambient[i] = lights[i]->getAmbientColour();
		lightPtr->diffuse[i] = lights[i]->getDiffuseColour();
		lightPtr->position[i] = XMFLOAT4(lights[i]->getPosition().x, lights[i]->getPosition().y, lights[i]->getPosition().z, 1);
		lightPtr->direction[i] = XMFLOAT4(lights[i]->getDirection().x, lights[i]->getDirection().y, lights[i]->getDirection().z, 1);
		lightPtr->factors[i] = XMFLOAT4(0.5, 0.125, 0, 0);
		lightPtr->coneangle[i] = XMFLOAT4(lights[i]->getConeAngle(), 0, 0, 0);
	}

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	

	deviceContext->DSSetSamplers(0, 1, &sampleState);

	ID3D11ShaderResourceView* camD = cameraDepth->getDepthMapSRV();
	deviceContext->HSSetShaderResources(0, 1, &camD);
	deviceContext->HSSetSamplers(0, 1, &shadowSampleState);

	deviceContext->DSSetShaderResources(0, 1, &heightmap);

	deviceContext->PSSetShaderResources(0, 1, &lowTex);
	deviceContext->PSSetShaderResources(1, 1, &highTex);
	deviceContext->PSSetShaderResources(2, 1, &lowNorm);
	deviceContext->PSSetShaderResources(3, 1, &highNorm);

	for (int i = 0; i < LIGHTCOUNT; i++) {
		ID3D11ShaderResourceView* map = shadowMap[i]->getDepthMapSRV();

		deviceContext->PSSetShaderResources(4 + i, 1, &map);
	}

	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &shadowSampleState);

}



