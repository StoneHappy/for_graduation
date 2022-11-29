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
#include <Widgets/CPropertyHeader.h>
#include <Widgets/CDoubleProperty.h>
#include <Widgets/CStringProperty.h>
#include <Widgets/NewProjectDialog.h>
#include <QFileDialog>
static QPointer<QPlainTextEdit> s_messageLogWidget;
static QPointer<QFile> s_logFile;


/*******************************Property***********************************************/ 
// Tag Component
CStringProperty* tagProperty;
CStringProperty* uuidProperty;
// Transform Component
CPropertyHeader* tsfheader;
CPropertyHeader* posheader;
CPropertyHeader* rotheader;
CPropertyHeader* sclheader;
CDoubleProperty* pxProperty;
CDoubleProperty* pyProperty;
CDoubleProperty* pzProperty;
CDoubleProperty* rxProperty;
CDoubleProperty* ryProperty;
CDoubleProperty* rzProperty;
CDoubleProperty* sxProperty;
CDoubleProperty* syProperty;
CDoubleProperty* szProperty;
/*******************************Property***********************************************/

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
	craeteComponentView();
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
	m_entityTreeModel = new QStandardItemModel(this);
	m_entityTreeSelectModel = new QItemSelectionModel(m_entityTreeModel, this);

	ui->entityTreeView->setModel(m_entityTreeModel);
	ui->entityTreeView->setSelectionModel(m_entityTreeSelectModel);
	ui->entityTreeView->setHeaderHidden(true);
	ui->entityTreeView->setAlternatingRowColors(true);
	ui->entityTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->entityTreeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slot_treeviewEntity_customcontextmenu(const QPoint&)));
	m_treeviewEntityRoot = new QStandardItem(QString::fromLocal8Bit("场景根节点"));
	QIcon icon;
	icon.addFile(":/images/root.png");
	m_treeviewEntityRoot->setIcon(icon);
	m_entityTreeModel->appendRow(m_treeviewEntityRoot);

	connect(m_entityTreeSelectModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slot_on_entityTreeSelectModel_currentChanged(const QModelIndex&, const QModelIndex&)));
	connect(ui->componentTreeWidget,
		SIGNAL(tagChanged()),
		this,
		SLOT(slot_tagPropertyChanged())
	);
}

void MainWindow::craeteComponentView()
{
	ui->componentTreeWidget->init();
	// Tag Component
	tagProperty = new CStringProperty("tagProperty", QString::fromLocal8Bit("标签"), QString::fromLocal8Bit("entity"));
	uuidProperty = new CStringProperty(QString::fromLocal8Bit("uuidProperty").toUtf8(), QString::fromLocal8Bit("UUID"), QString::fromLocal8Bit("%1").arg(113123123124));
	uuidProperty->setDisabled(true);
	// Transform Component
	tsfheader = new CPropertyHeader("tsfheader", QString::fromLocal8Bit("转换组件"));
	posheader = new CPropertyHeader(tsfheader, "tsfHeader", QString::fromLocal8Bit("位置"));
	rotheader = new CPropertyHeader(tsfheader, "rotheader", QString::fromLocal8Bit("旋转"));
	sclheader = new CPropertyHeader(tsfheader, "sclheader", QString::fromLocal8Bit("缩放"));
	pxProperty = new CDoubleProperty(posheader, "pxProperty", "x:", 0, 0, -1000.0, 1000.0);
	pyProperty = new CDoubleProperty(posheader, "pyProperty", "y:", 0, 0, -1000.0, 1000.0);
	pzProperty = new CDoubleProperty(posheader, "pzProperty", "z:", 0, 0, -1000.0, 1000.0);

	rxProperty = new CDoubleProperty(rotheader, "rxProperty", "x:", 0, 0, -1000.0, 1000.0);
	ryProperty = new CDoubleProperty(rotheader, "ryProperty", "y:", 0, 0, -1000.0, 1000.0);
	rzProperty = new CDoubleProperty(rotheader, "rzProperty", "z:", 0, 0, -1000.0, 1000.0);

	sxProperty = new CDoubleProperty(sclheader, "sxProperty", "x:", 0, 0, -1000.0, 1000.0);
	syProperty = new CDoubleProperty(sclheader, "syProperty", "y:", 0, 0, -1000.0, 1000.0);
	szProperty = new CDoubleProperty(sclheader, "szProperty", "z:", 0, 0, -1000.0, 1000.0);

	//ui->componentTreeWidget->adjustToContents();
}

void MainWindow::clearAllComponentProperty()
{
	ui->componentTreeWidget->remove(tagProperty);
	ui->componentTreeWidget->remove(uuidProperty);
	ui->componentTreeWidget->remove(tsfheader );
	ui->componentTreeWidget->remove(posheader );
	ui->componentTreeWidget->remove(rotheader );
	ui->componentTreeWidget->remove(sclheader );
	ui->componentTreeWidget->remove(pxProperty);
	ui->componentTreeWidget->remove(pyProperty);
	ui->componentTreeWidget->remove(pzProperty);

	ui->componentTreeWidget->remove(rxProperty);
	ui->componentTreeWidget->remove(ryProperty);
	ui->componentTreeWidget->remove(rzProperty);

	ui->componentTreeWidget->remove(sxProperty);
	ui->componentTreeWidget->remove(syProperty);
	ui->componentTreeWidget->remove(szProperty);
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
void MainWindow::on_actNewProject_triggered()
{
	NewProjectDialog* newProjectDlg = new NewProjectDialog(this);

	auto rnt = newProjectDlg->exec();

	if (rnt == QDialog::Accepted)
	{
		QString projectPath = newProjectDlg->m_ProjectPath;
		qDebug(QString("projectPath: %1").arg(projectPath).toUtf8());
	}
}
void MainWindow::on_actOpenProject_triggered()
{
	QFileDialog* fileDlg = new QFileDialog(this, QString::fromLocal8Bit("打开工程"), QDir::currentPath(), QString::fromLocal8Bit("工程文件(*.gu)"));
	auto rnt =  fileDlg->exec();

	if (rnt == QDialog::Accepted)
	{
		auto projectPath = fileDlg->FileName;
	}
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
	item->setData((UINT64)uuid);
	m_entityMap[uuid] = item;
	QIcon icon;
	icon.addFile(":/images/entity.png");
	item->setIcon(icon);
	item->setEditable(false);
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

void MainWindow::slot_on_entityTreeSelectModel_currentChanged(const QModelIndex& currentIndex, const QModelIndex& previousIndex)
{
	clearAllComponentProperty();
	auto item = m_entityTreeModel->itemFromIndex(currentIndex);
	UINT64 uuid = item->data().toLongLong();
	if (uuid == 0) return;
	auto entity = GU::g_CoreContext.g_Scene.getEntityByUUID(uuid);

	if (entity.hasComponent<GU::TagComponent>())
	{
		auto tag = entity.getComponent<GU::TagComponent>().Tag.c_str();
		tagProperty->setValue(tag);
		ui->componentTreeWidget->add(tagProperty);
	}

	if (entity.hasComponent<GU::IDComponent>())
	{
		auto uuid = entity.getComponent<GU::IDComponent>().ID;
		uuidProperty->setValue(QString("%1").arg(uuid));
		ui->componentTreeWidget->add(uuidProperty);
	}

	if (entity.hasComponent<GU::TransformComponent>())
	{
		auto translation = entity.getComponent<GU::TransformComponent>().Translation;
		auto rotation = entity.getComponent<GU::TransformComponent>().Rotation;
		auto scale = entity.getComponent<GU::TransformComponent>().Scale;

		pxProperty->setValue(translation.x);
		pyProperty->setValue(translation.y);
		pzProperty->setValue(translation.z);

		rxProperty->setValue(rotation.x);
		ryProperty->setValue(rotation.y);
		rzProperty->setValue(rotation.z);

		sxProperty->setValue(scale.x);
		syProperty->setValue(scale.y);
		szProperty->setValue(scale.z);

		ui->componentTreeWidget->add(tsfheader);
		ui->componentTreeWidget->add(posheader);
		ui->componentTreeWidget->add(rotheader);
		ui->componentTreeWidget->add(sclheader);


		ui->componentTreeWidget->add(pxProperty);
		ui->componentTreeWidget->add(pyProperty);
		ui->componentTreeWidget->add(pzProperty);

		ui->componentTreeWidget->add(rxProperty);
		ui->componentTreeWidget->add(ryProperty);
		ui->componentTreeWidget->add(rzProperty);

		ui->componentTreeWidget->add(sxProperty);
		ui->componentTreeWidget->add(syProperty);
		ui->componentTreeWidget->add(szProperty);

	}
	ui->componentTreeWidget->adjustToContents();
}
void MainWindow::slot_tagPropertyChanged()
{
	auto item = m_entityTreeModel->itemFromIndex(m_entityTreeSelectModel->currentIndex());
	std::string uuidstring = item->data().toString().toStdString();
	uint64_t uuid;
	sscanf(uuidstring.c_str(), "%llu", &uuid);
	if (uuid == 0) return;
	auto entity = GU::g_CoreContext.g_Scene.getEntityByUUID(uuid);
	auto tag = entity.getComponent<GU::TagComponent>().Tag;
	item->setText(tag.c_str());
}