#include "Type.h"
#include <iostream>
#include <QDebug>
namespace GU
{
	void STDDebugLogFunction::operator()(LogType tpye, const char* msg)
	{
		std::cout << msg << std::endl;
	}

	void QtDebugLogFunction::operator() (LogType type, const char* msg)
	{
		switch (type)
		{
		case GU::LogType::Debug:
			qDebug(QString::fromLocal8Bit(msg).toUtf8());
			break;
		case GU::LogType::Warning:
			qWarning(QString::fromLocal8Bit(msg).toUtf8());
			break;
		case GU::LogType::Critical:
			qCritical(QString::fromLocal8Bit(msg).toUtf8());
			break;
		case GU::LogType::Fatal:
			qFatal(QString::fromLocal8Bit(msg).toUtf8());
			break;
		default:
			break;
		}
	}
}