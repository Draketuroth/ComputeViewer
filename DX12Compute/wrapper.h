#include "pch.h"
#include "Compute/Compute.h"

extern "C"
{
	__declspec(dllexport) void* createCompute(void)
	{
		return new Compute();
	}

	__declspec(dllexport) void destroyCompute(void* ptr)
	{
		Compute* ref = reinterpret_cast<Compute*>(ptr);
		delete ref;
	}

	__declspec(dllexport) void setShaderPath(void* ptr, const wchar_t* path)
	{
		Compute* ref = reinterpret_cast<Compute*>(ptr);
		ref->SetShaderPath(path);
	}

	__declspec(dllexport) void setTextureSize(void* ptr, int width, int height, int pixelSize)
	{
		Compute* ref = reinterpret_cast<Compute*>(ptr);
		ref->SetTextureSize(width, height, pixelSize);
	}
	__declspec(dllexport) void setDispatchSize(void* ptr, int x, int y, int z)
	{
		Compute* ref = reinterpret_cast<Compute*>(ptr);
		ref->SetDispatchSize(x, y, z);
	}

	__declspec(dllexport) void dispatch(void* ptr)
	{
		Compute* ref = reinterpret_cast<Compute*>(ptr);
		ref->Dispatch();
	}

	__declspec(dllexport) unsigned char* readBackData(void* ptr)
	{
		Compute* ref = reinterpret_cast<Compute*>(ptr);
		return ref->GetImageData();
	}
};