#include "CompiledShader.h"

namespace D3dtut
{
	CompiledShader::CompiledShader(std::string filename) : size(0), compiledShader(0)
	{
		std::ifstream fin;
		fin.open(filename, std::ios::in | std::ios::binary);

		if (!fin)
		{
			throw std::exception(("File not found: " + filename + "\n").c_str());
		}

		fin.seekg(0, std::ios::end);
		size = static_cast<int>(fin.tellg());
		compiledShader = new char[size];
		fin.seekg(0, std::ios::beg);
		fin.read(compiledShader, size);
		fin.close();
	}

	CompiledShader::~CompiledShader()
	{
		if (compiledShader)
		{
			delete[] compiledShader;
		}
	}

	const void* CompiledShader::get() const
	{
		return reinterpret_cast<void*>(compiledShader);
	}

	const int CompiledShader::getSize() const
	{
		return size;
	}
}