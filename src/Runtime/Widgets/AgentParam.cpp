#include "AgentParam.h"
#include "ui_AgentParam.h"
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
AgentParam::AgentParam(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AgentParam)
{
    ui->setupUi(this);
}
void AgentParam::on_pushButtonSet_clicked()
{
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
	GLOBAL_RCSCHEDULER->isConsiderDie = ui->isConsiderDie->isChecked();
	accept();
}
AgentParam::~AgentParam()
{
    delete ui;
}
