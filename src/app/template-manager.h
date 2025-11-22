#pragma once
#include "service.h"
#include "template/template.h"

namespace ulvl::app {
	class TemplateManager : public Service {
	public:
		Template* currentTemplate;
		std::vector<Template*> templates;

		virtual void AddCallback() override;
		void setTemplate(const char* templateName);
		inline void setTemplate(std::string& templateName) { setTemplate(templateName.c_str()); }
		void setTemplate(Template* temp);
		void loadTemplate(const char* templateName);
		void reloadAll();
		void deloadTemplates();
		inline void loadTemplate(std::string& templateName) { loadTemplate(templateName.c_str()); }
	};
}
