#include "DepthShader.h"

DepthShader::DepthShader(ID3D11Device* device, HWND hwnd) : MyBaseShader(device, hwnd)
{
	initShader(L"depth_vs.cso", L"depth_ps.cso");
}

DepthShader::~DepthShader()
{
	MyBaseShader::~MyBaseShader();
}

void DepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	createDefaultBuffer(sizeof(MatrixBufferType), &matrixBuffer);
}

void DepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& lightViewMatrix, const XMMATRIX& lightProjectionMatrix)
{
	setVertexShaderParameters(deviceContext, worldMatrix, lightViewMatrix, lightProjectionMatrix);
}
