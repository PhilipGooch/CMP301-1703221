#include "TessellationCircleMesh.h"

// Initialise buffers and lad texture.
TessellationCircleMesh::TessellationCircleMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution, int radius) :
	resolution(resolution),
	radius(radius),
	amplitude(0.015f),
	frequency(4.5f),
	speed(5.78f),
	edge0(64),
	edge1(64),
	edge2(64),
	inside(64)
{
	initBuffers(device);
}

// Release resources.
TessellationCircleMesh::~TessellationCircleMesh()
{
	// Run parent deconstructor.
	BaseMesh::~BaseMesh();
}

// Build quad mesh.
void TessellationCircleMesh::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = resolution + 1;
	indexCount = resolution * 3;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	float angle = 0;
	float theta = 3.1415962f * 2 / resolution;
	for (int i = 0; i < resolution; i++)
	{
		vertices[i].position = XMFLOAT3(cosf(angle) * radius, 0.0f, sinf(angle) * radius);
		vertices[i].texture = XMFLOAT2((cosf(-angle) / 2) + 0.5, (sinf(-angle) / 2) + 0.5);
		vertices[i].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		angle += theta;
	}
	vertices[resolution].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertices[resolution].texture = XMFLOAT2(0.5f, 0.5f);
	vertices[resolution].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	int index = 0;
	for (int i = 0; i < resolution - 1; i++)
	{
		indices[index++] = resolution;
		indices[index++] = i + 1;
		indices[index++] = i;
	}
	indices[index++] = resolution;
	indices[index++] = 0;
	indices[index++] = resolution - 1;

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

// Override sendData() to change topology type. Control point patch list is required for tessellation.
void TessellationCircleMesh::sendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
}


