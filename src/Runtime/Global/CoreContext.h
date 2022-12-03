#pragma once
#include <memory>
#include <Core/Type.h>
class MainWindow;
class ThreadPool;
namespace GU
{
	class CoreContext;
	class Scene;
	class VulkanContext;
	class Project;
	class Asset;
	extern CoreContext g_CoreContext;

	class CoreContext
	{
	public:
		CoreContext();
		~CoreContext();
		QtDebugLogFunction g_Log = QtDebugLogFunction();
		std::chrono::steady_clock::time_point g_lastTimePoint;
		float g_deltaTime = 0;
		float g_editDeltaTime = 0;
		float g_timeIntegral = 0;
		bool g_isPlay = false;
		bool g_isStop = true;
		float g_winWidth = 1280.0, g_winHeight = 720.0;
		std::shared_ptr<Scene> g_scene;
		void timeTick();
		std::shared_ptr<VulkanContext> g_vulkanContext;
		std::shared_ptr<Asset> g_asset;
		std::shared_ptr<ThreadPool> g_threadPool;
		MainWindow* g_p_mainWindow;
		std::shared_ptr<Project> g_Proejct;
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
#define GLOBAL_SAVE_PROJECT(projectfile) ::GU::g_CoreContext.g_Proejct->save(projectfile)
#define GLOBAL_OPEN_PROJECT(projectfile) ::GU::g_CoreContext.g_Proejct->open(projectfile)
#define GLOBAL_PROJECT_FILE_PATH ::GU::g_CoreContext.g_Proejct->projectFilePath
#define GLOBAL_PROJECT_PATH ::GU::g_CoreContext.g_Proejct->projectDirPath
#define GLOBAL_ASSET_PATH ::GU::g_CoreContext.g_Proejct->assetDirPath
#define GLOBAL_MODEL_PATH ::GU::g_CoreContext.g_Proejct->modelDirPath
#define GLOBAL_THREAD_POOL ::GU::g_CoreContext.g_threadPool
#define GLOBAL_TIME_TICK() ::GU::g_CoreContext.timeTick()
