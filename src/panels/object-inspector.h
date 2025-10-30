#pragma once
#include "../ui/panel.h"
#include "../app.h"
#include "../app/project-manager.h"

namespace ulvl {
	class ObjectInspector : public Panel {
	public:
		virtual Properties GetProperties() {
			return { .name = "Object Inspector", .position = { 150, 100 }, .size = { 100, 350 }, .pivot{ 0, 0 } };
		}

		virtual void RenderPanel() {
			auto* app = Application::instance;

			ImGui::Text("imgui sega balls!");

			if (auto* projectManager = app->getService<app::ProjectManager>()) {
				for (auto* project : projectManager->projects) {
					if (ImGui::TreeNode(project->hsonPath.filename().string().c_str())) {

						for (auto* object : project->objects) {
							ImGui::Selectable(object->hson->name.value().c_str());
						}

						ImGui::TreePop();
					}
				}
			}
		}
	};
}
