#pragma once
#include <Core/Type.h>
namespace GU
{
	class GlobalContext;

	extern GlobalContext g_GlobalContext;

	class GlobalContext
	{
	public:
		LogFunc g_Log = STDDebugLogFunction();
	};
}