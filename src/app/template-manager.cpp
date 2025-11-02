#include "template-manager.h"
#include <filesystem>

using namespace ulvl::app;
namespace fs = std::filesystem;

TemplateManager::Template::Template(const char* templateName) : name{ templateName } {
	fs::path hsonFile = fs::path{ "templates" } / templateName / "hson.json";
	hsonTemplate = new hl::set_object_type_database{ hsonFile.wstring().c_str() };
	generateTree();
}

void TemplateManager::Template::generateTree() {
	if (!hsonTemplate) return;

	for (auto it = hsonTemplate->begin(); it != hsonTemplate->end(); ++it) {
		const char* key = it->first;
		hl::set_object_type& value = it->second;

		std::string path{ value.category + "/" + key };
		createNode(path.c_str())->objectType = &value;
	}
}

TemplateManager::Template::TreeNode* TemplateManager::Template::getNode(const char* name, const std::vector<TreeNode*>& tree)
{
	TreeNode* node{ nullptr };

	for (auto* node : tree) {
		if (strcmp(node->name, name) == 0)
			return node;

		if (node->type == TemplateManager::Template::TreeNode::NodeType::OBJECT) continue;

		node = getNode(name, node->childNodes);
	}

	return node;
}

TemplateManager::Template::TreeNode* TemplateManager::Template::getNode(const char* name)
{
	return getNode(name, tree);
}

TemplateManager::Template::TreeNode* ulvl::app::TemplateManager::Template::getNodeByPath(const char* path)
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

TemplateManager::Template::TreeNode* TemplateManager::Template::createNode(const char* path) {
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
			if (child->name && name == child->name) {
				node = child;
				break;
			}
		}

		if (!node) {
			node = new TreeNode{};
			node->name = _strdup(name.c_str());
			node->type = (i == result.size() - 1) ? TreeNode::NodeType::OBJECT : TreeNode::NodeType::CATEGORY;
			lastTree->push_back(node);
		}

		lastNode = node;

		if (node->type == TreeNode::NodeType::CATEGORY)
			lastTree = &node->childNodes;
	}

	return lastNode;
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
	auto* temp = new Template{ templateName };
	templates.push_back(temp);
	if (currentTemplate == nullptr)
		setTemplate(temp);
}
