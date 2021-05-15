#pragma once

#include "BaseMesh.h"

class MyPlaneMesh : public BaseMesh
{

public:
	MyPlaneMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 100);
	~MyPlaneMesh();

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};
