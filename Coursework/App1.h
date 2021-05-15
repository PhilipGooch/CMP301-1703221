// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework

#include "StencilShader.h"
#include "AlphaShader.h"
#include "ColourShader.h"
#include "TessellationShader.h"
#include "TessellationTextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "TextureShader.h"
#include "GodRayShader.h"
#include "Portal.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "TessellationCircleMesh.h"

#define PI 3.1415962f

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool blackPass();
	bool scenePass();
	void depthPass(int index);
	bool finalPass();
	bool render();
	void gui();

private:
	StencilShader* stencilShader;
	AlphaShader* alphaShader;
	ColourShader* colourShader;
	TessellationShader* tessellationShader;
	GodRayShader* godRayShader;
	TessellationTextureShader* tessellationTextureShader;
	TextureShader* textureShader;
	DepthShader* depthShader;
	ShadowShader* shadowShader;

	RenderTexture* blackTexture;
	RenderTexture* sceneTexture;

	PointLight* pointLights[4];
	SpotLight* spotLight;
	Portal* orangePortal;
	Portal* bluePortal;

	XMFLOAT3 godRayLightSource;

	OrthoMesh* orthoMesh;
	OrthoMesh* smallOrthoMesh;
	TessellationCircleMesh* tessellationCircleMesh;
	SphereMesh* sphereMesh;
	MyPlaneMesh* planeMesh;

	float time;

	int planeSize;

	float decay = 0.974;
	float exposure = 0.239;
	float density = 0.748;
	float weight = 0.479;
	float illuminationDecay = 0.419;
	int samples = 100;

	ShadowMap* shadowMaps[4];
	OrthoMesh* depthOrthoMesh;

	bool godRaysOn = true;

	float close = 2.1f;
	float distant = 100.0f;
};

#endif