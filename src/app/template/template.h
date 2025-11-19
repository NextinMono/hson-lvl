#pragma once
#include <hedgelib/sets/hl_set_obj_type.h>
#include "squirrelwrap.h"

namespace ulvl::app {
	struct Template {
		struct TreeNode {
			enum class NodeType {
				CATEGORY,
				OBJECT
			};

			std::string name;
			NodeType type;
			hl::set_object_type* objectType;
			std::vector<TreeNode*> childNodes;

			~TreeNode() { for (auto* child : childNodes) delete child; }
		};

		struct Tree {
			std::vector<TreeNode*> tree;

			~Tree() { for (auto* node : tree) delete node; }
			void generateTree(hl::set_object_type_database* hsonTemplate);
			TreeNode* getNode(const char* name, const std::vector<TreeNode*>& tree);
			TreeNode* getNode(const char* name);
			TreeNode* getNodeByPath(const char* path);
			TreeNode* createNode(const char* path);
		};

		const char* name;
		hl::set_object_type_database* hsonTemplate;
		Tree objectTree{};
		SquirrelWrap squirrelWrap{};

		~Template();
		Template(const char* templateName);
		ModelData getModelData(ObjectService::Object* obj);
		void addDebugVisual(ObjectService::Object* obj);
		void addDynamicDebugVisual(ObjectService::Object* obj);
		void dynamicDebugVisualEnd(ObjectService::Object* obj);

		static bool templateExists(const char* name);
	};
}
