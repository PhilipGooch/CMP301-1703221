
#pragma once

#include "BaseMesh.h"

using namespace DirectX;

class CircleMesh : public BaseMesh
{

public:
	CircleMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution, float radius);
	~CircleMesh();

	// Public variables for ImGui.
public:
	float amplitude;		// should create Pool class... a mesh does not move...
	float frequency;
	float speed;

	float edge0;
	float edge1;
	float edge2;
	float inside;

private:
	void initBuffers(ID3D11Device* device);

private:
	int resolution;
	float radius;
};


