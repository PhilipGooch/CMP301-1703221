#pragma once

#include "DXF.h"
#include "MyBaseShader.h"
#include "PointLight.h"
#include "SpotLight.h"

using namespace std;
using namespace DirectX;

class ShadowShader : public MyBaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[4];
		XMMATRIX lightProjection[4];
	};

	struct PointLightsBufferType
	{
		XMFLOAT4 ambientColour[4];
		XMFLOAT4 diffuseColour[4];
		XMFLOAT4 position[4];
		XMFLOAT4 direction[4];
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
		float constant;
		float linear;
		float quadratic;
		float on;
	};

public:
	ShadowShader(ID3D11Device* device, HWND hwnd);
	~ShadowShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ShadowMap** depthMaps, PointLight** pointLights, SpotLight* spotLight);
	void setVertexShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, PointLight** pointLights);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* pointLightsBuffer;

	ID3D11Buffer* spotLightBuffer;
};

