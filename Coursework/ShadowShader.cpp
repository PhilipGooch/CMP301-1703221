#include "ShadowShader.h"


ShadowShader::ShadowShader(ID3D11Device* device, HWND hwnd) : MyBaseShader(device, hwnd)
{
	initShader(L"shadow_vs.cso", L"shadow_ps.cso");
}


ShadowShader::~ShadowShader()
{
	MyBaseShader::~MyBaseShader();
}

void ShadowShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	createDefaultBuffer(sizeof(MatrixBufferType), &matrixBuffer);
	createDefaultBuffer(sizeof(PointLightsBufferType), &pointLightsBuffer);
	createDefaultBuffer(sizeof(SpotLightBufferType), &spotLightBuffer);

	createAnisotropicSampler(&samplerStateAnisotropic);
	createExactSampler(&samplerStateExact);
}

void ShadowShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ShadowMap** shadowMaps, PointLight** pointLights, SpotLight* spotLight)
{
	setVertexShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, pointLights);

	deviceContext->Map(pointLightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	PointLightsBufferType* pointLightsBufferType = (PointLightsBufferType*)mappedResource.pData;
	for (int i = 0; i < 4; i++)
	{
		pointLightsBufferType->ambientColour[i] = pointLights[i]->ambient;
		pointLightsBufferType->diffuseColour[i] = pointLights[i]->diffuse;
		pointLightsBufferType->position[i] = XMFLOAT4(pointLights[i]->light->getPosition().x, pointLights[i]->light->getPosition().y, pointLights[i]->light->getPosition().z, 1.0f);
		pointLightsBufferType->direction[i] = XMFLOAT4(pointLights[i]->light->getDirection().x, pointLights[i]->light->getDirection().y, pointLights[i]->light->getDirection().z, 1.0f);
	}
	pointLightsBufferType->constant = XMFLOAT4(pointLights[0]->constant, pointLights[1]->constant, pointLights[2]->constant, pointLights[3]->constant);
	pointLightsBufferType->linear = XMFLOAT4(pointLights[0]->linear, pointLights[1]->linear, pointLights[2]->linear, pointLights[3]->linear);
	pointLightsBufferType->quadratic = XMFLOAT4(pointLights[0]->quadratic, pointLights[1]->quadratic, pointLights[2]->quadratic, pointLights[3]->quadratic);
	pointLightsBufferType->on = XMFLOAT4(pointLights[0]->on, pointLights[1]->on, pointLights[2]->on, pointLights[3]->on);
	deviceContext->Unmap(pointLightsBuffer, 0);

	deviceContext->Map(spotLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	SpotLightBufferType* spotLightBufferType = (SpotLightBufferType*)mappedResource.pData;
	spotLightBufferType->diffuse = spotLight->diffuse;
	spotLightBufferType->ambient = spotLight->ambient;
	spotLightBufferType->position = XMFLOAT4(spotLight->camera->getPosition().x, spotLight->camera->getPosition().y, spotLight->camera->getPosition().z, 1.0f);
	spotLightBufferType->direction = XMFLOAT4(spotLight->getDirection().x, spotLight->getDirection().y, spotLight->getDirection().z, 1.0f);
	spotLightBufferType->constant = spotLight->constant;
	spotLightBufferType->linear = spotLight->linear;
	spotLightBufferType->quadratic = spotLight->quadratic;
	spotLightBufferType->on = boolToFloat(spotLight->on);
	deviceContext->Unmap(spotLightBuffer, 0);

	ID3D11ShaderResourceView* depthMaps[4];
	for (int i = 0; i < 4; i++)
	{
		depthMaps[i] = shadowMaps[i]->getDepthMapSRV();
	}

	deviceContext->PSSetConstantBuffers(0, 1, &pointLightsBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &spotLightBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMaps[0]);
	deviceContext->PSSetShaderResources(2, 1, &depthMaps[1]);
	deviceContext->PSSetShaderResources(3, 1, &depthMaps[2]);
	deviceContext->PSSetShaderResources(4, 1, &depthMaps[3]);
	deviceContext->PSSetSamplers(0, 1, &samplerStateAnisotropic);
	deviceContext->PSSetSamplers(1, 1, &samplerStateExact);
}

void ShadowShader::setVertexShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, PointLight** pointLights)
{
	// Transpose the matrices to prepare them for the shader.
	XMMATRIX worldTranspose = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewTranspose = XMMatrixTranspose(viewMatrix);
	XMMATRIX projectionTranspose = XMMatrixTranspose(projectionMatrix);
	XMMATRIX lightViewTranspose[4] = { XMMatrixTranspose(pointLights[0]->light->getViewMatrix()), XMMatrixTranspose(pointLights[1]->light->getViewMatrix()), XMMatrixTranspose(pointLights[2]->light->getViewMatrix()), XMMatrixTranspose(pointLights[3]->light->getViewMatrix()) };
	XMMATRIX lightProjectionTranspose[4] = { XMMatrixTranspose(pointLights[0]->light->getProjectionMatrix()), XMMatrixTranspose(pointLights[1]->light->getProjectionMatrix()), XMMatrixTranspose(pointLights[2]->light->getProjectionMatrix()), XMMatrixTranspose(pointLights[3]->light->getProjectionMatrix()) };

	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldTranspose;
	dataPtr->view = viewTranspose;
	dataPtr->projection = projectionTranspose;
	for (int i = 0; i < 4; i++)
	{
		dataPtr->lightView[i] = lightViewTranspose[i];
		dataPtr->lightProjection[i] = lightProjectionTranspose[i];
	}
	deviceContext->Unmap(matrixBuffer, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
}
