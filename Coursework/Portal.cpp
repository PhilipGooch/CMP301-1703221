#include "Portal.h"

Portal::Portal(HWND hwnd, D3D* renderer, Input* input, int screenWidth, int screenHeight, XMFLOAT3 position, 
			   StencilShader* stencilShader, ColourShader* colourShader, AlphaShader* alphaShader, TessellationTextureShader* tessellationTextureShader, GodRayShader* godRayShader, DepthShader* depthShader, ShadowShader* shadowShader, TessellationShader* tessellationShader, TextureShader* textureShader,
			   TextureManager* textureManager, int ID, float time, XMFLOAT3 godRayLightSource, float decay, float exposure, float density, float weight, float illuminationDecay, int samples, bool godRaysOn) :
	renderer(renderer),
	position(position),
	stencilShader(stencilShader),
	colourShader(colourShader),
	alphaShader(alphaShader),		
	godRayShader(godRayShader),
	tessellationTextureShader(tessellationTextureShader),
	depthShader(depthShader),
	shadowShader(shadowShader),
	tessellationShader(tessellationShader),
	textureManager(textureManager), 
	otherPortal(nullptr),
	ID(ID),
	time(time),
	godRayLightSource(godRayLightSource),
	decay(decay),
	exposure(exposure),
	density(density),
	weight(weight),
	illuminationDecay(illuminationDecay),
	samples(samples),
	godRaysOn(godRaysOn),
	textureShader(textureShader)
{
	// Ortho mesh for combined pass. Combining the stencil and scene pass.
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);

	// Circle mesh's for portal textures.
	stencilMesh = new CircleMesh(renderer->getDevice(), renderer->getDeviceContext(), 64, 2.0f);
	borderMesh = new CircleMesh(renderer->getDevice(), renderer->getDeviceContext(), 64, 2.2f);

	// Pool mesh.
	tessellationCircleMesh = new TessellationCircleMesh(renderer->getDevice(), renderer->getDeviceContext(), 64, 5);

	// Textures for different passes.
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, 0.1f, 100.0f);
	stencilTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, 0.1f, 100.0f);
	combinedTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, 0.1f, 100.0f);
	blackTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, 0.1f, 100.0f);
	godRayTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, 0.1f, 100.0f);

	// Portal's camera. This moves in relation to the main camera.
	camera = new FPCamera(input, screenWidth, screenHeight, hwnd);
}

Portal::~Portal()
{
}

void Portal::updateCamera(XMFLOAT3 playerPosition, XMFLOAT3 playerLookAt)	
{
	// Manipulating the porta's camera in relation to the main camera.
	camera->setPosition(otherPortal->position.x + (otherPortal->position.x - playerPosition.x) - (otherPortal->position.x - position.x), otherPortal->position.y + (playerPosition.y - position.y), 32 + 32 - playerPosition.z);
	camera->setRotation(playerLookAt.x, 180 + playerLookAt.y, playerLookAt.z);
	camera->update();
}

void Portal::stencilPass(FPCamera* playerCamera)
{
	stencilTexture->setRenderTarget(renderer->getDeviceContext());
	stencilTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// Using the main camera's view matrix to display the portal stencil from it's perspective.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = playerCamera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Using alpha shader to render a green stencil in the shape of the portal, on a white background.
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(0, PI, 0);
	worldMatrix *= XMMatrixTranslation(position.x, position.y, position.z);
	stencilMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"green"));
	textureShader->render(renderer->getDeviceContext(), stencilMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}

void Portal::combinedPass()
{
	combinedTexture->setRenderTarget(renderer->getDeviceContext());
	combinedTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX projectionMatrix = renderer->getOrthoMatrix();

	// Using the stencil shader to render a texture that only shows the scene texture where the stencil is. 
	// The rest of the texture is left black with an alpha value of 0.
	orthoMesh->sendData(renderer->getDeviceContext());
	if(godRaysOn)
		stencilShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, godRayTexture->getShaderResourceView(), stencilTexture->getShaderResourceView());
	else
		stencilShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, sceneTexture->getShaderResourceView(), stencilTexture->getShaderResourceView());
	stencilShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}

void Portal::godRayPass(FPCamera* playerCamera)
{
	godRayTexture->setRenderTarget(renderer->getDeviceContext());
	godRayTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Using the god ray shader to render the god rays using the black texture and scene texture.
	orthoMesh->sendData(renderer->getDeviceContext());
	godRayShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, blackTexture->getShaderResourceView(), sceneTexture->getShaderResourceView(), godRayLightSource, XMFLOAT2(1200, 675), viewMatrix, projectionMatrix, decay, exposure, density, weight, illuminationDecay, samples);
	godRayShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}

void Portal::border(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	// Rendering the border of the portal. This is an oval. The portal texture is drawn on top of it.
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(0, PI, 0);
	worldMatrix *= XMMatrixTranslation(position.x, position.y, position.z - 0.005f);
	borderMesh->sendData(renderer->getDeviceContext());
	// Orange
	if (ID == 0)
		colourShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, XMFLOAT4(1.0f, 0.6f, 0.0f, 1.0f));
	// Blue
	else if(ID == 1)
		colourShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	colourShader->render(renderer->getDeviceContext(), borderMesh->getIndexCount());
}

void Portal::blackPass(FPCamera* playerCamera, float planeSize, SphereMesh* sphereMesh, TextureManager* textureMgr, TextureShader* textureShader)
{
	blackTexture->setRenderTarget(renderer->getDeviceContext());
	blackTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Drawing the pool with a tessellation texture shader. No lighting applied to the black pass texture.
	worldMatrix *= XMMatrixTranslation(32 / 4 * 3, 0.5f, 32 / 4); // change hard coded values... make pool class...
	tessellationCircleMesh->sendData(renderer->getDeviceContext());
	tessellationTextureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"water"), tessellationCircleMesh, time);
	tessellationTextureShader->render(renderer->getDeviceContext(), tessellationCircleMesh->getIndexCount());
	
	// Spheres
	// Passing the spheres a black texture.					// get rid of hard coded values...
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f - 1.5f, 1, planeSize * 0.5f + 1.5f);
	sphereMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"black"));
	textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f + 1.5f, 1, planeSize * 0.5f + 1.5f);
	sphereMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"black"));
	textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f + 1.5f, 1, planeSize * 0.5f - 1.5f);
	sphereMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"black"));
	textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f - 1.5f, 1, planeSize * 0.5f - 1.5f);
	sphereMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"black"));
	textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();

}

// This is the same as the main scene pass except no portals are drawn.
void Portal::scenePass(ShadowMap** shadowMaps, PointLight** pointLights, SpotLight* spotLight, float planeSize, SphereMesh* sphereMesh, MyPlaneMesh* planeMesh)
{
	sceneTexture->setRenderTarget(renderer->getDeviceContext());
	sceneTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Pool
	worldMatrix *= XMMatrixTranslation(planeSize / 4 * 3, 0.5f, planeSize / 4);
	tessellationCircleMesh->sendData(renderer->getDeviceContext());
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"water"), pointLights, spotLight, tessellationCircleMesh, time, camera->getPosition());
	tessellationShader->render(renderer->getDeviceContext(), tessellationCircleMesh->getIndexCount());


	// Light indicators
	for (int i = 0; i < 4; i++)
	{
		if (pointLights[i]->on)
		{
			worldMatrix = renderer->getWorldMatrix();
			worldMatrix *= XMMatrixScaling(0.25f, 0.25f, 0.25f);
			worldMatrix *= XMMatrixTranslation(pointLights[i]->position.x, pointLights[i]->position.y, pointLights[i]->position.z);
			sphereMesh->sendData(renderer->getDeviceContext());
			colourShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, pointLights[i]->diffuse);
			colourShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());
		}
	}

	// Spheres
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f - 1.5f, 1, planeSize * 0.5f - 1.5f);
	sphereMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"white"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f - 1.5f, 1, planeSize * 0.5f + 1.5f);
	sphereMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"white"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f + 1.5f, 1, planeSize * 0.5f - 1.5f);
	sphereMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"white"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f + 1.5f, 1, planeSize * 0.5f + 1.5f);
	sphereMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"white"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	// Room
	// Bottom
	worldMatrix = renderer->getWorldMatrix();
	planeMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// top
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(PI, 0.0f, 0.0f);
	worldMatrix *= XMMatrixTranslation(0.0f, planeSize, planeSize);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// left
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(-PI / 2, -PI / 2, 0.0f);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// right
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(-PI / 2, PI / 2, 0.0f);
	worldMatrix *= XMMatrixTranslation(planeSize, 0.0f, planeSize);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// front
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(-PI / 2, 0.0f, 0.0f);
	worldMatrix *= XMMatrixTranslation(0.0f, 0.0f, planeSize);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// back
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(PI / 2, 0.0f, 0.0f);
	worldMatrix *= XMMatrixTranslation(0.0f, planeSize, 0.0f);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureManager->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}

