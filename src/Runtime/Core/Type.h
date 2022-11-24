#pragma once
#include <functional>
namespace GU
{
	enum class LogType
	{
		Debug,
		Warning,
		Critical,
		Fatal
	};

	typedef std::function<void(LogType, const char*)> LogFunc;
	struct STDDebugLogFunction
	{
		void operator() (LogType type, const char* msg);
	};

	struct QtDebugLogFunction
	{
		void operator() (LogType type, const char* msg);
	};
}