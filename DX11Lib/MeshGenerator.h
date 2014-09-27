#pragma once

#include "MeshData.h"
#include <cassert>

namespace D3dtut
{
	class MeshGenerator
	{
	public: 
		static MeshData CreateGrid(float lengthX, float lengthZ, int numX, int numZ, const float* color, float(*f)(float, float));
		static MeshData CreateFrustum(float topRadius, float bottomRadius, int numStacks, int numVerticalSections, float height, const float* color);
		static MeshData CreateSphere(float radius, int numStacks, int numVerticalSections, const float* color);
	};
}