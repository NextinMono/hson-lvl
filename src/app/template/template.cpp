#include "template.h"
#include "sq-funcs.h"
#include "sq-types.h"
#include <filesystem>

using namespace ulvl::app;
namespace fs = std::filesystem;

Template::~Template() {
	delete hsonTemplate;
	delete squirrelWrap;
}

Template::Template(const char* templateName) : name{ templateName } {
	fs::path temFolder = rel_to_exe( fs::path{ "templates" } / templateName );
	fs::path hsonFile = temFolder / "hson.json";
	hsonTemplate = new hl::set_object_type_database{ hsonFile.wstring().c_str() };
	objectTree.generateTree(hsonTemplate);

	reloadScripts();
}

void Template::reloadScripts() {
	if (squirrelWrap) delete squirrelWrap;
	squirrelWrap = new SquirrelWrap();
	squirrelWrap->init();
	registerFuncs(*squirrelWrap);
	registerTypes(*squirrelWrap);

	fs::path temFolder = rel_to_exe(fs::path{ "templates" } / name);
	fs::path objPath = temFolder / "src";
	if (fs::exists(objPath)) {
		for (auto& file : fs::directory_iterator{ objPath })
			if (file.path().extension() == ".nut")
				squirrelWrap->loadFile(file);
	}
}

ModelData Template::getModelData(ObjectService::Object* obj) {
	fs::path objPath = rel_to_exe( fs::path{ "templates" } / name / "src" );
	objPath /= { obj->hson->type + std::string{ ".nut" } };
	if (!fs::exists(objPath))
		return {};

	return squirrelWrap->callGetModelData(obj);
}

void Template::addDebugVisual(ObjectService::Object* obj) {
	fs::path objPath = rel_to_exe( fs::path{ "templates" } / name / "src" );
	objPath /= { obj->hson->type + std::string{ ".nut" } };
	if (!fs::exists(objPath))
		return;

	squirrelWrap->callAddDebugVisual(obj);
}

void Template::addDynamicDebugVisual(ObjectService::Object* obj) {
	fs::path objPath = rel_to_exe(fs::path{ "templates" } / name / "src");
	objPath /= { obj->hson->type + std::string{ ".nut" } };
	if (!fs::exists(objPath))
		return;

	squirrelWrap->callAddDynamicDebugVisual(obj);
}

void Template::dynamicDebugVisualEnd(ObjectService::Object* obj) {
	fs::path objPath = rel_to_exe(fs::path{ "templates" } / name / "src");
	objPath /= { obj->hson->type + std::string{ ".nut" } };
	if (!fs::exists(objPath))
		return;

	squirrelWrap->callDynamicDebugVisualEnd(obj);
}

bool Template::templateExists(const char* name) {
	if (name == nullptr || strcmp(name, "") == 0) return false;

	fs::path temPath = rel_to_exe( fs::path{ "templates" } / name );
	return fs::exists(temPath);
}

void Template::Tree::generateTree(hl::set_object_type_database* hsonTemplate) {
	if (!hsonTemplate) return;

	for (auto it = hsonTemplate->begin(); it != hsonTemplate->end(); ++it) {
		const char* key = it->first;
		hl::set_object_type& value = it->second;

		std::string path{ value.category + "/" + key };
		auto* node = createNode(path.c_str());
		node->objectType = hsonTemplate->get(it->first);
	}
}

Template::TreeNode* Template::Tree::getNode(const char* name, const std::vector<TreeNode*>& tree)
{
	TreeNode* node{ nullptr };

	for (auto* node : tree) {
		if (node->name == name)
			return node;

		if (node->type == Template::TreeNode::NodeType::OBJECT) continue;

		node = getNode(name, node->childNodes);
	}

	return node;
}

Template::TreeNode* Template::Tree::getNode(const char* name)
{
	return getNode(name, tree);
}

Template::TreeNode* Template::Tree::getNodeByPath(const char* path)
{
	std::stringstream ss{ path };
	std::string part;
	std::vector<std::string> result;

	while (std::getline(ss, part, '/'))
		result.push_back(part);

	TreeNode* lastNode{ nullptr };
	std::vector<TreeNode*>* lastTree{ &tree };
	int m{ 0 };
	for (auto& name : result) {
		if (name == "") continue;

		for (auto* node : *lastTree) {
			if (name == node->name) {
				if (result.size() - 1 == m)
					lastNode = node;
				if (node->type == TreeNode::NodeType::CATEGORY)
					lastTree = &node->childNodes;
				break;
			}
		}
		m++;
	}

	return lastNode;
}

Template::TreeNode* Template::Tree::createNode(const char* path) {
	std::stringstream ss{ path };
	std::string part;
	std::vector<std::string> result;

	while (std::getline(ss, part, '/'))
		result.push_back(part);

	TreeNode* lastNode{ nullptr };
	std::vector<TreeNode*>* lastTree{ &tree };
	std::string newPath{};
	for (size_t i = 0; i < result.size(); ++i) {
		auto& name{ result[i] };
		if (name.empty()) continue;

		TreeNode* node{ nullptr };
		for (auto* child : *lastTree) {
			if (child->name.c_str() && name == child->name) {
				node = child;
				break;
			}
		}

		if (!node) {
			node = new TreeNode{};
			node->name = name;
			node->type = (i == result.size() - 1) ? TreeNode::NodeType::OBJECT : TreeNode::NodeType::CATEGORY;
			lastTree->push_back(node);
		}

		lastNode = node;

		if (node->type == TreeNode::NodeType::CATEGORY)
			lastTree = &node->childNodes;
	}

	return lastNode;
}
