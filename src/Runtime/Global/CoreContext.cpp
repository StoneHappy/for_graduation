#include "CoreContext.h"
#include <Core/Type.h>
#include <chrono>
#include <Scene/Scene.h>
#include <Scene/Asset.h>
#include <Renderer/VulkanContext.h>
#include <filesystem>
#include <Core/ThreadPool.h>
#include <Core/Project.h>
#include <Function/AgentNav/RCScheduler.h>
namespace GU
{
	CoreContext g_CoreContext;
	CoreContext::CoreContext()
	{
		g_lastTimePoint = std::chrono::high_resolution_clock::now();

		g_scene = std::make_shared<Scene>();
		g_vulkanContext = std::make_shared<VulkanContext>();
		g_asset = std::make_shared<Asset>();
		g_Proejct = std::make_shared<Project>();
		g_threadPool = std::make_shared<::ThreadPool>(8);
		g_rcScheduler = std::make_shared<RCScheduler>();
	}
	CoreContext::~CoreContext()
	{

	}
	void CoreContext::timeTick()
	{
		if (g_isStop)
		{
			g_timeIntegral = 0;
			auto currentTime = std::chrono::high_resolution_clock::now();
			g_editDeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - g_lastTimePoint).count();
		}
		else if(g_isPlay)
		{
			auto currentTime = std::chrono::high_resolution_clock::now();
			g_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - g_lastTimePoint).count();
			g_timeIntegral += g_deltaTime;
			g_editDeltaTime = 0;
		}
		else
		{
			g_deltaTime = 0;
			g_editDeltaTime = 0;
		}
		g_lastTimePoint = std::chrono::high_resolution_clock::now();
	}
}