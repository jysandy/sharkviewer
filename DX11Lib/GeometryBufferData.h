#pragma once

#include <DirectXMath.h>
#include <vector>
#include "IndexOffsetData.h"
#include "Vertex.h"
#include "MeshData.h"

using std::vector;

namespace D3dtut
{
	class GeometryBufferData
	{
	public:
		const vector<Vertex>& getVertices() const;
		const vector<unsigned int>& getIndices() const;
		const vector<IndexOffsetData>& getIndexOffsetData() const;

		void Append(MeshData data);

	private:
		vector<IndexOffsetData> indexData;		//Indexed by ID
		vector<unsigned int> indices;
		vector<Vertex> vertices;
	};
}