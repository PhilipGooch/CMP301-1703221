#include "StencilShader.h"

StencilShader::StencilShader(ID3D11Device* device, HWND hwnd) : MyBaseShader(device, hwnd)
{
	initShader(L"stencil_vs.cso", L"stencil_ps.cso");
}


StencilShader::~StencilShader()
{
	MyBaseShader::~MyBaseShader();
}

void StencilShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	createDefaultBuffer(sizeof(MatrixBufferType), &matrixBuffer);

	createAnisotropicSampler(&samplerStateAnisotropic);

	// Sampler for stencil texture.
	createExactSampler(&samplerStateExact);
}


void StencilShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* stencil)
{
	setVertexShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &stencil);
	deviceContext->PSSetSamplers(0, 1, &samplerStateAnisotropic);
	deviceContext->PSSetSamplers(1, 1, &samplerStateExact);
}
