#pragma once
#include <imgui.h>
#include "../app.h"
#include "../app/object-selection-manager.h"
#include "../app/project-manager.h"
#include "../app/file-dialog-service.h"
#include "../panels/settings.h"

namespace ulvl {
	void ToolBar() {
		auto* app = Application::instance;
		auto* projectMgr = app->getService<app::ProjectManager>();
		auto* objSelectMgr = app->getService<app::ObjectSelectionManager>();
		auto* fileDialogServ = app->getService<app::FileDialogService>();
		auto* templateMgr = app->getService<app::TemplateManager>();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::BeginMenu("New")) {
					if (ImGui::MenuItem("Layer", "Ctrl+N", nullptr, projectMgr->canNew())) projectMgr->newLayer();
					if (ImGui::MenuItem("Project", "Ctrl+Shift+N", nullptr, projectMgr->canNew())) projectMgr->newProj();
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Open", "Ctrl+O", nullptr, fileDialogServ->canOpen())) fileDialogServ->open();
				if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, objSelectMgr->canSave())) objSelectMgr->save();
				if (ImGui::MenuItem("Save All", "Ctrl+Shift+S", nullptr, projectMgr->canSaveAll())) projectMgr->saveAll();

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Settings")) {
				app->addPanel<SettingsPanel>();
			}

			if (ImGui::MenuItem("Reload Scripts", nullptr, objSelectMgr->canSave())) templateMgr->reloadAll();
			ImGui::EndMainMenuBar();
		}
	}
}
