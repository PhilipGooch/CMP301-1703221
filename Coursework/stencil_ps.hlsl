
// The texture of the scene from the portal's perspective.
Texture2D sceneTexture : register(t0);
// The texture of the stencil. This is white everywhere except for where the portal will be, which is green.
Texture2D stencilTexture : register(t1);
// sampler0 is anisotropic and is used for sampling the scene texture.
SamplerState sampler0 : register(s0);
// sampler1 is exact and is used for sampling the stencil texture.
SamplerState sampler1 : register(s1);

struct InputType
{
	float4 screenPosition : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	// Sampling scene texture.
	float4 textureColour = sceneTexture.Sample(sampler0, input.tex);

	// Sampling stencil texture.
	float4 stencilColour = stencilTexture.Sample(sampler1, input.tex);

	// If the stencil colour is green, return the colour from the scene texture.
	if (stencilColour.r == 0.0f && stencilColour.g == 1.0f && stencilColour.b == 0.0f)
	{
		return textureColour;
	}
	// If the colour is not green, return a colour value with an alpha value of 0.
	else
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}



