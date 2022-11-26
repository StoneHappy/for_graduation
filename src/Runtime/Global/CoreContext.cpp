#include "CoreContext.h"

namespace GU
{
	CoreContext g_CoreContext;
	CoreContext::CoreContext()
	{
		g_lastTimePoint = std::chrono::high_resolution_clock::now();
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