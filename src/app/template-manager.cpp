#include "template-manager.h"
#include "settings-manager.h"
#include "../app.h"

using namespace ulvl::app;

void TemplateManager::AddCallback() {
	loadTemplate(Application::instance->getService<app::SettingsManager>()->settings.selectedTemplateName);
}

void TemplateManager::setTemplate(const char* templateName) {
	for (auto* temp : templates)
		if (strcmp(temp->name, templateName) == 0) {
			setTemplate(temp);
			return;
		}
}

void TemplateManager::setTemplate(Template* temp) {
	currentTemplate = temp;
}

void TemplateManager::loadTemplate(const char* templateName) {
	if (!Template::templateExists(templateName)) return;

	auto* temp = new Template{ templateName };
	templates.push_back(temp);
	if (currentTemplate == nullptr)
		setTemplate(temp);
}

void TemplateManager::reloadAll() {
	for (auto* temp : templates)
		temp->reloadScripts();
}

void TemplateManager::deloadTemplates() {
	for (auto* temp : templates)
		delete temp;

	templates.clear();
}
