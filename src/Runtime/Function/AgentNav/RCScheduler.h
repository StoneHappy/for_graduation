#pragma once
#include <Recast.h>
#include <memory>
namespace GU
{
	class NavMesh;
	class RCScheduler
	{
	public:
		RCScheduler() = default;
		~RCScheduler() = default;

		void handelConfig(rcConfig rcconfig, std::shared_ptr<NavMesh>);

		void handelBuild();
	private:
		rcConfig rc_cfg;
		std::shared_ptr<NavMesh> navmesh;
	};
}