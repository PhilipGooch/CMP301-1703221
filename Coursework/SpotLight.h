#pragma once

#include "DXF.h"

class SpotLight
{

public:
	SpotLight(FPCamera* camera, XMFLOAT4 diffuse, XMFLOAT4 ambient);

	void rotate();

	inline XMFLOAT3 getDirection() { return direction; }
	
	// Public variables for ImGui manipulation.
public:
	Light* light;

	FPCamera* camera;

	float constant;
	float linear;
	float quadratic;

	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;

	bool on;

private:
	XMFLOAT3 direction;
	XMMATRIX rotationMatrix;
	XMVECTOR lookAt;
};

