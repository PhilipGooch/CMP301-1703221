// Simple pixel shader that returns the colour passed in.

cbuffer colourBuffer : register (b0)
{
	float4 colour;
};

struct InputType
{
	float4 screenPosition : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	return colour;
}



