#include "project-manager.h"
#include "../app.h"

using namespace ulvl::app;

ProjectManager::Project::Project(const std::filesystem::path& folderPath) : folderPath{ folderPath } {
	for (const auto& file : std::filesystem::directory_iterator{ folderPath })
		if (file.path().extension() == ".hson")
			loadLayer(file);
}

ProjectManager::Project::Project(const std::string& name) : folderPath{ name } { }

void ProjectManager::Project::loadLayer(const std::filesystem::path& hsonPath) {
	layers.push_back(new Layer{ hsonPath });
}

ProjectManager::Layer* ProjectManager::Project::getLayer(const hl::hson::project* project) {
	for (auto* layer : layers)
		if (layer->hson == project)
			return layer;

	return nullptr;
}

void ProjectManager::Project::save() {
	for (auto* layer : layers)
		layer->save();
}

void ProjectManager::Project::closeLayer(Layer* layer, bool removeFromVector) {
	if (removeFromVector)
		layers.erase(std::remove(layers.begin(), layers.end(), layer));

	auto* objServ = Application::instance->getService<ObjectService>();
	for (auto it = layer->objects.begin(); it != layer->objects.end(); it++) {
		auto* obj = (*it);
		if (obj->hasChildren())
			std::advance(it, obj->children.size());
		objServ->removeObject(obj, false);
	}
	layer->objects.clear();
	layer->hson->clear();
	delete layer->hson;
	delete layer;
}

void ProjectManager::Project::closeAll() {
	for (auto* layer : layers)
		closeLayer(layer, false);

	layers.clear();
}

void ProjectManager::AddCallback() {
	Application::instance->addListener(this);
}

void ProjectManager::loadProject(const std::filesystem::path& folderPath) {
	projects.push_back(new Project{ folderPath });
}

void ProjectManager::loadLayer(const std::filesystem::path& hsonPath) {
	layers.push_back(new Layer{ hsonPath });
}

void ProjectManager::addProject(const std::string& name) {
	projects.push_back(new Project{ name });
}

void ProjectManager::addLayer(const std::string& name) {
	layers.push_back(new Layer{ name });
}

ProjectManager::Layer* ProjectManager::getLayer(const hl::hson::project* project) {
	for (auto* proj : projects)
		if (auto* layer = proj->getLayer(project))
			return layer;

	return nullptr;
}

void ProjectManager::setUnsaved(bool unsaved) {
	if (this->unsaved == unsaved) return;

	this->unsaved = unsaved;
	gfx::Graphics::instance->setUnsaved(unsaved);
}

void ProjectManager::newProj() {
	addProject("new-project");
}

void ProjectManager::newLayer() {
	addLayer("new-layer");
}

void ProjectManager::saveAll() {
	for (auto* project : projects)
		project->save();
	for (auto* layer : layers)
		layer->save();
}

void ProjectManager::closeProject(Project* proj) {
	projects.erase(std::remove(projects.begin(), projects.end(), proj));

	proj->closeAll();

	delete proj;
}

void ProjectManager::closeLayer(Layer* layer) {
	layers.erase(std::remove(layers.begin(), layers.end(), layer));

	auto* objServ = Application::instance->getService<ObjectService>();
	for (auto it = layer->objects.begin(); it != layer->objects.end(); it++) {
		auto* obj = (*it);
		if (obj->hasChildren())
			std::advance(it, obj->children.size());
		objServ->removeObject(obj, false);
	}
	layer->objects.clear();
	layer->hson->clear();
	delete layer->hson;
	delete layer;
}

void ProjectManager::EventCallback(SDL_Event e) {
	const bool* keys = SDL_GetKeyboardState(NULL);

	if ((keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) && (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) && keys[SDL_SCANCODE_N]) newProj();
	else if ((keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) && keys[SDL_SCANCODE_N]) newLayer();
}

ProjectManager::Layer::Layer(const std::filesystem::path& hsonPath) : hsonPath{ hsonPath } {
	hson = new hl::hson::project{ hsonPath };
	auto* objectService = Application::instance->getService<ObjectService>();
	for (auto& object : hson->objects)
		objects.push_back(objectService->addObject(object.first, &object.second, hson));
	for (auto* object : objects)
		object->updateChildren();
}

ProjectManager::Layer::Layer(const std::string& name) : hsonPath{ name } {
	if (!hsonPath.has_extension())
		hsonPath += ".hson";

	hson = new hl::hson::project{};
}

void ProjectManager::Layer::save() {
	Application::instance->getService<ProjectManager>()->setUnsaved(false);
	hson->save(hsonPath);
}
