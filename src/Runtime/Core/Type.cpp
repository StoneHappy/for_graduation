#include "Type.h"
#include <iostream>
#include <QDebug>
namespace GU
{
	void STDDebugLogFunction::operator()(const char* msg)
	{
		std::cout << msg << std::endl;
	}

	void QtDebugLogFunction::operator() (const char* msg)
	{
		qDebug(QString::fromLocal8Bit(msg).toUtf8());
	}
}