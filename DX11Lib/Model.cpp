#include "Model.h"

namespace D3dtut
{
	Model::Model(int i, std::string name) : id(i), name(name)
	{

	}

	const int Model::getId() const
	{
		return id;
	}
}