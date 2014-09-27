#pragma once

#include <DirectXMath.h>

using namespace DirectX;
namespace D3dtut
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		XMFLOAT4 color;


		Vertex() {}
		Vertex(XMFLOAT3 p, XMFLOAT3 n, XMFLOAT2 tex, const float* col);
	};
}