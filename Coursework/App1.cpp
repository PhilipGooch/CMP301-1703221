// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1() :
	time(0.0f),
	planeSize(32)
{
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Loading textures.
	textureMgr->loadTexture(L"red", L"res/red.png");
	textureMgr->loadTexture(L"green", L"res/green.png");
	textureMgr->loadTexture(L"blue", L"res/blue.png");
	textureMgr->loadTexture(L"yellow", L"res/yellow.png");
	textureMgr->loadTexture(L"magenta", L"res/magenta.png");
	textureMgr->loadTexture(L"cyan", L"res/cyan.png");
	textureMgr->loadTexture(L"white", L"res/white.png");
	textureMgr->loadTexture(L"orange", L"res/orange.png");
	textureMgr->loadTexture(L"black", L"res/black.png");
	textureMgr->loadTexture(L"concrete", L"res/concreteWall.png");
	textureMgr->loadTexture(L"water", L"res/water.jpg");
	textureMgr->loadTexture(L"camel", L"res/camel.jpg");

	// Creating shader handler classes.
	stencilShader = new StencilShader(renderer->getDevice(), hwnd);
	alphaShader = new AlphaShader(renderer->getDevice(), hwnd);
	colourShader = new ColourShader(renderer->getDevice(), hwnd);
	tessellationShader = new TessellationShader(renderer->getDevice(), hwnd);
	godRayShader = new GodRayShader(renderer->getDevice(), hwnd);
	tessellationTextureShader = new TessellationTextureShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);

	// Creating render textures for God ray shader.
	blackTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, 0.1f, 100.0f);
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, 0.1f, 100.0f);

	// World position for light source for the God Ray
	godRayLightSource = XMFLOAT3(planeSize / 4 * 3, 0.5f, planeSize / 4);
	
	// Creating instances of portals. This should be re-designed. I am passing all the shaders and most variables from this class...
	orangePortal = new Portal(hwnd, renderer, input, screenWidth, screenHeight, XMFLOAT3(planeSize / 4, 3.5f, planeSize), stencilShader, colourShader, alphaShader, tessellationTextureShader, godRayShader, depthShader, shadowShader, tessellationShader, textureShader, textureMgr, 0, time, godRayLightSource, decay, exposure, density, weight, illuminationDecay, samples, godRaysOn);	// <--- no need to pass time to constructor. it changes...
	bluePortal = new Portal(hwnd, renderer, input, screenWidth, screenHeight, XMFLOAT3(planeSize / 4 * 3, 3.5f, planeSize), stencilShader, colourShader, alphaShader, tessellationTextureShader, godRayShader, depthShader, shadowShader, tessellationShader, textureShader, textureMgr, 1, time, godRayLightSource, decay, exposure, density, weight, illuminationDecay, samples, godRaysOn);
	orangePortal->setOtherPortal(bluePortal);
	bluePortal->setOtherPortal(orangePortal);

	// Creating shadow maps.
	int shadowmapWidth = 4096;
	int shadowmapHeight = 4096;
	for (int i = 0; i < 4; i++)
	{
		shadowMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	}

	// Creating instances of point lights. First two are actually used as spotlights...
	pointLights[0] = new PointLight(XMFLOAT3(planeSize * 0.5f + 4, 5, planeSize * 0.5f + 4), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f), 0.412f, 0.217f, 0.004f, true, planeSize, 0);
	pointLights[1] = new PointLight(XMFLOAT3(planeSize * 0.5f + 4, 5, planeSize * 0.5f - 4), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f), 0.412f, 0.217f, 0.004f, true, planeSize, 1);
	pointLights[2] = new PointLight(XMFLOAT3(planeSize * 0.75f, 7.0f, planeSize * 0.5f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f), 0.412f, 0.108f, 0.004f, true, planeSize, 2);
	pointLights[3] = new PointLight(XMFLOAT3(planeSize / 4 * 3, 1.5, planeSize / 4), XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f), XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f), 0.412f, 0.108f, 0.004f, true, planeSize, 3);

	// Creating spotlight from camera.
	spotLight = new SpotLight(camera, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f));

	// Creating ortho mesh the size of the screen.
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);
	smallOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, 350, 200);

	// Creating small ortho mesh for debugging shadows.
	depthOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, 430, 230);

	// Creating pool mesh. This should be it's own class...
	tessellationCircleMesh = new TessellationCircleMesh(renderer->getDevice(), renderer->getDeviceContext(), 64, 5);

	// Creating sphere mesh for light indicators.
	sphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	// Creating plane mesh for walls.
	planeMesh = new MyPlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), planeSize + 1);

	// Setting up the camera.
	camera->setPosition(2.8f, 7.25f, 0.27f);
	camera->setRotation(14.0f, 43.0f, 0.0f);


	//wireframeToggle = true;

	pointLights[0]->on = false;
	pointLights[1]->on = false;
	pointLights[2]->on = true;
	godRaysOn = true;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Updating time. Passing this to world and portals.
	time += timer->getTime();
	orangePortal->time = time;
	bluePortal->time = time;

	// Updating portals' god ray boolean
	orangePortal->godRaysOn = godRaysOn;
	bluePortal->godRaysOn = godRaysOn;

	// Updating the positions of the lights.
	for (int i = 0; i < 4; i++)
	{
		pointLights[i]->updatePosition(time);
		pointLights[i]->updateDirection();
	}
	spotLight->rotate();


	// Teleporting camera when walking through a portal.
	float offset = 0.0f;
	if (camera->getPosition().x > orangePortal->position.x - 2 * 0.6 && camera->getPosition().x < orangePortal->position.x + 2 * 0.6 &&
		camera->getPosition().y > orangePortal->position.y - 2 && camera->getPosition().y < orangePortal->position.y + 2 &&
		camera->getPosition().z > planeSize - offset)
	{
		camera->setPosition(bluePortal->position.x - (camera->getPosition().x - orangePortal->position.x), bluePortal->position.y + (camera->getPosition().y - orangePortal->position.y), bluePortal->position.z - offset);
		camera->setRotation(camera->getRotation().x, camera->getRotation().y + 180, camera->getRotation().z);
	}
	else if (camera->getPosition().x > bluePortal->position.x - 2 * 0.6 && camera->getPosition().x < bluePortal->position.x + 2 * 0.6 &&
		camera->getPosition().y > bluePortal->position.y - 2 && camera->getPosition().y < bluePortal->position.y + 2 &&
		camera->getPosition().z > planeSize - offset)
	{
		camera->setPosition(orangePortal->position.x - (camera->getPosition().x - bluePortal->position.x), orangePortal->position.y + (camera->getPosition().y - bluePortal->position.y), orangePortal->position.z - offset);
		camera->setRotation(camera->getRotation().x, camera->getRotation().y + 180, camera->getRotation().z);
	}

	// Clamping camera to the room.
	float border = 0.2f;
	if (camera->getPosition().x < border)
	{
		camera->setPosition(border, camera->getPosition().y, camera->getPosition().z);
	}
	if (camera->getPosition().y < 3.5)
	{
		camera->setPosition(camera->getPosition().x, 3.5, camera->getPosition().z);
	}
	if (camera->getPosition().z < border)
	{
		camera->setPosition(camera->getPosition().x, camera->getPosition().y, border);
	}
	if (camera->getPosition().x > planeSize - border)
	{
		camera->setPosition(planeSize - border, camera->getPosition().y, camera->getPosition().z);
	}
	if (camera->getPosition().y > planeSize - border)
	{
		camera->setPosition(camera->getPosition().x, planeSize - border, camera->getPosition().z);
	}
	if (camera->getPosition().z > planeSize - border && 
		(camera->getPosition().x < orangePortal->position.x - 2 * 0.6 || (camera->getPosition().x > orangePortal->position.x + 2 * 0.6f && camera->getPosition().x < bluePortal->position.x - 2 * 0.6f) || camera->getPosition().x > bluePortal->position.x + 2 * 0.6f))
	{
		camera->setPosition(camera->getPosition().x, camera->getPosition().y, planeSize - border);
	}

	// Updating the portal's camera's positions relative to the main camera.
	camera->update();
	orangePortal->updateCamera(XMFLOAT3(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z), XMFLOAT3(camera->getRotation().x, camera->getRotation().y, camera->getRotation().z));
	bluePortal->updateCamera(XMFLOAT3(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z), XMFLOAT3(camera->getRotation().x, camera->getRotation().y, camera->getRotation().z));


	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	// Black passes - Used for God rays.
	// Depth passes - Used for generating shadows. This is done for each light.
	// Scene passes - Used for rendering the entire scene with lighting.
	// God ray passes - Used for generating god rays from black pass and combining with the scene texture.
	// Stencil passes - Used to generate the stencils for the portals.
	// Combined passes - Used to sample the parts of the scene to render for the portals.

	// Depth passes are the same for the real scene and in the portals.
	for (int i = 0; i < 4; i++)
	{
		depthPass(i);
	}

	// Portal 1.
	if (godRaysOn)
		orangePortal->blackPass(camera, planeSize, sphereMesh, textureMgr, textureShader);
	orangePortal->scenePass(shadowMaps, pointLights, spotLight, planeSize, sphereMesh, planeMesh);
	if (godRaysOn)
		orangePortal->godRayPass(camera);
	orangePortal->stencilPass(camera);
	orangePortal->combinedPass();

	// Portal 2.
	if (godRaysOn)
		bluePortal->blackPass(camera, planeSize, sphereMesh, textureMgr, textureShader);
	bluePortal->scenePass(shadowMaps, pointLights, spotLight, planeSize, sphereMesh, planeMesh);
	if (godRaysOn)
		bluePortal->godRayPass(camera);
	bluePortal->stencilPass(camera);
	bluePortal->combinedPass();

	// Main camera.
	if (godRaysOn)
		blackPass();
	scenePass();
	finalPass();
	return true;
}

bool App1::blackPass()
{
	blackTexture->setRenderTarget(renderer->getDeviceContext());
	blackTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Drawing the pool with a tessellation texture shader. No lighting applied to the black pass texture.
	worldMatrix *= XMMatrixTranslation(planeSize / 4 * 3, 0.5f, planeSize / 4);
	tessellationCircleMesh->sendData(renderer->getDeviceContext());
	tessellationTextureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), tessellationCircleMesh, time);
	tessellationTextureShader->render(renderer->getDeviceContext(), tessellationCircleMesh->getIndexCount());

	// Drawing everything else in the scene black. This will obscure the God rays.

	// Spheres
	// Passing the spheres a black texture.
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f - 1.5, 1, planeSize * 0.5f + 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"black"));
	textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f + 1.5, 1, planeSize * 0.5f + 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"black"));
	textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f + 1.5, 1, planeSize * 0.5f - 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"black"));
	textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f - 1.5, 1, planeSize * 0.5f - 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"black"));
	textureShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());
	
	renderer->setBackBufferRenderTarget();
	
	return true;
}

bool App1::scenePass()
{
	sceneTexture->setRenderTarget(renderer->getDeviceContext());
	sceneTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX projectionMatrix = renderer->getOrthoMatrix();

	// Portals
	// Portal textures are rendered on an ortho mesh the size of the screen and are blended together with an alpha shader.
	// -------------------- Have to do alpha shader stuff first...
	renderer->setAlphaBlending(true);
	orthoMesh->sendData(renderer->getDeviceContext());
	alphaShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, orangePortal->combinedTexture->getShaderResourceView());
	alphaShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setAlphaBlending(false);

	renderer->setAlphaBlending(true);
	orthoMesh->sendData(renderer->getDeviceContext());
	alphaShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, bluePortal->combinedTexture->getShaderResourceView());
	alphaShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setAlphaBlending(false);
	// ------------------------

	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Portal borders
	orangePortal->border(worldMatrix, viewMatrix, projectionMatrix);
	bluePortal->border(worldMatrix, viewMatrix, projectionMatrix);

	// Pool
	worldMatrix *= XMMatrixTranslation(planeSize / 4 * 3, 0.5f, planeSize / 4);
	tessellationCircleMesh->sendData(renderer->getDeviceContext());
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), pointLights, spotLight, tessellationCircleMesh, time, camera->getPosition());
	tessellationShader->render(renderer->getDeviceContext(), tessellationCircleMesh->getIndexCount());
	
	// Light Indicators
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

	// Spheres							// get rid of hard coded values...
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f - 1.5, 1, planeSize * 0.5f - 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"white"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f - 1.5, 1, planeSize * 0.5f + 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"white"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f + 1.5, 1, planeSize * 0.5f - 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"white"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f + 1.5, 1, planeSize * 0.5f + 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"white"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	// Room
	// Bottom
	worldMatrix = renderer->getWorldMatrix();
	planeMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// top
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(PI, 0.0f, 0.0f);
	worldMatrix *= XMMatrixTranslation(0.0f, planeSize, planeSize);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// left
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(-PI / 2, -PI / 2, 0.0f);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// right
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(-PI / 2, PI / 2, 0.0f);
	worldMatrix *= XMMatrixTranslation(planeSize, 0.0f, planeSize);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// front
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(-PI / 2, 0.0f, 0.0f);
	worldMatrix *= XMMatrixTranslation(0.0f, 0.0f, planeSize);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// back
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixRotationRollPitchYaw(PI / 2, 0.0f, 0.0f);
	worldMatrix *= XMMatrixTranslation(0.0f, planeSize, 0.0f);
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"concrete"), shadowMaps, pointLights, spotLight);
	shadowShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();

	return true;
}

// Everything displayed in the depth pass will cast shadows.
// Takes an integer as a parameter dictating which shadow map to render to and with which point light.
void App1::depthPass(int index)
{
	// Set the render target to be the render to texture.
	shadowMaps[index]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	XMMATRIX worldMatrix;
	XMMATRIX lightViewMatrix;
	XMMATRIX lightProjectionMatrix;
	
	pointLights[index]->light->generateProjectionMatrix(close, distant);
	pointLights[index]->light->generateViewMatrix();

	// Setting matrices from light's perspective.
	worldMatrix = renderer->getWorldMatrix();
	lightViewMatrix = pointLights[index]->light->getViewMatrix();
	lightProjectionMatrix = pointLights[index]->light->getProjectionMatrix();

	// Rendering spheres.					// get rid of hard coded values...
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f - 1.5, 1, planeSize * 0.5f - 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f - 1.5, 1, planeSize * 0.5f + 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f + 1.5, 1, planeSize * 0.5f - 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix *= XMMatrixTranslation(planeSize * 0.5f + 1.5, 1, planeSize * 0.5f + 1.5);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

bool App1::finalPass()
{
	// Clear the scene. 
	renderer->beginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();

	// Need the camera's projection matrix to convert godRayLightSource into screen space.
	XMMATRIX viewMatrix = camera->getViewMatrix();	
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix(); 

	// Rendering the final image to the back buffer, applying the God rays post processing effect.

	if (godRaysOn)
	{
		// Render the scene with God rays.
		orthoMesh->sendData(renderer->getDeviceContext());
		godRayShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, blackTexture->getShaderResourceView(), sceneTexture->getShaderResourceView(), godRayLightSource, XMFLOAT2(1200, 675), viewMatrix, projectionMatrix, decay, exposure, density, weight, illuminationDecay, samples);
		godRayShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	}
	else
	{
		// Render the scene without God rays.
		orthoMesh->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	}

	// Debugging...
	//worldMatrix = renderer->getWorldMatrix();
	//renderer->setZBuffer(false);
	//orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	//orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering
	//smallOrthoMesh->sendData(renderer->getDeviceContext());
	//textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, shadowMaps[2]->getDepthMapSRV());
	//textureShader->render(renderer->getDeviceContext(), smallOrthoMesh->getIndexCount());
	//renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();
	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	// ImGui::SliderFloat("close", &close, 0.1, 10);
	// ImGui::SliderFloat("distant", &distant, 0.2, 1000);
	/*ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("(%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::Text("(%.2f, %.2f, %.2f)", camera->getRotation().x, camera->getRotation().y, camera->getRotation().z);*/
	ImGui::Text("Orange Portal");
	ImGui::SliderFloat("X", &orangePortal->position.x, 2, planeSize - 2);
	ImGui::SliderFloat("Y", &orangePortal->position.y, 3.5, planeSize - 3.5);
	ImGui::Text("Blue Portal");
	ImGui::SliderFloat("X ", &bluePortal->position.x, 2, planeSize - 2);
	ImGui::SliderFloat("Y ", &bluePortal->position.y, 3.5, planeSize - 3.5);
	/*ImGui::SliderFloat("Constant", &pointLights[0]->constant, 0, 1);
	ImGui::SliderFloat("Linear", &pointLights[0]->linear, 0, 1);
	ImGui::SliderFloat("Quadratic", &pointLights[0]->quadratic, 0, 1);*/
	
	//ImGui::Checkbox("Pool Point Light", &pointLights[3]->on);
	ImGui::Checkbox("Flash Light", &spotLight->on);
	ImGui::Checkbox("God Rays", &godRaysOn);
	/*ImGui::SliderFloat("Decay", &decay, 0.5, 1);
	ImGui::SliderFloat("Exposure", &exposure, 0, 2);
	ImGui::SliderFloat("Density", &density, 0, 5);
	ImGui::SliderFloat("Weight", &weight, 0, 2);
	ImGui::SliderFloat("Illumination decay", &illuminationDecay, 0, 2);
	ImGui::SliderInt("Samples", &samples, 0, 200);*/
	
	ImGui::Checkbox("Light 1", &pointLights[2]->on);
	ImGui::SliderFloat("X       ", &pointLights[2]->position.x, 0, (float)planeSize);
	ImGui::SliderFloat("Y       ", &pointLights[2]->position.y, 0, (float)planeSize);
	ImGui::SliderFloat("Z       ", &pointLights[2]->position.z, 0, (float)planeSize);
	//ImGui::SliderFloat("Orange X", &pointLights[0]->position.x, 0, (float)planeSize);
	ImGui::Checkbox("Light 2", &pointLights[0]->on);
	ImGui::SliderFloat("Y    ", &pointLights[0]->position.y, 0, (float)planeSize);
	//ImGui::SliderFloat("Orange Z", &pointLights[0]->position.z, 0, (float)planeSize);
	//ImGui::SliderFloat("Blue X", &pointLights[1]->position.x, 0, (float)planeSize);
	ImGui::Checkbox("Light 3", &pointLights[1]->on);
	ImGui::SliderFloat("Y     ", &pointLights[1]->position.y, 0, (float)planeSize);
	//ImGui::SliderFloat("Blue Z", &pointLights[1]->position.z, 0, (float)planeSize);
	ImGui::Text("Pool light");
	ImGui::SliderFloat("X  ", &pointLights[3]->position.x, 0, (float)planeSize);
	ImGui::SliderFloat("Y  ", &pointLights[3]->position.y, 0, (float)planeSize);
	ImGui::SliderFloat("Z  ", &pointLights[3]->position.z, 0, (float)planeSize);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

