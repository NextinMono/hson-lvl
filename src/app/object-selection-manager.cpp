#include "object-selection-manager.h"
#include "../app.h"
#include "../graphics/graphics.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include "project-manager.h"

using namespace ulvl::app;

void ObjectSelectionManager::updateMatrix() {
	objectMatrix = selected->getWorldMatrix();
}

void ObjectSelectionManager::updateObject() {
	Application::instance->getService<app::ProjectManager>()->setUnsaved(true);

	if (selected->hasParent()) {
		glm::mat4 parentWorld = selected->getParent()->getWorldMatrix();
		glm::mat4 newLocal = glm::inverse(parentWorld) * objectMatrix;
		selected->setLocalPosition({ newLocal[3] });
		selected->setLocalQuaternion({ glm::quat_cast(newLocal) });
	}
	else {
		selected->setLocalPosition({ objectMatrix[3] });
		selected->setLocalQuaternion({ glm::quat_cast(objectMatrix) });
	}
	shouldUpdateDebug = true;
}

void ObjectSelectionManager::AddCallback() {
	Application::instance->addListener(this);
}

void ObjectSelectionManager::select(ObjectService::Object* object) {
	if (selected) {
		selected->removeDynamicVisual();
		selected->dynamicVisualReset();
	}
	selected = object;
	updateMatrix();
}

void ObjectSelectionManager::deselect() {
	selected = nullptr;
}

void ObjectSelectionManager::renderGizmo() {
	if (!selected) return;
	auto* graphics = gfx::Graphics::instance;
	auto* camera = graphics->camera;

	auto viewMatrix = camera->viewMatrix();
	auto projMatrix = camera->projMatrix();

	projMatrix[1][1] *= -1.0f;

	updateMatrix();
	if (ImGuizmo::Manipulate((const float*)&viewMatrix, (const float*)&projMatrix, ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE::WORLD, (float*)&objectMatrix))
		updateObject();
}

void ObjectSelectionManager::save() {
	if (!canSave()) return;

	auto* app = Application::instance;
	auto* projectMgr = app->getService<app::ProjectManager>();
	auto* objSelectMgr = app->getService<app::ObjectSelectionManager>();

	if (auto* selected = objSelectMgr->selected)
		if (auto* layer = projectMgr->getLayer(selected->owner))
			layer->save();
}

void ObjectSelectionManager::PreRender() {
	if (selected) {
		if (shouldUpdateDebug) {
			shouldUpdateDebug = false;
			selected->updateDebugVisual();
		}
		selected->removeDynamicVisual();
		selected->addDynamicVisual();
	}
}

void ObjectSelectionManager::EventCallback(SDL_Event e) {
	const bool* keys = SDL_GetKeyboardState(NULL);

	if ((keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) && keys[SDL_SCANCODE_S]) save();
}
