
Texture2D shaderTexture : register(t0);
Texture2D depthMap0 : register(t1);
Texture2D depthMap1 : register(t2);
Texture2D depthMap2 : register(t3);
Texture2D depthMap3 : register(t4);

SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer PointLightsBuffer : register(b0)
{
	float4 pointLightsAmbient[4];
	float4 pointLightsDiffuse[4];
	float4 pointLightsPosition[4];
	float4 pointLightsDirection[4];
	float4 pointLightsConstant;
	float4 pointLightsLinear;
	float4 pointLightsQuadratic;
	float4 pointLightsOn;
};

cbuffer SpotLightBuffer : register(b1)
{
	float4 spotLightAmbient;
	float4 spotLightDiffuse;
	float4 spotLightPosition;
	float4 spotLightDirection;
	float spotLightConstant;
	float spotLightLinear;
	float spotLightQuadratic;
	float spotLightOn;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 worldPosition : POSITION;

	// Passing the light positions 
	float4 lightViewPos0 : TEXCOORD1;
	float4 lightViewPos1 : TEXCOORD2;
	float4 lightViewPos2 : TEXCOORD3;
	float4 lightViewPos3 : TEXCOORD4;
};

// Map function from https://forum.processing.org/two/discussion/22471/how-does-mapping-function-work
float map(float value, float istart, float istop, float ostart, float ostop) {
	return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

float4 calculatePointLighting(float3 inverseLightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, inverseLightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 calculateSpotLighting(float3 inverseLightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, inverseLightDirection));

	// Mapping the intensity value from a value between 0 and 1 to a value between 0.5 and 1.
	// This gives less drop off to light intensity as the angle between the light and the normal increases.
	intensity = map(intensity, 0, 1, 0.5, 1);

	float4 colour = saturate(diffuse * intensity);
	return colour;
}

bool inShadow(float4 lightViewPosition, Texture2D depthMap)
{
	float2 textureCoordinate = lightViewPosition.xy / lightViewPosition.w;
	textureCoordinate *= float2(0.5, -0.5);
	textureCoordinate += float2(0.5f, 0.5f);

	// Determine if the projected coordinates are in the 0 to 1 range.  
	if (textureCoordinate.x < 0.f || textureCoordinate.x > 1.f || textureCoordinate.y < 0.f || textureCoordinate.y > 1.f)
	{
		return false;
	}

	// If the position of the vertex in relation to the light is behind the light, it is not in shadow.
	if (lightViewPosition.z <= 0)
		return false;

	// Sample the shadow map (get depth of geometry)
	float depthValue = depthMap.Sample(shadowSampler, textureCoordinate).r;

	// Calculate the depth from the light.
	float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
	float shadowMapBias = 0.0045f;
	lightDepthValue -= shadowMapBias;

	// If the depth from the light is greater than the depth value from the depth map, it is in shadow, else it is not.
	if (lightDepthValue > depthValue )
	{
		return true;
	}
	else
	{
		return false;
	}
}

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

	// pass this directly as an array...
	float4 lightViewPositions[4] = { input.lightViewPos0, input.lightViewPos1, input.lightViewPos2, input.lightViewPos3 };
	Texture2D depthMaps[4] = { depthMap0, depthMap1, depthMap2, depthMap3 };

	float3 pointLightsVectors[4];
	float pointLightsDistances[4];
	float attenuation;
	float4 pointLightsColour = float4(0.f, 0.f, 0.f, 1.f);

	// For each point light.
	for (int i = 0; i < 3; i++)
	{
		// If the point light is on.
		if (pointLightsOn[i])
		{
			// If the pixel is not in shadow.
			if (!inShadow(lightViewPositions[i], depthMaps[i]))
			{
				// Light vector is the normalised vector from the geometry to the light.
				pointLightsVectors[i] = normalize(pointLightsPosition[i] - input.worldPosition);

				// Light distance is the distance from the geometry to the light.
				pointLightsDistances[i] = length(pointLightsPosition[i] - input.worldPosition);

				// Attenuation is calculated using a quadratic equation using the variables for constant, linear and quadratic passed into the shader.
				// Attenuation is a value between 0 and 1.
				attenuation = 1 / (pointLightsConstant[i] + (pointLightsLinear[i] * pointLightsDistances[i]) + (pointLightsQuadratic[i] * pow(pointLightsDistances[i], 2)));

				// Lighting is calculated and multiplied by attenuation with the ambient of the light added to it.
				pointLightsColour += calculatePointLighting(pointLightsVectors[i], input.normal, pointLightsDiffuse[i]) * attenuation + pointLightsAmbient[i];
			}
		}
	}


	float4 spotLightColour = float4(0.f, 0.f, 0.f, 1.f);

	// Spot light
	if (spotLightOn)
	{
		float3 spotLightVector;
		float3 inverseSpotLightVector;
		float spotLightDistance;
		float halfConeAngle = 3.1415926 / 12;

		// Light vector is the normalised vector from the geometry to the light.
		spotLightVector = normalize(spotLightPosition - input.worldPosition);

		// Getting the inverse light vector for calculating the angle between the light and the normal.
		inverseSpotLightVector = -spotLightVector;

		// Light distance is the distance from the geometry to the light.
		spotLightDistance = length(spotLightPosition - input.worldPosition);

		// The angle between the light and the normal.
		float cosA = dot(spotLightDirection, inverseSpotLightVector);

		// Attenuation is calculated using a quadratic equation using the variables for constant, linear and quadratic passed into the shader.
		// Attenuation is a value between 0 and 1.
		float attenuation = 1 / (spotLightConstant + (spotLightLinear * spotLightDistance) + (spotLightQuadratic * pow(spotLightDistance, 2)));

		// Calculating the spotlight lighting colour.
		spotLightColour = calculateSpotLighting(spotLightVector, input.normal, spotLightDiffuse);
		spotLightColour *= attenuation;
		spotLightColour *= pow(max(cosA, 0), 16);
		
	}

	return textureColour * saturate(pointLightsColour + spotLightColour);
}