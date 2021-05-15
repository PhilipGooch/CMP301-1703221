// The technique used in this shader I learnt from https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch13.html and https://medium.com/community-play-3d/god-rays-whats-that-5a67f26aeac2.

SamplerState sampler0 : register(s0);

// This is the texture that will be sampled to create god rays.
Texture2D blackTexture : register(t0);

// This is the texture of the scene that the god rays will be added to.
Texture2D sceneTexture : register(t1);

cbuffer GodRayBuffer : register(b0)
{
	// World position of the light source. (The centre of the pool.)
	float4 lightPosition;
	float2 screenSize;
	float2 padding0;
	// View and projection matrices used to convert the light source position into screen space.
	matrix viewMatrix;
	matrix projectionMatrix;
	// Variables for manipulating how the God rays are generated.
	float cDecay;
	float cExposure;
	float cDensity;
	float cWeight;
	float cIlluminationDecay;
	int cSamples;
	float2 padding1;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
	// Converting light position into view space.
	float4 lightPositionOnScreen = lightPosition;
	lightPositionOnScreen = mul(lightPositionOnScreen, viewMatrix);
	lightPositionOnScreen = mul(lightPositionOnScreen, projectionMatrix);

	// Converting light position to clip space.
	lightPositionOnScreen.xyz /= lightPositionOnScreen.w;
	lightPositionOnScreen.xy *= float2(0.5f, -0.5f);
	lightPositionOnScreen.xy += 0.5f;

	// Setting dynamic variables. Can't alter variables passed in to a constant buffer/
	
	// decay is a constant that affects the illumination decay.
	float decay = cDecay;
	// exposure is a constant that is used to adjust how much the overall calculated god ray is multiplied by when applying to scene texture.
	float exposure = cExposure;
	// density is a constant that affects the distance the next sample will be taken from. 
	float density = cDensity;
	// weight is a constant that affects the brightness of the sample taken.
	float weight = cWeight;
	// illuminationDecas is a variable that affects the brightness of the sample taken. This is increased at every itteration.
	float illuminationDecay = cIlluminationDecay;
	// samples is the number of samples that will be added to the colour value.
	int samples = 10;

	// deltaTexCoor is a vector that describes a segment of the line in which the ray is travelling. This is used to find the position of the next sample.
	float2 deltaTexCoord = (input.tex - lightPositionOnScreen.xy);
	deltaTexCoord *= 1.0 / float(samples) * density;

	// textureColour is the original colour of the black texture at the texture coordinate.
	float4 textureColour = blackTexture.Sample(sampler0, input.tex) * 0.4f;

	// tc is the new texture coordinate of the next spot in the ray. 
	float2 tc = input.tex;
	for (int i = 0; i < samples; i++) {
		// Finding the next texture coordinate.
		tc -= deltaTexCoord;
		// Sampling the colour at this texture coordinate.
		float4 sampleColour = blackTexture.Sample(sampler0, tc) * 0.4f;
		sampleColour *= illuminationDecay * weight;
		// Adding to the original colour.
		textureColour += sampleColour;
		illuminationDecay *= decay;
	}

	// Sampling the colour in the scene texture.
	float4 realColor = sceneTexture.Sample(sampler0, input.tex) * 1.1f;

	// Returning the manipulated colours added together.

	return ((float4((float3(textureColour.r, textureColour.g, textureColour.b) * exposure), 1.0f)) + (realColor * (1.1f))); 
}