#pragma once
#include "service.h"
#include "../graphics/instancedmodel.h"

namespace ulvl::app {
	class DebugVisualService : public Service {
	public:
		gfx::InstancedModel* cube;
		gfx::InstancedModel* sphere;
		gfx::InstancedModel* cylinder;

		virtual ~DebugVisualService() override;
		void initCube();
		void initSphere();
		void initCylinder();
		virtual void AddCallback() override;
		void addCube(gfx::InstancedMesh mesh);
		void addSphere(gfx::InstancedMesh mesh);
		void addCylinder(gfx::InstancedMesh mesh);
		void removeMeshes(int id);
	};
}
