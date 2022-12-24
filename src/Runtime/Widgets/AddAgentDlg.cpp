#include "AddAgentDlg.h"
#include "ui_AddAgentDlg.h"
#include <Global/CoreContext.h>
#include <MainWindow.h>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <Scene/Asset.h>
#include <Scene/Scene.h>
#include <Scene/Component.h>
#include <Scene/Entity.h>
#include <Function/Animation/Animation.h>
#include <Function/AgentNav/RCScheduler.h>
#include <DetourCrowd.h>
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
AddAgentDlg::AddAgentDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAgentDlg)
{
    ui->setupUi(this);
}
void AddAgentDlg::on_pushButtonAddAgent_clicked()
{
	if (ui->pushButtonAddAgent->isChecked())
	{
		ui->pushButtonSetTarget->setChecked(false);
		GLOBAL_RCSCHEDULER->isSetAgent = true;
		GLOBAL_RCSCHEDULER->isSetTarget = false;
	}
	if (!ui->pushButtonAddAgent->isChecked())
	{
		GLOBAL_RCSCHEDULER->isSetAgent = false;
		GLOBAL_RCSCHEDULER->isSetTarget = false;
		return;
	}

	dtCrowdAgentParams ap;
	memset(&ap, 0, sizeof(ap));
	ap.height = GLOBAL_RCSCHEDULER->m_rcparams.m_agentHeight;
	ap.radius = GLOBAL_RCSCHEDULER->m_rcparams.m_agentRadius;
	ap.maxAcceleration = 8.0f;
	ap.maxSpeed = 3.5f;
	ap.collisionQueryRange = ap.radius * 12.0f;
	ap.pathOptimizationRange = ap.radius * 30.0f;
	ap.updateFlags = 0;
	if (ui->anticipateTurn->isChecked())
		ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
	if (ui->optimizePathVisibility->isChecked())
		ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
	if (ui->optimizePathTopology->isChecked())
		ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
	if (ui->obstacleAvoidance->isChecked())
		ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
	if (ui->separationWeight->value()!=0)
		ap.updateFlags |= DT_CROWD_SEPARATION;
	ap.obstacleAvoidanceType = (unsigned char)3.0f;
	ap.separationWeight = ui->separationWeight->value();

	GLOBAL_RCSCHEDULER->agentParams = ap;
}
AddAgentDlg::~AddAgentDlg()
{
    delete ui;
}

void AddAgentDlg::on_pushButtonSetTarget_clicked()
{
	if (ui->pushButtonSetTarget->isChecked())
	{
		ui->pushButtonAddAgent->setChecked(false);
		GLOBAL_RCSCHEDULER->isSetAgent = false;
		GLOBAL_RCSCHEDULER->isSetTarget = true;
	}
}

void AddAgentDlg::closeEvent(QCloseEvent*)
{
	GLOBAL_RCSCHEDULER->isSetAgent = false;
	GLOBAL_RCSCHEDULER->isSetTarget = false;
}
