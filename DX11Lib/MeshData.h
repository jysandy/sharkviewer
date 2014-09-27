#pragma once

#include <vector>
#include "Vertex.h"
#include "IdGenerator.h"

using std::vector;

namespace D3dtut
{
	struct MeshData
	{
		vector<unsigned int> indices;
		vector<Vertex> vertices; 
		const int getId() const;
		MeshData();
		
	private:
		int id;
	};
}