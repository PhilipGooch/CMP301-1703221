// Samples the texture colour at the given texture coordinate and discards the pixel if the colour's alpha value is less than 1.

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
	float4 screenPosition : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;

	textureColour = texture0.Sample(sampler0, input.tex);

	// Discard the pixel if the alpha value is less than 1.
	clip(textureColour.a - 1.0f);			

	return textureColour;
}



