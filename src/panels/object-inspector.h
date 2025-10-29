#pragma once
#include "../ui/panel.h"
#include "../app.h"

namespace ulvl {
	class ObjectInspector : public Panel {
	public:
		virtual Properties GetProperties() {
			return { .name = "Object Inspector", .position = { 150, 100 }, .size = { 100, 350 }, .pivot{ 0, 0 } };
		}

		virtual void RenderPanel() {
			auto* app = Application::instance;

			ImGui::Text("imgui sega balls!");
		}
	};
}
