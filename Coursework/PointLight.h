#pragma once

#include "DXF.h"

class PointLight 
{
public:
	PointLight(XMFLOAT3 position, XMFLOAT4 diffuse, XMFLOAT4 ambient, float constant, float linear, float quadratic, bool on, float planeSize, int ID);
	~PointLight();

	void updatePosition(float time);

	void updateDirection();

	XMFLOAT3 normalize(XMFLOAT3 vector);

	float magnitude(XMFLOAT3 vector);


	// Public variables for ImGui manipulation.
public:
	Light* light;

	XMFLOAT3 position;

	float constant;
	float linear;
	float quadratic;

	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;

	bool on;

	float planeSize;

	int ID;

};

