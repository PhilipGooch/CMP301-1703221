#pragma once

#include "DXF.h"
#include "MyBaseShader.h"

class GodRayShader : public MyBaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT4 lightPosition;
	};

	struct GodRayBufferType
	{
		XMFLOAT4 lightPosition;
		XMFLOAT2 screenSize;
		XMFLOAT2 padding0;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
		float decay;
		float exposure;
		float density;
		float weight;
		float illuminationDecay;
		int samples;
		XMFLOAT2 padding1;
	};

public:
	GodRayShader(ID3D11Device* device, HWND hwnd);
	~GodRayShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& orthoViewMatrix, const XMMATRIX& orthoMatrix, ID3D11ShaderResourceView* texture0, ID3D11ShaderResourceView* texture1, XMFLOAT3 godRayLightSource, XMFLOAT2 screenSize, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float decay, float exposure, float density, float weight, float illuminationDecay, int samples);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* godRayBuffer;
};
