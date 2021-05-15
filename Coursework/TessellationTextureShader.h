#pragma once

#include "DXF.h"
#include "MyBaseShader.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "TessellationCircleMesh.h"

class TessellationTextureShader : public MyBaseShader
{
public:
	struct ManipulationBufferType
	{
		float time;
		float amplitude;
		float speed;
		float frequency;
	};

	struct TessellationBufferType
	{
		float edge0;
		float edge1;
		float edge2;
		float inside;
	};

public:
	TessellationTextureShader(ID3D11Device* device, HWND hwnd);
	~TessellationTextureShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, TessellationCircleMesh* tessellationCircleMesh, float time);

	// Setting up parameters for the domain shader.
	void setDomainShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	// Overloaded function that sets up the Hull and Domain shaders.
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* manipulationBuffer;
	ID3D11Buffer* tessellationBuffer;
};
