
cbuffer MatrixBufferType : register (b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix[4];
	matrix lightProjectionMatrix[4];
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
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 worldPosition : POSITION;
	// pass as array?  
	// LightViewPos is the position of the vertex in relation to the light.
	float4 lightViewPos0 : TEXCOORD1;
	float4 lightViewPos1 : TEXCOORD2;
	float4 lightViewPos2 : TEXCOORD3;
	float4 lightViewPos3 : TEXCOORD4;
};


OutputType main(InputType input)
{
	OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.worldPosition = mul(input.position, worldMatrix);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos0 = mul(input.position, worldMatrix);
	output.lightViewPos0 = mul(output.lightViewPos0, lightViewMatrix[0]);
	output.lightViewPos0 = mul(output.lightViewPos0, lightProjectionMatrix[0]);
	output.lightViewPos1 = mul(input.position, worldMatrix);
	output.lightViewPos1 = mul(output.lightViewPos1, lightViewMatrix[1]);
	output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix[1]);
	output.lightViewPos2 = mul(input.position, worldMatrix);
	output.lightViewPos2 = mul(output.lightViewPos2, lightViewMatrix[2]);
	output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix[2]);
	output.lightViewPos3 = mul(input.position, worldMatrix);
	output.lightViewPos3 = mul(output.lightViewPos3, lightViewMatrix[3]);
	output.lightViewPos3 = mul(output.lightViewPos3, lightProjectionMatrix[3]);

	output.tex = input.tex;

	// Multiplying the normal vector by the world matrix to get to world space and normalizing.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}