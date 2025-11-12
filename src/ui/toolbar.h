#pragma once
#include <imgui.h>
#include "../app.h"
#include "../app/object-selection-manager.h"
#include "../app/project-manager.h"

namespace ulvl {
	void ToolBar() {
		auto* app = Application::instance;
		auto* projectMgr = app->getService<app::ProjectManager>();
		auto* objSelectMgr = app->getService<app::ObjectSelectionManager>();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::BeginMenu("New")) {
					if (ImGui::MenuItem("Layer", "Ctrl+N")) projectMgr->newLayer();
					if (ImGui::MenuItem("Project", "Ctrl+Shift+N")) projectMgr->newProj();
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {}
				if (ImGui::MenuItem("Save", "Ctrl+S")) objSelectMgr->save();
				if (ImGui::MenuItem("Save All", "Ctrl+Shift+S")) projectMgr->saveAll();

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}
