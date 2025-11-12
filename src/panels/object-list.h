#pragma once
#include "../ui/panel.h"
#include "../app.h"
#include "../app/project-manager.h"
#include "../app/object-selection-manager.h"

namespace ulvl {
	class ObjectList : public Panel {
	public:
		virtual Properties GetProperties() {
			return { .name = "Object List", .position = { 150, 100 }, .size = { 100, 350 }, .pivot{ 0, 0 } };
		}

		bool includeProjects{ true };

		virtual void RenderPanel() override;
	};
}
