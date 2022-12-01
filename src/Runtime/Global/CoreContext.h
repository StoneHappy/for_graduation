#pragma once
#include <Core/Type.h>
#include <chrono>
#include <Scene/Scene.h>
#include <Scene/Asset.h>
#include <Renderer/VulkanContext.h>
#include <filesystem>
#include <Core/ThreadPool.h>
#include <Core/Project.h>
class MainWindow;
namespace GU
{
	class CoreContext;
	extern CoreContext g_CoreContext;

	class CoreContext
	{
	public:
		CoreContext();
		QtDebugLogFunction g_Log = QtDebugLogFunction();
		std::chrono::steady_clock::time_point g_lastTimePoint;
		float g_deltaTime = 0;
		float g_editDeltaTime = 0;
		float g_timeIntegral = 0;
		bool g_isPlay = false;
		bool g_isStop = true;
		float g_winWidth = 1280.0, g_winHeight = 720.0;
		Scene g_scene;
		void timeTick();
		VulkanContext g_vulkanContext;
		Asset g_asset;
		ThreadPool g_threadPool{8};
		MainWindow* g_p_mainWindow;
		Project g_Proejct;
	};

}
#define DEBUG_LOG(msg, ...) ::GU::g_CoreContext.g_Log(::GU::LogType::Debug ,msg, __VA_ARGS__)
#define WARNING_LOG(msg, ...) ::GU::g_CoreContext.g_Log(::GU::LogType::Warning ,msg, __VA_ARGS__)
#define CRITICAL_LOG(msg, ...) ::GU::g_CoreContext.g_Log(::GU::LogType::Critical ,msg, __VA_ARGS__)
#define FATAL_LOG(msg, ...) ::GU::g_CoreContext.g_Log(::GU::LogType::Fatal ,msg, __VA_ARGS__)

#define GLOBAL_ASSET ::GU::g_CoreContext.g_asset
#define GLOBAL_MAINWINDOW ::GU::g_CoreContext.g_p_mainWindow
#define GLOBAL_VULKAN_CONTEXT ::GU::g_CoreContext.g_vulkanContext
#define GLOBAL_DELTATIME ::GU::g_CoreContext.g_deltaTime
#define GLOBAL_PLAY ::GU::g_CoreContext.g_isPlay
#define GLOBAL_STOP ::GU::g_CoreContext.g_isStop
#define GLOBAL_SCENE ::GU::g_CoreContext.g_scene
#define GLOBAL_SAVE_PROJECT(projectfile) ::GU::g_CoreContext.g_Proejct.save(projectfile)
#define GLOBAL_OPEN_PROJECT(projectfile) ::GU::g_CoreContext.g_Proejct.open(projectfile)
#define GLOBAL_PROJECT_FILE_PATH ::GU::g_CoreContext.g_Proejct.projectFilePath
#define GLOBAL_PROJECT_PATH ::GU::g_CoreContext.g_Proejct.projectDirPath
#define GLOBAL_ASSET_PATH ::GU::g_CoreContext.g_Proejct.assetDirPath
#define GLOBAL_TIME_TICK() ::GU::g_CoreContext.timeTick()
