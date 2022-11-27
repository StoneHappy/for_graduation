#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMdiArea>
#include <QLoggingCategory>
#include <QPlainTextEdit>
#include <QPointer>
#include <QMenu>
#include <QToolButton>
#include <Widgets/AboutDialog.h>
#include <QLabel>
#include <QDateTime>
#include <Global/CoreContext.h>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QPoint>
#include <Scene/Entity.h>
#include <Scene/Component.h>
static QPointer<QPlainTextEdit> s_messageLogWidget;
static QPointer<QFile> s_logFile;
static void messageHandler(QtMsgType msgType, const QMessageLogContext& logContext, const QString& msg)
{

	QString txt;
	switch (msgType) {
	case QtDebugMsg:
		txt = QString("[Debug]: %1").arg(msg);
		break;
	case QtWarningMsg:
		txt = QString("[Warning]: %1").arg(msg);
		break;
	case QtCriticalMsg:
		txt = QString("[Critical]: %1").arg(msg);
		break;
	case QtFatalMsg:
		txt = QString("[Fatal]: %1").arg(msg);
		abort();
	}

	if (!s_messageLogWidget.isNull())
		s_messageLogWidget->appendPlainText(txt);
	QTextStream ts(s_logFile);
	ts << txt << endl;
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_vulkanWindow(new GU::VulkanWindow())
{
    ui->setupUi(this);

	// Debug log
	s_messageLogWidget = ui->debugLog;
	s_messageLogWidget->setReadOnly(true);
	QDateTime currDateTime = QDateTime::currentDateTime();
	s_logFile = new QFile(QString("./logs/%1-log.txt").arg(currDateTime.toString("yyyy-MM-dd-hh-mm-ss-zzz")));
	s_logFile->open(QIODevice::WriteOnly | QIODevice::Append);
	s_logFile->resize(0);
	qInstallMessageHandler(messageHandler);

	// vulkanWindow
    QWidget* vulkanWindowWrapper = QWidget::createWindowContainer(m_vulkanWindow);
	QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));
	inst = new QVulkanInstance();
	inst->setLayers(QByteArrayList()
		<< "VK_LAYER_GOOGLE_threading"
		<< "VK_LAYER_LUNARG_parameter_validation"
		<< "VK_LAYER_LUNARG_object_tracker"
		<< "VK_LAYER_LUNARG_core_validation"
		<< "VK_LAYER_LUNARG_image"
		<< "VK_LAYER_LUNARG_swapchain"
		<< "VK_LAYER_GOOGLE_unique_objects");

	if (!inst->create())
		qFatal("Failed to create Vulkan instance: %d", inst->errorCode());
	m_vulkanWindow->setVulkanInstance(inst);
	setCentralWidget(vulkanWindowWrapper);

	// pop Menu
	createPopMenu();
	createEntityView();

	// statusbar
	m_mousePosition = new QLabel(this);
	m_mousePosition->setText(QString::fromLocal8Bit("正在加载程序..."));
	ui->statusbar->addWidget(m_mousePosition);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createPopMenu()
{
	QMenu* menu = new QMenu(this);
	menu->addAction(ui->actShowViewDock);
	menu->addAction(ui->actShowInfoDock);
	QToolButton* toolBtn = new QToolButton(this);
	toolBtn->setPopupMode(QToolButton::InstantPopup);
	toolBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	toolBtn->setDefaultAction(ui->actSelPopMenu);
	toolBtn->setMenu(menu);
	ui->toolBar->addSeparator();
	ui->toolBar->addWidget(toolBtn);

	ui->toolBar->addSeparator();
	ui->toolBar->addAction(ui->actPlay);
	ui->toolBar->addAction(ui->actPause);
	ui->actPause->setVisible(false);
	ui->toolBar->addAction(ui->actStop);
	ui->toolBar->addSeparator();
	ui->toolBar->addAction(ui->actAbout);
}

void MainWindow::createEntityView()
{
	m_model = new QStandardItemModel(this);
	m_selectModel = new QItemSelectionModel(m_model, this);
	ui->entityTreeView->setModel(m_model);
	ui->entityTreeView->setHeaderHidden(true);
	ui->entityTreeView->setAlternatingRowColors(true);
	ui->entityTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->entityTreeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slot_treeviewEntity_customcontextmenu(const QPoint&)));
	m_treeviewEntityRoot = new QStandardItem(QString::fromLocal8Bit("场景根节点"));
	QIcon icon;
	icon.addFile(":/images/root.png");
	m_treeviewEntityRoot->setIcon(icon);
	m_model->appendRow(m_treeviewEntityRoot);
}

void MainWindow::craeteComponentView()
{
}

void MainWindow::on_actShowViewDock_triggered()
{
	if (ui->actShowViewDock->isChecked())
		ui->dockEntity->show();
	else
		ui->dockEntity->hide();
}

void MainWindow::on_actShowInfoDock_triggered()
{
	if (ui->actShowInfoDock->isChecked())
		ui->dockInfo->show();
	else
		ui->dockInfo->hide();
}

void MainWindow::on_actAbout_triggered()
{
	AboutDialog* aboutDialog = new AboutDialog(this);
	aboutDialog->exec();
}

void MainWindow::on_actPlay_triggered()
{
	if (ui->actPlay->isVisible()) ui->actPlay->setVisible(false);
	if (!ui->actPause->isVisible()) ui->actPause->setVisible(true);
	ui->dockEntity->hide();
	ui->dockInfo->hide();
	ui->actShowInfoDock->setChecked(false);
	ui->actShowViewDock->setChecked(false);
	GU::g_CoreContext.g_isStop = false;
	GU::g_CoreContext.g_isPlay = true;
}

void MainWindow::on_actPause_triggered()
{
	if (ui->actPause->isVisible()) ui->actPause->setVisible(false);
	if (!ui->actPlay->isVisible()) ui->actPlay->setVisible(true);
	GU::g_CoreContext.g_isPlay = false;
	GU::g_CoreContext.g_isStop = false;
}

void MainWindow::on_actStop_triggered()
{
	if (ui->actPause->isVisible()) ui->actPause->setVisible(false);
	if (!ui->actPlay->isVisible()) ui->actPlay->setVisible(true);
	ui->dockEntity->show();
	ui->dockInfo->show();
	ui->actShowInfoDock->setChecked(true);
	ui->actShowViewDock->setChecked(true);
	GU::g_CoreContext.g_isPlay = false;
	GU::g_CoreContext.g_isStop = true;
}

void MainWindow::on_actCreateEntity_triggered()
{
	auto entity =  GU::g_CoreContext.g_Scene.createEntity();
	auto uuid = entity.getComponent<GU::IDComponent>().ID;
	auto name = entity.getComponent<GU::TagComponent>().Tag;
	QStandardItem* item = new QStandardItem(name.c_str());
	m_entityMap[uuid] = item;
	QIcon icon;
	icon.addFile(":/images/entity.png");
	item->setIcon(icon);
	m_treeviewEntityRoot->appendRow(item);
}

void MainWindow::on_actCopyEntity_triggered()
{

}

void MainWindow::on_actDeleteEntity_triggered()
{

}

void MainWindow::slot_treeviewEntity_customcontextmenu(const QPoint& point)
{
	QMenu* menu = new QMenu(this);
	menu->addAction(ui->actCreateEntity);
	menu->addAction(ui->actCopyEntity);
	menu->addAction(ui->actDeleteEntity);
	menu->exec(QCursor::pos());
}



