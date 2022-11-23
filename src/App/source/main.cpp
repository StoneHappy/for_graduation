#include <iostream>
#include <QApplication>
#include <QSplashScreen>
#include <MainWindow.h>
#include <QDateTime>
int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QPixmap pixmap(QString::fromLocal8Bit(":/images/校徽.png"));
	QSplashScreen splash(pixmap);
	splash.show();
	splash.showMessage(QString::fromLocal8Bit("程序正在加载...."));

#if 0
	QDateTime time = QDateTime::currentDateTime();
	QDateTime currentTime = QDateTime::currentDateTime();
	while (time.secsTo(currentTime) <= 5)
	{
		currentTime = QDateTime::currentDateTime();
		a.processEvents();
	}
#endif

	MainWindow w;

	w.showMaximized();

	splash.finish(&w);

	return a.exec();
}
