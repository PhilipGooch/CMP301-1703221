
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

// Passing point light data for 4 lights.
cbuffer PointLightsBuffer : register(b0)
{
	float4 pointLightsAmbient[4];
	float4 pointLightsDiffuse[4];
	float4 pointLightsPosition[4];
	float4 pointLightsConstant;
	float4 pointLightsLinear;
	float4 pointLightsQuadratic;
	float4 pointLightsOn;
};

// Passing data for spotlight.
cbuffer SpotLightBuffer : register(b1)
{
	float4 spotLightAmbient;
	float4 spotLightDiffuse;
	float4 spotLightPosition;
	float4 spotLightDirection;
	float spotLightOn;
	float3 padding3;
};

struct InputType
{
	float4 screenPosition : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	// Passing the world position of the geometry for lighting calculations.
	float4 worldPosition : POSITION;
};

// Map function from https://forum.processing.org/two/discussion/22471/how-does-mapping-function-work
float map(float value, float istart, float istop, float ostart, float ostop) {
	return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

float4 calculateLighting(float3 inverseLightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, inverseLightDirection));

	// Mapping the intensity value from a value between 0 and 1 to a value between 0.5 and 1.
	// This gives less drop off to light intensity as the angle between the light and the normal increases.
	intensity = map(intensity, 0, 1, 0.5, 1);

	// Colour is clamped between 0 and 1.
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 main(InputType input) : SV_TARGET
{
	//return float4(input.normal.y, input.normal.y, input.normal.y, 1.0f);

	// Texture colour
	float4 textureColour = texture0.Sample(sampler0, input.tex);

	// Total light colour.
	float4 lightColour = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Colour for each point light.
	float4 pointLightColour;

	// Light vector for each light.
	float3 pointLightsVector[4];

	// Light distance from geometry for each light.
	float pointLightsDistance[4];

	// The pool is only currently only affected by the fourth point light as too much lighting messes with the post processing effect.
	for (int i = 3; i < 4; i++)
	{
		// Point light colour is reset to black for each light.
		pointLightColour = float4(0.0f, 0.0f, 0.0f, 1.0f);
		if (pointLightsOn[i])
		{
			// Light vector is the normalised vector from the geometry to the light.
			pointLightsVector[i] = normalize(pointLightsPosition[i] - input.worldPosition);

			// Light distance is the distance from the geometry to the light.
			pointLightsDistance[i] = length(pointLightsPosition[i] - input.worldPosition);

			// Attenuation is calculated using a quadratic equation using the variables for constant, linear and quadratic passed into the shader.
			// Attenuation is a value between 0 and 1.
			float attenuation = 1 / (pointLightsConstant[i] + (pointLightsLinear[i] * pointLightsDistance[i]) + (pointLightsQuadratic[i] * pow(pointLightsDistance[i], 2)));

			// Lighting is calculated and multiplied by attenuation with the ambient of the light added to it.
			pointLightColour += calculateLighting(pointLightsVector[i], input.normal, pointLightsDiffuse[i]) * attenuation + pointLightsAmbient[i];

			// Adding the point light colour to the overall light colour.
			lightColour += pointLightColour;
		}
	}

	// The pool is not affected by the spotlight currently so it is commented out.

	//// Spot light
	//if (spotLightOn)
	//{
	//	float4 spotLightColour = { 0.0f, 0.0f, 0.0f, 1.0f };
	//	float3 spotLightVector;
	//	float3 inverseSpotLightVector;
	//	float spotLightDistance;
	//	float cosA;
	//	float cosB;
	//	float halfConeAngle = 3.1415926 / 12;

	//	spotLightVector = normalize(spotLightPosition - input.worldPosition);
	//	inverseSpotLightVector = -spotLightVector;
	//	spotLightDistance = length(spotLightPosition - input.worldPosition);
	//	cosA = dot(spotLightDirection, inverseSpotLightVector);
	//	cosB = cos(halfConeAngle);
	//	//if (cosA > cosB)
	//	{
	//		//float attenuation = 1 / (spotLightConstantFactor + (spotLightLinearFactor * spotLightDistance) + (spotLightQuadraticFactor * pow(spotLightDistance, 2)));
	//		spotLightColour = calculateLighting(spotLightVector, input.normal, spotLightDiffuse);
	//		//spotLightColour *= attenuation;
	//		spotLightColour *= pow(max(cosA, 0), 16);
	//	}
	//	lightColour += spotLightColour;
	//}

	return textureColour * saturate(lightColour);

}



