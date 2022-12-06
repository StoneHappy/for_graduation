#include "NavMeshParamsDlg.h"
#include "ui_NavMeshParamsDlg.h"
#include <Function/AgentNav/RCScheduler.h>
NavMeshParamsDlg::NavMeshParamsDlg(QWidget *parent) :
    QDialog(parent), 
    ui(new Ui::NavMeshParamsDlg)
{
    ui->setupUi(this);
    connect(ui->pushButtonOK, SIGNAL(clicked()), this, SLOT(slot_accept()));
}

void NavMeshParamsDlg::slot_accept()
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
}

NavMeshParamsDlg::~NavMeshParamsDlg()
{
    delete ui;
}
