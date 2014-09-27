#pragma once

#include <DirectXMath.h>
#include <string>

using namespace DirectX;

namespace D3dtut
{
	struct Model
	{
		XMFLOAT4X4 world;
		std::string name;

		Model(int i, std::string name);
		const int getId() const;

	private:
		int id;
	};
}