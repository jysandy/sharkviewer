#include "Vertex.h"

namespace D3dtut
{
	Vertex::Vertex(XMFLOAT3 p, XMFLOAT3 n, XMFLOAT2 tex, const float* col)
		: position(p), normal(n), texCoord(tex), color(col)
	{

	}
}