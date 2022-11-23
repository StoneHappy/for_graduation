#pragma once
#include <functional>
namespace GU
{
	typedef std::function<void(const char*)> LogFunc;
	struct STDDebugLogFunction
	{
		void operator() (const char* msg);
	};

	struct QtDebugLogFunction
	{
		void operator() (const char* msg);
	};
}