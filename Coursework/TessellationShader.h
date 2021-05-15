#pragma once

#include "DXF.h"
#include "MyBaseShader.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "TessellationCircleMesh.h"

class TessellationShader : public MyBaseShader
{
public:
	struct PointLightsBufferType
	{
		XMFLOAT4 ambientColour[4];
		XMFLOAT4 diffuseColour[4];
		XMFLOAT4 position[4];
		XMFLOAT4 constant;
		XMFLOAT4 linear;
		XMFLOAT4 quadratic;
		XMFLOAT4 on;
	};

	struct SpotLightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 position;
		XMFLOAT4 direction;
		float on;
		XMFLOAT3 padding;
	};

	struct ManipulationBufferType
	{
		float time;
		float amplitude;
		float speed;
		float frequency;
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct TessellationBufferType
	{
		float edge0;
		float edge1;
		float edge2;
		float inside;
	};

public:
	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, PointLight** pointLights, SpotLight* spotLight, TessellationCircleMesh* tessellationCircleMesh, float time, XMFLOAT3 cameraPosition);

	// Setting up parameters for the domain shader.
	void setDomainShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	// Overloaded function that sets up the Hull and Domain shaders.
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* pointLightsBuffer;
	ID3D11Buffer* spotLightBuffer;
	ID3D11Buffer* manipulationBuffer;
	ID3D11Buffer* tessellationBuffer;
};
