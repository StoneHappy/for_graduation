#pragma once
#include <Core/Type.h>
#include <chrono>
#include <Scene/Scene.h>
namespace GU
{
	class CoreContext;

	extern CoreContext g_CoreContext;

	class CoreContext
	{
	public:
		CoreContext();
		LogFunc g_Log = STDDebugLogFunction();
		std::chrono::steady_clock::time_point g_lastTimePoint;
		float g_deltaTime = 0;
		float g_editDeltaTime = 0;
		float g_timeIntegral = 0;
		bool g_isPlay = false;
		bool g_isStop = true;
		float g_winWidth = 1280.0, g_winHeight = 720.0;
		Scene g_Scene;
		void timeTick();

	};

}
#define DEBUG_LOG(msg) ::GU::g_CoreContext.g_Log(::GU::LogType::Debug ,msg)
#define WARNING_LOG(msg) ::GU::g_CoreContext.g_Log(::GU::LogType::Warning ,msg)
#define CRITICAL_LOG(msg) ::GU::g_CoreContext.g_Log(::GU::LogType::Critical ,msg)
#define FATAL_LOG(msg) ::GU::g_CoreContext.g_Log(::GU::LogType::Fatal ,msg)
