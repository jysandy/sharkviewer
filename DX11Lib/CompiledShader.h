#pragma once

#include <fstream>
#include <string>
#include <stdexcept>

namespace D3dtut
{
	class CompiledShader
	{
	public:
		CompiledShader(std::string filename);
		~CompiledShader();

		const void* get() const;
		const int getSize() const;

	private:
		char *compiledShader;
		int size;
	};
}