#pragma once
#include <string>
#include <Windows.h>

namespace D3dtut
{
	namespace D3dutil
	{
		class D3DException
		{
		public:
			std::wstring error;

			D3DException(LPTSTR foo) : error(foo) {}
		};
	}
}