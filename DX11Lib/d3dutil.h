#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <stdexcept>
#include <string>
#include "D3DException.h"

#define WIN32_LEAN_AND_MEAN

namespace D3dtut
{
	namespace D3dutil
	{
		inline void HR(HRESULT hresult)
		{
			if (FAILED(hresult))
			{
				LPTSTR errorText = NULL;

				FormatMessage(
					FORMAT_MESSAGE_FROM_SYSTEM
					| FORMAT_MESSAGE_ALLOCATE_BUFFER
					| FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					hresult,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR)&errorText,
					0,
					NULL);

				if (errorText != NULL)
				{
					throw D3DException(errorText);
				}
			}
		}

		template <typename T>
		class ComPtr
		{
		private:
			T* ptr;

		public:
			ComPtr() : ptr(nullptr) {}
			~ComPtr()
			{
				this->release();
			}

			void release()
			{
				if (ptr != nullptr)
				{
					ptr->Release();
					ptr = nullptr;
				}
			}

			T* operator->()
			{
				return ptr;
			}

			T* get()
			{
				return ptr;
			}

			T** address()
			{
				return &ptr;
			}
		};

	}
}