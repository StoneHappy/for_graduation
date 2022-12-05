#include "RCScheduler.h"

namespace GU
{
	void RCScheduler::handelConfig(rcConfig rcconfig, std::shared_ptr<NavMesh> p_nvmesh)
	{
		rc_cfg = rcconfig;
		navmesh = p_nvmesh;
	}
	void RCScheduler::handelBuild()
	{

	}
}