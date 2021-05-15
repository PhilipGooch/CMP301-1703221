// Basic vertex shader that just converts the local position to the screen position using given matrices.
// Also passes along the texture coordinate.

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct InputType
{
	float4 localPosition : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 screenPosition : SV_POSITION;
	float2 tex : TEXCOORD0;
};

OutputType main(InputType input)
{
	OutputType output;

	output.screenPosition = mul(input.localPosition, worldMatrix);
	output.screenPosition = mul(output.screenPosition, viewMatrix);
	output.screenPosition = mul(output.screenPosition, projectionMatrix);

	output.tex = input.tex;

	return output;
}