cbuffer PerObject
{
	float4x4 WorldViewProj;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};


VertexOut VS(VertexIn vertexIn)
{
	VertexOut vertexOut;
	
	// Transform to homogeneous clip space.
	vertexOut.PosH = mul(float4(vertexIn.PosL, 1.0f), WorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vertexOut.Color = vertexIn.Color;
    
    return vertexOut;
}

float4 PS(VertexOut vertexOut) : SV_Target
{
	return float4(1, 1, 1, 1);
	//vertexOut.Color;
}

technique10 ColorTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}