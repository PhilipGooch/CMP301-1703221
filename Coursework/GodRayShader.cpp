#include "GodRayShader.h"

GodRayShader::GodRayShader(ID3D11Device* device, HWND hwnd) : MyBaseShader(device, hwnd)
{
	initShader(L"godRay_vs.cso", L"godRay_ps.cso");
}

GodRayShader::~GodRayShader()
{
	MyBaseShader::~MyBaseShader();
}

void GodRayShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	createDefaultBuffer(sizeof(MatrixBufferType), &matrixBuffer);
	createDefaultBuffer(sizeof(GodRayBufferType), &godRayBuffer);

	createAnisotropicSampler(&samplerStateAnisotropic);  // maybe need _CLAMP?
}

void GodRayShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& orthoViewMatrix, const XMMATRIX& orthoMatrix, ID3D11ShaderResourceView* texture0, ID3D11ShaderResourceView* texture1, XMFLOAT3 godRayLightSource, XMFLOAT2 screenSize, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float decay, float exposure, float density, float weight, float illuminationDecay, int samples)
{
	// Transpose the matrices to prepare them for the shader.
	XMMATRIX worldTranspose = XMMatrixTranspose(worldMatrix);
	XMMATRIX orthoViewTranspose = XMMatrixTranspose(orthoViewMatrix);
	XMMATRIX orthoTranspose = XMMatrixTranspose(orthoMatrix);
	XMMATRIX viewTranspose = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionTranspose = XMMatrixTranspose(projectionMatrix);

	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldTranspose;
	dataPtr->view = orthoViewTranspose;
	dataPtr->projection = orthoTranspose;
	dataPtr->lightPosition = XMFLOAT4(godRayLightSource.x, godRayLightSource.y, godRayLightSource.z, 1.0f);
	deviceContext->Unmap(matrixBuffer, 0);

	deviceContext->Map(godRayBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	GodRayBufferType* godRayPtr = (GodRayBufferType*)mappedResource.pData;
	godRayPtr->lightPosition = XMFLOAT4(godRayLightSource.x, godRayLightSource.y, godRayLightSource.z, 1.0f);
	godRayPtr->screenSize = screenSize;
	godRayPtr->padding0 = XMFLOAT2(0, 0);
	godRayPtr->viewMatrix = viewTranspose;
	godRayPtr->projectionMatrix = projectionTranspose;
	godRayPtr->decay = decay;
	godRayPtr->exposure = exposure;
	godRayPtr->density = density;
	godRayPtr->weight = weight;
	godRayPtr->illuminationDecay = illuminationDecay;
	godRayPtr->samples = samples;
	godRayPtr->padding1 = XMFLOAT2(0, 0);
	deviceContext->Unmap(godRayBuffer, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &godRayBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture0);
	deviceContext->PSSetShaderResources(1, 1, &texture1);
}
