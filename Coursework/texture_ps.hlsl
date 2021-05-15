// Simple pixel shader that returns the texture colour at the texture coordinate.

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
	float4 screenPosition : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour = texture0.Sample(sampler0, input.tex);

	return textureColour;
}



