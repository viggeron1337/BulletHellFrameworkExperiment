Texture2D txDiffuse : register(t0);
SamplerState sampAni : register(s0);


cbuffer FS_CONSTANT_BUFFER
{
	matrix wvpMatrix;
	matrix worldMatrix;
	float3 lightPos;
};

struct FS_IN
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Color : COLOR;
	float3 Normal : NORMAL;
	float3 worldPos : WORLDPOS;
};

float4 PS_main(FS_IN input) : SV_Target
{
	float4 ambient = float4(0.2f,0.2f,0.2f,1.0f);

	float3 lightDir = lightPos - input.worldPos.xyz;

	float3 samp = txDiffuse.Sample(sampAni, input.Tex).xyz;
	float diffuseFactor = dot(normalize(input.Normal.xyz), normalize(lightDir));

	diffuseFactor = max(diffuseFactor, 0);

	float3 finalColor = ambient + (diffuseFactor * samp);
	return float4(finalColor,0);
}