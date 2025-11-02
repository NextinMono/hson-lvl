#pragma once
#include "service.h"
#include <hedgelib/hh/hl_hh_gedit.h>
#include <hedgelib/sets/hl_set_obj_type.h>

namespace ulvl::app {
	class TemplateManager : public Service {
	public:
		struct Template {
			struct TreeNode {
				enum class NodeType {
					CATEGORY,
					OBJECT
				};

				const char* name;
				NodeType type;
				hl::set_object_type* objectType;
				std::vector<TreeNode*> childNodes;

				~TreeNode() { delete name; }
			};

			const char* name;
			hl::set_object_type_database* hsonTemplate;
			std::vector<TreeNode*> tree;

			Template(const char* templateName);
			void generateTree();
			TreeNode* getNode(const char* name, const std::vector<TreeNode*>& tree);
			TreeNode* getNode(const char* name);
			TreeNode* getNodeByPath(const char* path);
			TreeNode* createNode(const char* path);
		};

		Template* currentTemplate;
		std::vector<Template*> templates;

		void setTemplate(const char* templateName);
		void setTemplate(Template* temp);
		void loadTemplate(const char* templateName);
	};
}
