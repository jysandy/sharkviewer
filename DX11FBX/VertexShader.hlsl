cbuffer perObject : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
}

struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

struct PixelShaderInput
{
	float4 posH : SV_POSITION;
	float4 posW : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	output.color = input.color;
	float4x4 wvp;
	wvp = mul(mul(world, view), projection);
	output.posH = mul(float4(input.pos, 1.0f), wvp);
	output.posW = mul(float4(input.pos, 1.0f), world);
	//This works as long as there is only uniform scaling.
	output.normal = mul(float4(input.normal, 0.0f), world);
	return output;
}