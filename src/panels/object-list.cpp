#include "object-list.h"
#include "../app/project-manager.h"
#include "../ui/editors/basic.h"

using namespace ulvl;

void RenderObject(app::ObjectService::Object* object) {
	auto* app = Application::instance;
	auto* objectService = app->getService<app::ObjectService>();
	auto* objectSelectionMgr = app->getService<app::ObjectSelectionManager>();
	ImGui::PushID(object);
	auto nameStr = object->getDisplayName();
	const char* objectName = nameStr.c_str();
	bool hasBeenDeleted{ false };

	ImGuiTreeNodeFlags treeFlags{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick };

	if (objectSelectionMgr->selected == object)
		treeFlags |= ImGuiTreeNodeFlags_Selected;

	bool isOpen{ false };
	if (!object->hasChildren())
		ImGui::TreeNodeEx(objectName, treeFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
	else
		isOpen = ImGui::TreeNodeEx(objectName, treeFlags);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
		objectSelectionMgr->select(object);

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("objServiceObject", &object, sizeof(app::ObjectService::Object*));
		ImGui::Text("%s", objectName);

		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		const ImGuiPayload* payload{ ImGui::AcceptDragDropPayload("objServiceObject") };
		if (payload && *(app::ObjectService::Object**)payload->Data) {
			auto* dropped = *(app::ObjectService::Object**)payload->Data;

			if (dropped->guid != object->hson->parentID) {
				bool containsChild{ false };

				for (auto* child : object->children)
					if (child == dropped) {
						containsChild = true;
						break;
					}

				auto* oldParent = dropped->getParent();

				if (containsChild)
					dropped->hson->parentID = object->hson->parentID;
				else
					dropped->hson->parentID = object->guid;

				object->updateChildren();
				if (object->hasParent())
					object->getParent()->updateChildren();
				if (oldParent)
					oldParent->updateChildren();
				dropped->updateModelMat();
				app->getService<app::ProjectManager>()->setUnsaved(true);
			}
		}

		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::Selectable("Remove")) {
			objectService->removeObject(object);
			hasBeenDeleted = true;
		}

		ImGui::EndPopup();
	}

	if (isOpen) {
		if (!hasBeenDeleted)
			for (auto* child : object->children)
				RenderObject(child);

		ImGui::TreePop();
	}

	ImGui::PopID();
}

void RenderLayer(app::ProjectManager::Layer * layer) {
	ImGui::PushID(layer);

	bool isOpen = ImGui::TreeNode(layer->hsonPath.stem().string().c_str());

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Save", "Ctrl+S"))
			layer->save();

		ImGui::EndPopup();
	}

	if (isOpen) {
		for (auto* object : layer->objects) {
			if (object->hasParent()) continue;

			RenderObject(object);
		}

		ImGui::TreePop();
	}

	ImGui::PopID();
}

void RenderProject(app::ProjectManager::Project* project) {
	ImGui::PushID(project);

	bool isOpen = ImGui::TreeNode(project->folderPath.stem().string().c_str());

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Save"))
			project->save();

		ImGui::EndPopup();
	}

	if (isOpen) {
		for (auto* layer : project->layers)
			RenderLayer(layer);

		ImGui::TreePop();
	}

	ImGui::PopID();
}

void ObjectList::RenderPanel() {
	auto* app = Application::instance;

	Editor("Include Projects", includeProjects);

	ImGui::Separator();

	auto* objectSelectionMgr = app->getService<app::ObjectSelectionManager>();
	if (auto* projectManager = app->getService<app::ProjectManager>()) {
		if (includeProjects)
			for (auto* project : projectManager->projects)
				RenderProject(project);
		else
			for (auto* project : projectManager->projects)
				for (auto* layer : project->layers)
					RenderLayer(layer);

		for (auto* layer : projectManager->layers)
			RenderLayer(layer);
	}
}
