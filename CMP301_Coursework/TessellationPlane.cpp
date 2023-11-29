#include "TessellationPlane.h"

TessellationPlane::TessellationPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	initBuffers(device);
}

TessellationPlane::~TessellationPlane()
{
	BaseMesh::~BaseMesh();
}

void TessellationPlane::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(top);

}

void TessellationPlane::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = 100;
	indexCount = vertexCount*4;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	int vertCount = 0;
	for (int i = 0; i < 10; i ++) {
		for (int j = 0; j < 10; j ++) {
			vertices[vertCount].position = XMFLOAT3(i, 0.0f, j);
			vertices[vertCount].normal = XMFLOAT3(0, 1.0f, 0.f);

			vertCount++;
		}
	}

	int indCount = 0;
	// Load the index array with data.
	for (int i = 0; i < 9; i++) 
	{
		for (int j = 0; j < 9; j++) {

			indices[indCount] = (i*10)+j;
			indices[indCount + 1] = (i * 10) + j + 10;
			indices[indCount + 2] = (i * 10) + j + 11;
			indices[indCount + 3] = (i * 10) + j + 1;

			indCount += 4;
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

}
