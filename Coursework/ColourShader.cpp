#include "ColourShader.h"

ColourShader::ColourShader(ID3D11Device* device, HWND hwnd) : MyBaseShader(device, hwnd)
{
	initShader(L"colour_vs.cso", L"colour_ps.cso");
}

ColourShader::~ColourShader()
{
	MyBaseShader::~MyBaseShader();
}

void ColourShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	createDefaultBuffer(sizeof(MatrixBufferType), &matrixBuffer);
	createDefaultBuffer(sizeof(ColourBufferType), &colourBuffer);
}

void ColourShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMFLOAT4 colour)
{
	setVertexShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	deviceContext->Map(colourBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ColourBufferType* colourPtr = (ColourBufferType*)mappedResource.pData;
	colourPtr->colour = colour;
	deviceContext->Unmap(colourBuffer, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &colourBuffer);
}
