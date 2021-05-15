#pragma once
#include "DXF.h"
#include "StencilShader.h"
#include "AlphaShader.h"
#include "GodRayShader.h"
#include "TessellationTextureShader.h"
#include "TessellationShader.h"
#include "TextureShader.h"
#include "DepthShader.h"
#include "ShadowShader.h"
#include "MyPlaneMesh.h"
#include "CircleMesh.h"
#include "ColourShader.h"
#include "TessellationCircleMesh.h"
#include "TextureShader.h"
#include "PointLight.h"

#define PI 3.1415926f

// Passing every single shader... re-design...

class Portal
{
public:
	Portal(HWND hwnd, D3D* renderer, Input* input, int screenWidth, int screenHeight, XMFLOAT3 position, 
		   StencilShader* stencilShader, ColourShader* colourShader, AlphaShader* alphaShader, TessellationTextureShader* tessellationTextureShader, GodRayShader* godRayShader, DepthShader* depthShader, ShadowShader* shadowShader, TessellationShader* tessellationShader, TextureShader* textureShader,
		   TextureManager* textureManager, int ID, float time, XMFLOAT3 godRayLightSource, float decay, float exposure, float density, float weight, float illuminationDecay, int samples, bool godRaysOn);
	~Portal();

	inline void setOtherPortal(Portal* otherPortal_) { otherPortal = otherPortal_; }

	void updateCamera(XMFLOAT3 playerPosition, XMFLOAT3 playerLookAt);

	void stencilPass(FPCamera* playerCamera);

	void scenePass(ShadowMap** shadowMaps, PointLight** pointLights, SpotLight* spotLight, float planeSize, SphereMesh* sphereMesh, MyPlaneMesh* planeMesh);

	void combinedPass();

	void border(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

	void blackPass(FPCamera* playerCamera, float planeSize, SphereMesh* sphereMesh, TextureManager* textureMgr, TextureShader* textureShader);

	void godRayPass(FPCamera* playerCamera);


//private:	make private...!
public:
	XMFLOAT3 position;
	//char axis;
	Portal* otherPortal;
	//char ID;

	FPCamera* camera;

	RenderTexture* stencilTexture;
	RenderTexture* sceneTexture;
	RenderTexture* combinedTexture;
	RenderTexture* blackTexture;
	RenderTexture* godRayTexture;

	CircleMesh* stencilMesh;
	CircleMesh* borderMesh;
	OrthoMesh* orthoMesh;
	TessellationCircleMesh* tessellationCircleMesh;

	D3D* renderer;
	StencilShader* stencilShader;
	AlphaShader* alphaShader;
	TextureManager* textureManager;
	ColourShader* colourShader;
	GodRayShader* godRayShader;
	TessellationTextureShader* tessellationTextureShader;
	DepthShader* depthShader;
	ShadowShader* shadowShader;
	TessellationShader* tessellationShader;
	TextureShader* textureShader;

	int ID;

	float time;

	XMFLOAT3 godRayLightSource;

	float decay;
	float exposure;
	float density;
	float weight;
	float illuminationDecay;
	int samples;

	bool godRaysOn;
};

