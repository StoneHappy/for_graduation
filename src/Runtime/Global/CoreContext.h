#pragma once
#include <Core/Type.h>
namespace GU
{
	class CoreContext;

	extern CoreContext g_CoreContext;

	class CoreContext
	{
	public:
		LogFunc g_Log = STDDebugLogFunction();
	};
}