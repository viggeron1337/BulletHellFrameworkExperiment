cbuffer GS_CONSTANT_BUFFER : register(b0)
{
	matrix wvpMatrix;
	matrix worldMatrix;
	vector lightPos;
};

struct INPUT
{
	float4 position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 color : COLOR; 
};

struct OUTPUT
{
	float4 position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 color : COLOR; 
	float3 normal : NORMAL;
	float3 worldVert : WORLDVERTEX;
};


[maxvertexcount(6)]
void GS_main(triangle INPUT input[3], inout TriangleStream<OUTPUT> fragmentShaderInput)
{
	OUTPUT output = (OUTPUT)0;

	float3 p0p1 = input[1].position.xyz - input[0].position.xyz;
	float3 p0p2 = input[2].position.xyz - input[0].position.xyz;

	float3 n = normalize(cross(p0p1.xyz, p0p2.xyz));

	for (uint i = 0; i < 3; i++)
	{
		output.position = mul(input[i].position, wvpMatrix);
		output.color = input[i].color; 
		output.normal = mul(float4(n, 0), worldMatrix).xyz;
		output.worldVert = mul(input[i].position, worldMatrix).xyz;
		output.Tex = input[i].Tex;

		fragmentShaderInput.Append(output);
	}

	fragmentShaderInput.RestartStrip();

	//UNDERSTAND THE REASONING BEHIND THE MATRIX MULTIPLICATIONS AND THEN PERHAPS TRY TO MAKE THINGS WORK 
	//WITHOUT GEOMETRY SHADER. 

	/*for (uint i = 0; i < 3; i++)
	{
		output.position = mul(input[i].position + float4(n, 0), wvpMatrix);
		output.color = input[i].color;
		output.normal = mul(float4(n, 0), worldMatrix).xyz;
		output.worldVert = mul(input[i].position + float4(n, 0), worldMatrix).xyz;
		output.Tex = input[i].Tex;

		fragmentShaderInput.Append(output);
	}
	fragmentShaderInput.RestartStrip();*/
}