#include "IdGenerator.h"

namespace D3dtut
{
	int generateId()
	{
		static int id = 0;
		return id++;
	}
}