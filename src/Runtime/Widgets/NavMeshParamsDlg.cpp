#include "NavMeshParamsDlg.h"
#include "ui_NavMeshParamsDlg.h"

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
	memset(&rc_cfg, 0, sizeof(rc_cfg));
	rc_cfg.cs = ui->p_cs->value();
	rc_cfg.ch = ui->p_ch->value();
	rc_cfg.walkableSlopeAngle = ui->p_agentMaxSlope->value();
	rc_cfg.walkableHeight = (int)ceilf(ui->p_agentHeight->value() / rc_cfg.ch);
	rc_cfg.walkableClimb = (int)floorf(ui->p_agentMaxClimb->value() / rc_cfg.ch);
	rc_cfg.walkableRadius = (int)ceilf(ui->p_agentRadius->value() / rc_cfg.cs);
	rc_cfg.maxEdgeLen = (int)(ui->p_edgeMaxLen->value() / ui->p_cs->value());
	rc_cfg.maxSimplificationError = ui->p_edgeMaxError->value();
	rc_cfg.minRegionArea = (int)rcSqr(ui->p_regionMinSize->value());		// Note: area = size*size
	rc_cfg.mergeRegionArea = (int)rcSqr(ui->p_regionMergeSize->value());	// Note: area = size*size
	rc_cfg.maxVertsPerPoly = (int)ui->p_vertsPerPoly->value();
	rc_cfg.detailSampleDist = ui->p_detailSampleDist->value() < 0.9f ? 0 : ui->p_cs->value() * ui->p_detailSampleDist->value();
	rc_cfg.detailSampleMaxError = ui->p_ch->value() * ui->p_detailSampleMaxError->value();
}

NavMeshParamsDlg::~NavMeshParamsDlg()
{
    delete ui;
}
