#include "MeshData.h"

namespace D3dtut
{
	MeshData::MeshData()
	{
		this->id = D3dtut::generateId();
	}

	const int MeshData::getId() const
	{
		return id;
	}
}