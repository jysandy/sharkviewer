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
/*
ComputedColor computePointColor(float3 normal, float3 toEye, float3 vertex)
{
	ComputedColor output;
	output.diffuse = float4(0, 0, 0, 1);
	output.ambient = output.diffuse;
	output.specular = output.diffuse;

	float3 lightVector;
	lightVector = pLight.position - vertex;
	float d;
	d = length(lightVector);
	
	if (d > pLight.range)
	{
		return output;
	}
	
	lightVector = normalize(lightVector);

	output.ambient = pLight.ambient * material.ambient;

	float attenuationFactor = 1.0 / (pLight.attenuation.x + pLight.attenuation.y * d + pLight.attenuation.z * d * d);

	float kd;
	kd = max(dot(lightVector, normal), 0);
	if (kd > 0)
	{
		output.diffuse = kd * pLight.diffuse * material.diffuse * attenuationFactor;
		float3 r;
		r = reflect(-lightVector, normal);
		float ks;
		ks = max(dot(toEye, r), 0);
		ks = pow(ks, material.specular.w);
		float factor;
		factor = ks * attenuationFactor;
		output.specular = factor * pLight.specular * material.specular;
	}

	output.ambient = saturate(output.ambient);
	output.diffuse = saturate(output.diffuse);
	output.specular = saturate(output.specular);

	return output;
}

ComputedColor computeSpotColor(float3 normal, float3 toEye, float3 vertex)
{
	ComputedColor output;
	output.ambient = float4(0, 0, 0, 1);
	output.diffuse = output.ambient;
	output.specular = output.ambient;

	float3 lightVector;
	lightVector = sLight.position - vertex;
	float d;
	d = length(lightVector);

	if (d > sLight.range)
	{
		return output;
	}

	lightVector = lightVector / d;

	float intensityFactor = pow(max(dot(-lightVector, sLight.direction), 0), sLight.spotlightPower);
	intensityFactor = toonKs(intensityFactor);
	float attenuationFactor = 1.0 / (sLight.attenuation.x + sLight.attenuation.y * d + sLight.attenuation.z * d * d);
	
	output.ambient = sLight.ambient * material.ambient * intensityFactor;

	float kd;
	kd = max(dot(lightVector, normal), 0);
	if (kd > 0)
	{
		float foo = intensityFactor * attenuationFactor;
		output.diffuse = kd * sLight.diffuse * material.diffuse * foo;
		float3 r;
		r = reflect(-lightVector, normal);
		float ks;
		ks = pow(max(dot(toEye, r), 0), material.specular.w);
		output.specular = ks * sLight.specular * material.specular * foo;
	}

	output.ambient = saturate(output.ambient);
	output.diffuse = saturate(output.diffuse);
	output.specular = saturate(output.specular);
	return output;
}
*/
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
	litColor.a = material.diffuse.a;
	
	return litColor;
}