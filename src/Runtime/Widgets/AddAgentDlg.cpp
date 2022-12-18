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
void AddAgentDlg::on_pushButtonOK_clicked()
{
	auto modelitem = GLOBAL_MAINWINDOW->m_skeletalmeshTableModel->itemFromIndex(GLOBAL_MAINWINDOW->m_skeletalmeshTableSelectModel->currentIndex());
	auto textureitem = GLOBAL_MAINWINDOW->m_textureTableModel->itemFromIndex(GLOBAL_MAINWINDOW->m_textureTableSelectModel->currentIndex());
	if (modelitem == nullptr || textureitem == nullptr || modelitem->data().isNull() || textureitem->data().isNull())
	{
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Icon::Critical);
		msgBox.setText(QString::fromLocal8Bit("未选择模型或者贴图"));
		msgBox.exec();
		return;
	}
	auto entity = GLOBAL_SCENE->createEntity();
	auto& materialComponent = entity.addComponent<GU::SkeletalMeshComponent>(modelitem->data().toULongLong(), textureitem->data().toULongLong());
	materialComponent.currentAnimation = GLOBAL_ANIMATION->getAnimationsWithUUID(GLOBAL_ASSET->getSkeletalMeshWithUUID(modelitem->data().toULongLong())->meshs[0].animationID).begin()->first;
	auto&& transform = entity.getComponent<GU::TransformComponent>();
	transform.Translation = GLOBAL_RCSCHEDULER->hitPos;
	glm::vec3 org = glm::normalize(glm::vec3(0, 0, 1.0));
	glm::vec3 dest = glm::normalize(glm::vec3(1.0, 0.0, -1.0));
	auto rot = glm::rotation(org, dest);
	auto euler = glm::eulerAngles(rot);
	transform.Rotation.x = euler.x;
	transform.Rotation.y = euler.y;
	transform.Rotation.z = euler.z;

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

	int idx = GLOBAL_RCSCHEDULER->addAgent(GLOBAL_RCSCHEDULER->hitPos, ap);
	GLOBAL_RCSCHEDULER->setMoveTarget(idx, { 25.2513, -2.37028, 23.9598 });
	glm::vec3 targetpos = { 25.2513, -2.37028, 23.9598 };
	auto&& agentcomponent = entity.addComponent<::GU::AgentComponent>(idx, targetpos);
}
AddAgentDlg::~AddAgentDlg()
{
    delete ui;
}
