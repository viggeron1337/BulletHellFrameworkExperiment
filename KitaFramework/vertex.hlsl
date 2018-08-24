cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	matrix wvpMatrix;
	matrix worldMatrix;
	vector lightPos;
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Color : COLOR;
	float3 Normal : NORMAL; 
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Color : COLOR;
	float3 Normal : NORMAL; 
	float3 WorldVertex : WORLDVERTEX; 
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1), wvpMatrix); 
	output.Color = input.Color;
	output.Tex = input.Tex;
	output.Normal = mul(float4(input.Normal, 0), worldMatrix).xyz; 
	output.WorldVertex = mul(float4(input.Pos, 0), worldMatrix).xyz; 

	return output;
}