#include "TessellationTextureShader.h"

TessellationTextureShader::TessellationTextureShader(ID3D11Device* device, HWND hwnd) : MyBaseShader(device, hwnd)
{
	initShader(L"tessellation_vs.cso", L"tessellation_hs.cso", L"tessellation_ds.cso", L"tessellationTexture_ps.cso");
}

TessellationTextureShader::~TessellationTextureShader()
{
	MyBaseShader::~MyBaseShader();
}

void TessellationTextureShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void TessellationTextureShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	initShader(vsFilename, psFilename);

	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);

	createDefaultBuffer(sizeof(MatrixBufferType), &matrixBuffer);
	createDefaultBuffer(sizeof(ManipulationBufferType), &manipulationBuffer);
	createDefaultBuffer(sizeof(TessellationBufferType), &tessellationBuffer);

	createAnisotropicSampler(&samplerStateAnisotropic);
}

void TessellationTextureShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, TessellationCircleMesh* tessellationCircleMesh, float time)
{
	setDomainShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	deviceContext->Map(manipulationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ManipulationBufferType* manipulationBufferType = (ManipulationBufferType*)mappedResource.pData;
	manipulationBufferType->time = time;
	manipulationBufferType->speed = tessellationCircleMesh->speed;
	manipulationBufferType->amplitude = tessellationCircleMesh->amplitude;
	manipulationBufferType->frequency = tessellationCircleMesh->frequency;
	deviceContext->Unmap(manipulationBuffer, 0);

	deviceContext->Map(tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TessellationBufferType* tessellationPtr = (TessellationBufferType*)mappedResource.pData;
	tessellationPtr->edge0 = tessellationCircleMesh->edge0;
	tessellationPtr->edge1 = tessellationCircleMesh->edge1;
	tessellationPtr->edge2 = tessellationCircleMesh->edge2;
	tessellationPtr->inside = tessellationCircleMesh->inside;
	deviceContext->Unmap(tessellationBuffer, 0);

	deviceContext->HSSetConstantBuffers(0, 1, &tessellationBuffer);
	deviceContext->DSSetConstantBuffers(1, 1, &manipulationBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &samplerStateAnisotropic);
}

void TessellationTextureShader::setDomainShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
{
	// Transpose the matrices to prepare them for the shader.
	XMMATRIX worldTranspose = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewTranspose = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionTranspose = XMMatrixTranspose(projectionMatrix);

	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldTranspose;
	dataPtr->view = viewTranspose;
	dataPtr->projection = projectionTranspose;
	deviceContext->Unmap(matrixBuffer, 0);

	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);
}
