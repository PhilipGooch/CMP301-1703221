#include "AlphaShader.h"

AlphaShader::AlphaShader(ID3D11Device* device, HWND hwnd) : MyBaseShader(device, hwnd)
{
	initShader(L"alpha_vs.cso", L"alpha_ps.cso");
}

AlphaShader::~AlphaShader()
{
	MyBaseShader::~MyBaseShader();
}

void AlphaShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	createDefaultBuffer(sizeof(MatrixBufferType), &matrixBuffer);

	createAnisotropicSampler(&samplerStateAnisotropic);
}

void AlphaShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture)
{
	setVertexShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &samplerStateAnisotropic);
}
