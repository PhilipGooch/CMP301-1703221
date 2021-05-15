// Tessellation domain shader.
// After tessellation the domain shader processes all the vertices.

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer ManipulationBuffer : register(b1)
{
	// Variables for manipulating the sin wave of the pool.
	float time;
	float amplitude;
	float speed;
	float frequency;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct InputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 worldPosition : POSITION;
};

float yPosition(float x, float z)
{
	// Passing the magnitude of the vector between the vertex and the centre of the pool to the sin function.
	// Multiplying this by a variable inversely proportional to the distance it is from the centre. This creates the effect of the ripples loosing momentum as it spreads.
	return sin(-sqrt(pow(x, 2) + pow(z, 2)) * frequency + time * speed) * clamp((amplitude * (10 - sqrt(pow(x, 2) + pow(z, 2)) * 1)), 0, 10);
}

[domain("tri")]
OutputType main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 3> patch)
{
    float3 vertexPosition;
    OutputType output;
 
    // Determine the position of the new vertex.
	// Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
	// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).
	vertexPosition = uvwCoord.x * patch[0].position + -uvwCoord.y * patch[1].position + -uvwCoord.z * patch[2].position;

	// Calculating the Y position of the vertex.
	vertexPosition.y = yPosition(vertexPosition.x, vertexPosition.z);
	
	// Passing the world position.
	output.worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix);

    // Calculate the position of the new vertex against the world, view, and projection matrices.
	// Passing the position in view space.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	
	// Setting the texture coordinates.
	output.tex = -uvwCoord.x * patch[0].tex + -uvwCoord.y * patch[1].tex + -uvwCoord.z * patch[2].tex;		
	
	// Calculating normals.

	// The distance it will sample the north and west neighbours from.
	float neighbourDistance = 0.1;

	// Calculating west X and Z coordinates.
	float3 westNeighbour = vertexPosition;
	westNeighbour.x -= neighbourDistance;

	// Calculating north X and Z coordinates.
	float3 northNeighbour = vertexPosition;
	northNeighbour.z += neighbourDistance;

	// Calculating Y coordinates for both of these points.
	westNeighbour.y = yPosition(westNeighbour.x, westNeighbour.z);
	northNeighbour.y = yPosition(northNeighbour.x, northNeighbour.z);

	// Calculating vectors to use for cross product calculation.
	float3 v1 = westNeighbour - vertexPosition;
	float3 v2 = northNeighbour - vertexPosition;

	// The normal is the cross product of these two vectors.
	output.normal = cross(v1, v2);
	
	// Normalising the normal.
	output.normal = normalize(output.normal);

    return output;
}

