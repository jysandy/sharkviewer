struct PixelShaderInput
{
	float4 posH : SV_POSITION;
	float4 posW : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};

struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

struct DirectionalLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 direction;
	float pad;
};

Texture2D Texture : register(t0);
sampler s : register(s0);

struct PointLight
{
	float3 position;
	float pad;
	float3 attenuation;
	float range;
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

struct Spotlight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 position;
	float range;
	float3 attenuation;
	float spotlightPower;
	float3 direction;
	float pad;
};

cbuffer perObject : register(b1)
{
	Material material;
	DirectionalLight dLight;
	float3 cameraPosition;
	float pad;
}

struct ComputedColor
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
};

ComputedColor computeDirectionalColor(float3 normal, float3 toEye)
{
	ComputedColor output;
	float3 lightVector = -1 * dLight.direction;

		output.ambient = dLight.ambient * material.ambient;

	float kd = max(dot(lightVector, normal), 0);
	output.diffuse = kd * dLight.diffuse * material.diffuse;
	
	float ks;
	if (kd > 0)
	{
		float3 r = reflect(-lightVector, normal);
			ks = max(dot(r, toEye), 0);
		ks = pow(ks, material.specular.w);
		output.specular = ks * dLight.specular * material.specular;
	}
	else
	{
		output.specular = float4(0, 0, 0, 0);
	}

	output.ambient = saturate(output.ambient);
	output.diffuse = saturate(output.diffuse);
	output.specular = saturate(output.specular);

	return output;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 normal;
	normal = normalize(input.normal);
	
	float3 vertex;
	vertex.x = input.posW.x;
	vertex.y = input.posW.y;
	vertex.z = input.posW.z;

	float3 toEye;
	toEye = normalize(cameraPosition - vertex);

	ComputedColor directionalColor;
	directionalColor = computeDirectionalColor(normal, toEye);
	
	float4 litColor;
	float4 texColor = Texture.Sample(s, input.texcoord);
	litColor = texColor * (directionalColor.ambient + directionalColor.diffuse) + directionalColor.specular;
	//litColor = directionalColor.ambient + directionalColor.diffuse + directionalColor.specular;
	litColor.a = material.diffuse.a;
	
	return litColor;
}