#include "NavMeshParamsDlg.h"
#include "ui_NavMeshParamsDlg.h"
#include <Function/AgentNav/RCScheduler.h>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <Global/CoreContext.h>
#include <Scene/Asset.h>
#include <Renderer/Mesh.h>
#include <Function/AgentNav/RCScheduler.h>
#include <QTableView>
#include <QHeaderView>
#include <Widgets/MeshSelectDialog.h>
#include <QMessageBox>
NavMeshParamsDlg::NavMeshParamsDlg(QWidget *parent, QStandardItemModel* p_meshTableModel,QItemSelectionModel* p_meshTableSelectModel) 
	:QDialog(parent), ui(new Ui::NavMeshParamsDlg), m_meshTableModel(p_meshTableModel), m_meshTableSelectModel(p_meshTableSelectModel)
{
    ui->setupUi(this);
	connect(ui->isRenderHF, SIGNAL(stateChanged(int)), this, SLOT(on_IsRenderHFStateChanged(int)));
	connect(ui->isRenderCC, SIGNAL(stateChanged(int)), this, SLOT(on_IsRenderCTStateChanged(int)));
	connect(ui->isRenderDM, SIGNAL(stateChanged(int)), this, SLOT(on_IsRenderDMStateChanged(int)));
	connect(ui->isRenderTContour, SIGNAL(stateChanged(int)), this, SLOT(on_IsRenderTContourChanged(int)));
	connect(ui->isRenderTCompactField, SIGNAL(stateChanged(int)), this, SLOT(on_IsRenderTCompactFieldChanged(int)));
}

void NavMeshParamsDlg::on_pushButtonOK_clicked()
{
	// Init build configuration from GUI
	rc_params.m_cellSize = ui->p_cs->value();
	rc_params.m_cellHeight = ui->p_ch->value();
	rc_params.m_agentMaxSlope = ui->p_agentMaxSlope->value();
	rc_params.m_agentHeight = ui->p_agentHeight->value();
	rc_params.m_agentMaxClimb = ui->p_agentMaxClimb->value();
	rc_params.m_agentRadius = ui->p_agentRadius->value();
	rc_params.m_edgeMaxLen = ui->p_edgeMaxLen->value();
	rc_params.m_edgeMaxError = ui->p_edgeMaxError->value();
	rc_params.m_detailSampleMaxError = ui->p_detailSampleMaxError->value();
	rc_params.m_regionMinSize = ui->p_regionMinSize->value();		
	rc_params.m_regionMergeSize = ui->p_regionMergeSize->value();
	rc_params.m_vertsPerPoly = ui->p_vertsPerPoly->value();
	rc_params.m_detailSampleDist = ui->p_detailSampleDist->value();
	rc_params.m_detailSampleMaxError =ui->p_detailSampleMaxError->value();
	rc_params.m_partitionType = ui->p_WATERSHED->isChecked() * 0 + ui->p_MONOTONE->isChecked() * 1 + ui->p_LAYERS->isChecked() * 2;
	rc_params.m_filterLedgeSpans = ui->p_filterLedgeSpans->isChecked();
	rc_params.m_filterLowHangingObstacles = ui->p_filterLowHangingObstacles->isChecked();
	rc_params.m_filterWalkableLowHeightSpans = ui->p_m_filterWalkableLowHeightSpans->isChecked();
	rc_params.m_keepInterResults = ui->p_keepInterResults->isChecked();

	auto item = m_meshTableModel->itemFromIndex(m_meshTableSelectModel->currentIndex());

	if (item == nullptr || item->data().isNull())
	{
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Icon::Critical);
		msgBox.setText(QString::fromLocal8Bit("Î´Ñ¡ÔñÄ£ÐÍ"));
		msgBox.exec();
		return;
	}
	auto uuid = item->data().toULongLong();
	auto meshnode = GLOBAL_ASSET->getMeshWithUUID(uuid);
	auto& mesh = meshnode->meshs[0];
	auto rcparams = rc_params;

	if (!GLOBAL_RCSCHEDULER->handelBuild(rcparams, &mesh))
	{
		DEBUG_LOG("%s", "navmesh build failed!");
	}
}

NavMeshParamsDlg::~NavMeshParamsDlg()
{
    delete ui;
}

void NavMeshParamsDlg::on_toolButton_clicked()
{
	MeshSelectDialog* meshselect = new MeshSelectDialog(this, m_meshTableModel, m_meshTableSelectModel);
	auto rnt = meshselect->exec();

	if (rnt == QDialog::Accepted)
	{
		ui->lineEdit->setText(m_meshTableModel->itemFromIndex(m_meshTableSelectModel->currentIndex())->text());
	}
}

void NavMeshParamsDlg::on_IsRenderHFStateChanged(int state)
{
	if (state == Qt::Checked)
	{
		GLOBAL_RCSCHEDULER->isRenderHeightField = true;
	}
	else
	{
		GLOBAL_RCSCHEDULER->isRenderHeightField = false;
	}
}

void NavMeshParamsDlg::on_IsRenderCTStateChanged(int state)
{
	if (state == Qt::Checked)
	{
		GLOBAL_RCSCHEDULER->isRenderContour = true;
	}
	else
	{
		GLOBAL_RCSCHEDULER->isRenderContour = false;
	}
}

void NavMeshParamsDlg::on_IsRenderDMStateChanged(int state)
{
	if (state == Qt::Checked)
	{
		GLOBAL_RCSCHEDULER->isRenderDetailMesh = true;
	}
	else
	{
		GLOBAL_RCSCHEDULER->isRenderDetailMesh = false;
	}
}

void NavMeshParamsDlg::on_IsRenderTContourChanged(int state)
{
	if (state == Qt::Checked)
	{
		GLOBAL_RCSCHEDULER->isRenderTContour = true;
	}
	else
	{
		GLOBAL_RCSCHEDULER->isRenderTContour = false;
	}
}

void NavMeshParamsDlg::on_IsRenderTCompactFieldChanged(int state)
{
	if (state == Qt::Checked)
	{
		GLOBAL_RCSCHEDULER->isRenderTCompactField = true;
	}
	else
	{
		GLOBAL_RCSCHEDULER->isRenderTCompactField = false;
	}
}
