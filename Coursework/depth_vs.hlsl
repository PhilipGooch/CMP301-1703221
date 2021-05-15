// The shader converts the local position to the screen position using given matrices.
// It stores this twice for depth value calculations in the pixel shader.

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

OutputType main(InputType input)
{
	OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, lightViewMatrix);
	output.position = mul(output.position, lightProjectionMatrix);

	// Store the position value in a second input value for depth value calculations.
	output.depthPosition = output.position;

	return output;
}