#pragma once
#include <functional>
#include <string>
#include <cstdio>
#include <iostream>
#include <QString>
#include <QDebug>
namespace GU
{
	enum class LogType
	{
		Debug,
		Warning,
		Critical,
		Fatal
	};

	class QtDebugLogFunction
	{
	public:
		template <typename... Args>
		void operator() (LogType type, const char* msg, Args&&... args)
		{
			switch (type)
			{
			case GU::LogType::Debug:
				qDebug(QString::fromLocal8Bit(msg).toUtf8(), args...);
				break;
			case GU::LogType::Warning:
				qWarning(QString::fromLocal8Bit(msg).toUtf8(), args...);
				break;
			case GU::LogType::Critical:
				qCritical(QString::fromLocal8Bit(msg).toUtf8(), args...);
				break;
			case GU::LogType::Fatal:
				qFatal(QString::fromLocal8Bit(msg).toUtf8(), args...);
				break;
			default:
				break;
			}
		}
	};
}