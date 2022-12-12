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
}
AddAgentDlg::~AddAgentDlg()
{
    delete ui;
}
