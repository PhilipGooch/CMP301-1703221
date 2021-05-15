#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class MyBaseShader : public BaseShader
{
public:
	MyBaseShader(ID3D11Device* device, HWND hwnd);
	~MyBaseShader();

protected:
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	void createDefaultBuffer(int sizeOfBufferType, ID3D11Buffer** buffer);

	// Creating a sampler that samples a texture anisotropically.
	void createAnisotropicSampler(ID3D11SamplerState** samplerState);

	// Creating a sampler that samples a texture exactly. This is used for depth maps.
	void createExactSampler(ID3D11SamplerState** samplerState);

	// Setting up the default vertex shader parameters. This is used for most shaders so makes sense to have in the base class.
	void setVertexShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix);

	// Function to convert a boolean to a float. This is used for sending boolean values to the GPU. It is easier to send everything as floats.
	float boolToFloat(bool boolean);

protected:
	// Structs to hold sampler data.
	ID3D11SamplerState* samplerStateAnisotropic;
	ID3D11SamplerState* samplerStateExact;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
};

