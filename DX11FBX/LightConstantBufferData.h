#pragma once
#include <DirectXMath.h>
#include "Material.h"
#include "DirectionalLight.h"

namespace FBXDemo
{
	struct LightConstantBufferData
	{
		Material material;
		DirectionalLight dLight;
		DirectX::XMFLOAT3 cameraPosition;
		float pad;
	};
}