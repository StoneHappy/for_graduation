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
			g_daltaTime = 0;
		}
		else if(g_isPlay)
		{
			auto currentTime = std::chrono::high_resolution_clock::now();
			g_daltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - g_lastTimePoint).count();
			g_timeIntegral += g_daltaTime;
		}
		else
		{
			g_daltaTime = 0;
		}
		g_lastTimePoint = std::chrono::high_resolution_clock::now();
	}
}