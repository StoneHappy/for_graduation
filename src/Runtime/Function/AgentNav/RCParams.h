#pragma once

namespace GU
{
	struct RCParams
	{
		float m_cellSize;
		float m_cellHeight;
		float m_agentHeight;
		float m_agentRadius;
		float m_agentMaxClimb;
		float m_agentMaxSlope;
		float m_regionMinSize;
		float m_regionMergeSize;
		float m_edgeMaxLen;
		float m_edgeMaxError;
		float m_vertsPerPoly;
		float m_detailSampleDist;
		float m_detailSampleMaxError;
		int		m_partitionType;
		bool	m_filterLowHangingObstacles;
		bool	m_filterLedgeSpans;
		bool	m_filterWalkableLowHeightSpans;
		bool m_keepInterResults;
	};
	const int MAX_AGENTS = 128;
	const int MAX_SMOOTH = 2048;
	const int MAX_POLYS = 256;
}