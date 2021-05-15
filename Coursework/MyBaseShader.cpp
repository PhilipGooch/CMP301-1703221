#include "MyBaseShader.h"

MyBaseShader::MyBaseShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
}

// Freeing up memory.
MyBaseShader::~MyBaseShader()
{
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	if (samplerStateAnisotropic)
	{
		samplerStateAnisotropic->Release();
		samplerStateAnisotropic = 0;
	}

	if (samplerStateExact)
	{
		samplerStateExact->Release();
		samplerStateExact = 0;
	}

	BaseShader::~BaseShader();
}

// Function to convert a boolean to a float. This is used for sending boolean values to the GPU. It is easier to send everything as floats.
float MyBaseShader::boolToFloat(bool boolean)
{
	return boolean ? float(1.0f) : float(0.0f);
}

// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
void MyBaseShader::createDefaultBuffer(int sizeOfBufferType, ID3D11Buffer** buffer)
{
	D3D11_BUFFER_DESC bufferDescription;
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.ByteWidth = sizeOfBufferType;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.MiscFlags = 0;
	bufferDescription.StructureByteStride = 0;
	renderer->CreateBuffer(&bufferDescription, NULL, buffer);
}

// Creating a sampler that samples a texture anisotropically.
void MyBaseShader::createAnisotropicSampler(ID3D11SamplerState** samplerState)
{
	D3D11_SAMPLER_DESC samplerDescription;
	samplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.MipLODBias = 0.0f;
	samplerDescription.MaxAnisotropy = 1;
	samplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDescription.MinLOD = 0;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDescription, samplerState);
}

// Creating a sampler that samples a texture exactly. This is used for depth maps.
void MyBaseShader::createExactSampler(ID3D11SamplerState** samplerState)
{
	D3D11_SAMPLER_DESC samplerDescription;
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDescription.BorderColor[0] = 1.0f;
	samplerDescription.BorderColor[1] = 1.0f;
	samplerDescription.BorderColor[2] = 1.0f;
	samplerDescription.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDescription, samplerState);
}

// Setting up the default vertex shader parameters. This is used for most shaders so makes sense to have in the base class.
void MyBaseShader::setVertexShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
{
	// Transpose the matrices to prepare them for the shader.
	XMMATRIX worldTranspose = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewTranspose = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionTranspose = XMMatrixTranspose(projectionMatrix);

	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldTranspose;
	dataPtr->view = viewTranspose;
	dataPtr->projection = projectionTranspose;
	deviceContext->Unmap(matrixBuffer, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
}


