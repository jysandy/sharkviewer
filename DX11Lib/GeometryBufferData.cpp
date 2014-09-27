#include "GeometryBufferData.h"

namespace D3dtut
{
	const vector<Vertex>& GeometryBufferData::getVertices() const
	{
		return vertices;
	}

	const vector<unsigned int>& GeometryBufferData::getIndices() const
	{
		return indices;
	}

	const vector<IndexOffsetData>& GeometryBufferData::getIndexOffsetData() const
	{
		return indexData;
	}

	void GeometryBufferData::Append(MeshData data)
	{
		IndexOffsetData appendData;
		appendData.baseVertexOffset = vertices.size();
		appendData.baseIndexOffset = indices.size();
		appendData.indexCount = data.indices.size();

		if (data.getId() >= indexData.size())
		{
			indexData.resize(data.getId() + 10);
		}

		indexData[data.getId()] = appendData;

		vertices.insert(vertices.end(), data.vertices.begin(), data.vertices.end());
		indices.insert(indices.end(), data.indices.begin(), data.indices.end());
	}
}