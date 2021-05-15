#include "TessellationShader.h"

TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : MyBaseShader(device, hwnd)
{
	initShader(L"tessellation_vs.cso", L"tessellation_hs.cso", L"tessellation_ds.cso", L"tessellation_ps.cso");
}

TessellationShader::~TessellationShader()
{
	MyBaseShader::~MyBaseShader();
}

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
}

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	initShader(vsFilename, psFilename);

	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);

	createDefaultBuffer(sizeof(MatrixBufferType), &matrixBuffer);
	createDefaultBuffer(sizeof(PointLightsBufferType), &pointLightsBuffer);
	createDefaultBuffer(sizeof(SpotLightBufferType), &spotLightBuffer);
	createDefaultBuffer(sizeof(ManipulationBufferType), &manipulationBuffer);
	createDefaultBuffer(sizeof(TessellationBufferType), &tessellationBuffer);

	createAnisotropicSampler(&samplerStateAnisotropic);
}

void TessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, PointLight** pointLights, SpotLight* spotLight, TessellationCircleMesh* tessellationCircleMesh, float time, XMFLOAT3 cameraPosition)
{
	setDomainShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	// POINT LIGHTS 
	deviceContext->Map(pointLightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	PointLightsBufferType* pointLightsBufferType = (PointLightsBufferType*)mappedResource.pData;
	for (int i = 0; i < 4; i++)
	{
		pointLightsBufferType->ambientColour[i] = pointLights[i]->ambient;
		pointLightsBufferType->diffuseColour[i] = pointLights[i]->diffuse;
		pointLightsBufferType->position[i] = XMFLOAT4(pointLights[i]->position.x, pointLights[i]->position.y, pointLights[i]->position.z, 1.0f);
	}
	pointLightsBufferType->constant = XMFLOAT4(pointLights[0]->constant, pointLights[1]->constant, pointLights[2]->constant, pointLights[3]->constant);
	pointLightsBufferType->linear = XMFLOAT4(pointLights[0]->linear, pointLights[1]->linear, pointLights[2]->linear, pointLights[3]->linear);
	pointLightsBufferType->quadratic = XMFLOAT4(pointLights[0]->quadratic, pointLights[1]->quadratic, pointLights[2]->quadratic, pointLights[3]->quadratic);
	pointLightsBufferType->on = XMFLOAT4(pointLights[0]->on, pointLights[1]->on, pointLights[2]->on, pointLights[3]->on);
	deviceContext->Unmap(pointLightsBuffer, 0);

	// SPOT LIGHT 
	deviceContext->Map(spotLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	SpotLightBufferType* spotLightBufferType = (SpotLightBufferType*)mappedResource.pData;
	spotLightBufferType->diffuse = spotLight->diffuse;
	spotLightBufferType->ambient = spotLight->ambient;
	spotLightBufferType->position = XMFLOAT4(spotLight->camera->getPosition().x, spotLight->camera->getPosition().y, spotLight->camera->getPosition().z, 1.0f);
	spotLightBufferType->direction = XMFLOAT4(spotLight->getDirection().x, spotLight->getDirection().y, spotLight->getDirection().z, 1.0f);
	spotLightBufferType->on = boolToFloat(spotLight->on);
	deviceContext->Unmap(spotLightBuffer, 0);

	// MANIPULATION 
	deviceContext->Map(manipulationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ManipulationBufferType* manipulationBufferType = (ManipulationBufferType*)mappedResource.pData;
	manipulationBufferType->time = time;
	manipulationBufferType->speed = tessellationCircleMesh->speed;
	manipulationBufferType->amplitude = tessellationCircleMesh->amplitude;
	manipulationBufferType->frequency = tessellationCircleMesh->frequency;
	manipulationBufferType->cameraPosition = cameraPosition;
	deviceContext->Unmap(manipulationBuffer, 0);

	// TESSELLATION
	deviceContext->Map(tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TessellationBufferType* tessellationPtr = (TessellationBufferType*)mappedResource.pData;
	tessellationPtr->edge0 = tessellationCircleMesh->edge0;
	tessellationPtr->edge1 = tessellationCircleMesh->edge1;
	tessellationPtr->edge2 = tessellationCircleMesh->edge2;
	tessellationPtr->inside = tessellationCircleMesh->inside;
	deviceContext->Unmap(tessellationBuffer, 0);
	
	deviceContext->HSSetConstantBuffers(0, 1, &tessellationBuffer);
	deviceContext->DSSetConstantBuffers(1, 1, &manipulationBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &pointLightsBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &spotLightBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &samplerStateAnisotropic);
}

void TessellationShader::setDomainShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
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
