// Tessellation Hull Shader.
// Prepares control points for tessellation.

cbuffer manipulationBuffer : register (b0)
{
	// Variables describe how much tessellation to do on each edge and inside.
	float edge0;
	float edge1;
	float edge2;
	float inside;
	float3 cameraPosition;
	float padding0;
}

struct InputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{    
	// Dynamic tessellation	gives a strange effect on the pool. I am leaving it with max tessellation.	

	//ConstantOutputType output;

	//float maxTessellation = 16;

	//float3 midPointInside = (inputPatch[0].position + inputPatch[1].position + inputPatch[2].position) / 3;
	//float3 midPointEdge0 = lerp(inputPatch[0].position, inputPatch[1].position, 0.5f);
	//float3 midPointEdge1 = lerp(inputPatch[1].position, inputPatch[2].position, 0.5f);
	//float3 midPointEdge2 = lerp(inputPatch[2].position, inputPatch[0].position, 0.5f);

	//float distanceInside = sqrt(pow(cameraPosition.x - midPointInside.x, 2) + pow(cameraPosition.y - midPointInside.y, 2) + pow(cameraPosition.z - midPointInside.z, 2));
	//float distanceEdge0 = sqrt(pow(cameraPosition.x - midPointEdge0.x, 2) + pow(cameraPosition.y - midPointEdge0.y, 2) + pow(cameraPosition.z - midPointEdge0.z, 2));
	//float distanceEdge1 = sqrt(pow(cameraPosition.x - midPointEdge1.x, 2) + pow(cameraPosition.y - midPointEdge1.y, 2) + pow(cameraPosition.z - midPointEdge1.z, 2));
	//float distanceEdge2 = sqrt(pow(cameraPosition.x - midPointEdge2.x, 2) + pow(cameraPosition.y - midPointEdge2.y, 2) + pow(cameraPosition.z - midPointEdge2.z, 2));

	//float tessellationFactorInside = max((maxTessellation - distanceInside), 1);
	//float tessellationFactorEdge0 = max(maxTessellation - distanceEdge0, 1);
	//float tessellationFactorEdge1 = max(maxTessellation - distanceEdge1, 1);
	//float tessellationFactorEdge2 = max(maxTessellation - distanceEdge2, 1);

	//// Set the tessellation factor for tessallating inside the triangle.
	//output.inside = tessellationFactorInside;

	//// Set the tessellation factors for the three edges of the triangle.
	//output.edges[0] = tessellationFactorEdge0;
	//output.edges[1] = tessellationFactorEdge1;
	//output.edges[2] = tessellationFactorEdge2;

    ConstantOutputType output;
	
    // Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = edge0;
	output.edges[1] = edge1;
	output.edges[2] = edge2;

    // Set the tessellation factor for tessallating inside the triangle.
	output.inside = inside;
	

    return output;
}


[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

	output.tex = patch[pointId].tex;

	output.normal = patch[pointId].normal;

    return output;
}