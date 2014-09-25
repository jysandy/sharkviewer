#pragma once

#include <DirectXMath.h>

namespace FBXDemo
{
	struct DirectionalLight
	{
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;
		DirectX::XMFLOAT3 direction;
		float pad;
	};
}