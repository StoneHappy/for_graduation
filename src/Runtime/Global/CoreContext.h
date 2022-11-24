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
#define GLOBAL_LOG(msg) ::GU::g_CoreContext.g_Log("正在创建渲染管线")
