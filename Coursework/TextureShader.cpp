#include "TextureShader.h"

TextureShader::TextureShader(ID3D11Device* device, HWND hwnd) : MyBaseShader(device, hwnd)
{
	initShader(L"texture_vs.cso", L"texture_ps.cso");
}


TextureShader::~TextureShader()
{
	MyBaseShader::~MyBaseShader();
}

void TextureShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	createDefaultBuffer(sizeof(MatrixBufferType), &matrixBuffer);

	createAnisotropicSampler(&samplerStateAnisotropic);
}

void TextureShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture)
{
	setVertexShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &samplerStateAnisotropic);
}
