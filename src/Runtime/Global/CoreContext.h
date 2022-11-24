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
#define DEBUG_LOG(msg) ::GU::g_CoreContext.g_Log(LogType::Debug ,msg)
#define WARNING_LOG(msg) ::GU::g_CoreContext.g_Log(LogType::Warning ,msg)
#define CRITICAL_LOG(msg) ::GU::g_CoreContext.g_Log(LogType::Critical ,msg)
#define FATAL_LOG(msg) ::GU::g_CoreContext.g_Log(LogType::Fatal ,msg)
