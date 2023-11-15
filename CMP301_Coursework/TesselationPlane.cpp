
// Tessellation mesh.
// Builds a simple triangle mesh for tessellation demonstration
// Overrides sendData() function for different primitive topology
#include "TesselationPlane.h"

// initialise buffers and load texture.
TessellationPlane::TessellationPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float length, float width)
{
	initBuffers(device, length, width);
}

// Release resources.
TessellationPlane::~TessellationPlane()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Build triangle (with texture coordinates and normals).
void TessellationPlane::initBuffers(ID3D11Device* device, float length, float width)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = length * width;
	indexCount = ((length-1) * (width-1))*6;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	// Load the vertex array with data.

	float size = 1.f;

	int count = 0;
	for (int i = 0; i < length; i++) {

		for (int j = 0; j < width; j++) {

			vertices[count].position = XMFLOAT3(width * (j*size), 0.0f, length * (j * size));
			vertices[count].texture = XMFLOAT2(1.f/width* j , 1.f / length*i);
			vertices[count].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			++count;



		}
	}

	count = 0;
	int funny = 0;
	for (int i = 0; i <length-1; i++) {

		for (int j = 0; j < width - 1; j++) {
			int shiftLen = i * width;
			int shiftLen2 = i + 1 * width;


			indices[count] = shiftLen + j;  // Top left.
			indices[count+1] = shiftLen+j;  // Bottom left.
			indices[count + 2] = shiftLen + j + 1;  // Top right.
			indices[count + 3] = shiftLen + j + 1;  // Top right.
			indices[count + 4] = shiftLen + j;  // Bottom left.
			indices[count+5] = shiftLen + j+1;  // Bottom right.
			count += 6;
		}

	

	}

	// Load the index array with data.
	

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

void TessellationPlane::initBuffers(ID3D11Device* device)
{
}


// Override sendData() to change topology type. Control point patch list is required for tessellation.
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

