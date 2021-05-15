#include "SpotLight.h"

SpotLight::SpotLight(FPCamera* camera, XMFLOAT4 diffuse, XMFLOAT4 ambient) :
	camera(camera),
	direction(XMFLOAT3(0.0f, 0.0f, 1.0f)),
	diffuse(diffuse),
	ambient(ambient),
	constant(0.36f),
	linear(0.026f),
	quadratic(0.0f),
	on(false)
{
	light = new Light;
	light->setDiffuseColour(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
	light->setAmbientColour(ambient.x, ambient.y, ambient.z, ambient.w);
	light->setPosition(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
}

void SpotLight::rotate()
{
	// Rotating the spotlight's direction vector using the camera's roll, pitch and yaw variables.
	rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(camera->getRotation().x), XMConvertToRadians(camera->getRotation().y), XMConvertToRadians(camera->getRotation().z));
	lookAt = XMVectorSet(0.0, 0.0, 1.0f, 1.0f);
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	direction.x = XMVectorGetX(lookAt);
	direction.y = XMVectorGetY(lookAt);
	direction.z = XMVectorGetZ(lookAt);
	light->setDirection(direction.x, direction.y, direction.z);
}
