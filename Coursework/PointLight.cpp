#include "PointLight.h"

PointLight::PointLight(XMFLOAT3 position, XMFLOAT4 diffuse, XMFLOAT4 ambient, float constant, float linear, float quadratic, bool on, float planeSize, int ID) :
	position(position),
	diffuse(diffuse),
	ambient(ambient),
	constant(constant),
	linear(linear),
	quadratic(quadratic),
	on(on),
	planeSize(planeSize),
	ID(ID)
{
	light = new Light;
	light->setDiffuseColour(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	light->setAmbientColour(ambient.x, ambient.y, ambient.z, ambient.w);
	light->setPosition(position.x, position.y, position.z);
	int sceneWidth = 100;
	int sceneHeight = 100;
	light->setDirection(0.0f, -4.0f, 0.7f);

	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);
	light->generateProjectionMatrix(2.1f, 100.f);
}

PointLight::~PointLight()
{
}

void PointLight::updatePosition(float time)
{
	if (ID == 0 || ID == 1)
	{
		position.x = planeSize * 0.5f + cos(time + 3.1415926 * 2 / 2 * ID) * 8;// cos(time + 3.1415926 * 2 / 3 * ID) * 8;
		//position.y = 6.0f + cos(time) * 2;// sin(time + 3.1415926 * 2 / 3 * ID);
		position.z = planeSize * 0.5f + sin(time + 3.1415926 * 2 / 2 * ID) * 8;// sin(time + 3.1415926 * 2 / 3 * ID) * 8;
	}
	light->setPosition(position.x, position.y, position.z);
}

void PointLight::updateDirection()
{
	//XMFLOAT3 direction = XMFLOAT3(1, 0, 0);
	XMFLOAT3 direction = normalize(XMFLOAT3(planeSize * 0.5f - position.x, 1.0f - position.y, planeSize * 0.5f - position.z)); // <--- change hard coded values.
	light->setDirection(direction.x, direction.y, direction.z);
}

XMFLOAT3 PointLight::normalize(XMFLOAT3 vector)
{
	float mag = magnitude(vector);
	vector.x /= mag;
	vector.y /= mag;
	vector.z /= mag;
	return vector;
}

// This should not be being done on the CPU...
float PointLight::magnitude(XMFLOAT3 vector)
{
	return sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
}
