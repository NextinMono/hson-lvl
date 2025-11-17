#include "object-service.h"
#include "template-manager.h"
#include "project-manager.h"
#include "../app.h"
#include "../graphics/graphics.h"
#include "../math/mat.h"
#include "../utilities/hson.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "object-selection-manager.h"
#include "cleaner-service.h"
#include "debug-visual-service.h"

using namespace ulvl::app;

ObjectService::Object::Object(const hl::guid& guid, hl::hson::object* object, hl::hson::project* proj) : guid{ guid }, hson{ object }, owner{ proj } {
	model = new gfx::Model{};

    updateModel();
    updateModelMat();
    updateDebugVisual();

    gfx::Graphics::instance->models.push_back(model);
}

ObjectService::Object::~Object() {
    auto* app = Application::instance;
    auto* objService = app->getService<ObjectService>();

    for (auto* child : children)
        objService->removeObject(child);

    Application::instance->getService<CleanerService>()->deleteModel(model);
}

void ObjectService::Object::setPosition(const glm::vec3& pos) {
    hson->position = { -pos.x, pos.y, pos.z };

    model->setPosition(pos);

    for (auto* child : children) child->updateModelMat();
}

void ObjectService::Object::setLocalPosition(const glm::vec3& pos) {
    hson->position = { -pos.x, pos.y, pos.z };

    if (hasParent()) {
        model->setPosition(getWorldMatrix()[3]);
    }
    else
        model->setPosition(pos);

    for (auto* child : children) child->updateModelMat();
}

void ObjectService::Object::setLocalRotation(const glm::vec3& rot) {
    auto quat = glm::quat{ rot };
    hson->rotation = { -quat.x, -quat.y, -quat.z, quat.w };

    if (hasParent()) {
        auto worldMatrix = getWorldMatrix();
        model->setRotation(glm::quat_cast(worldMatrix));
    }
    else
        model->setRotation(rot);

    for (auto* child : children) child->updateModelMat();
}

void ObjectService::Object::setLocalQuaternion(const glm::quat& rot) {
    hson->rotation = { -rot.x, -rot.y, -rot.z, rot.w };

    if (hasParent()) {
        auto worldMatrix = getWorldMatrix();
        model->setRotation(glm::quat_cast(worldMatrix));
    }
    else
        model->setRotation(rot);

    for (auto* child : children) child->updateModelMat();
}

void ObjectService::Object::setRotation(const glm::vec3& rot) {
    auto quat = glm::quat{ rot };
    hson->rotation = { -quat.x, -quat.y, -quat.z, quat.w };

    model->setRotation(rot);

    for (auto* child : children) child->updateModelMat();
}

void ObjectService::Object::setQuaternion(const glm::quat& rot) {
    hson->rotation = { -rot.x, -rot.y, -rot.z, rot.w };
    model->setRotation(rot);

    for (auto* child : children) child->updateModelMat();
}

glm::vec3 ObjectService::Object::getLocalPosition() const {
    if (!hson->position.has_value()) return glm::vec3{ 0, 0, 0 };

    auto& hsonPos = hson->position.value();
    return glm::vec3{ -hsonPos.x, hsonPos.y, hsonPos.z };
}

glm::vec3 ObjectService::Object::getLocalRotation() const {
    if (!hson->rotation.has_value()) return glm::vec3{ 0, 0, 0 };

    auto& hsonRot = hson->rotation.value();
    auto quat = glm::quat{ hsonRot.w, -hsonRot.x, -hsonRot.y, -hsonRot.z };
    return glm::eulerAngles(quat);
}

glm::quat ObjectService::Object::getLocalQuaternion() const {
    if (!hson->rotation.has_value()) return glm::quat{ 1, 0, 0, 0 };

    auto& hsonRot = hson->rotation.value();
    return glm::quat{ hsonRot.w, -hsonRot.x, -hsonRot.y, -hsonRot.z };
}

glm::vec3 ObjectService::Object::getPosition() const {
    auto worldMat = getWorldMatrix();
    return worldMat[3];
}

glm::vec3 ObjectService::Object::getRotation() const {
    return glm::eulerAngles(getQuaternion());
}

glm::quat ObjectService::Object::getQuaternion() const {
    auto worldMat = getWorldMatrix();
    return glm::quat_cast(worldMat);
}

glm::mat4 ObjectService::Object::getWorldMatrix() const {
    if (hasParent())
        return getParent()->getWorldMatrix() * getLocalMatrix();
    else
        return getLocalMatrix();
}

glm::mat4 ObjectService::Object::getLocalMatrix() const {
    return glm::translate(glm::mat4{ 1 }, getLocalPosition()) * glm::toMat4(getLocalQuaternion());
}

ObjectService::Object* ObjectService::Object::getParent() const {
    if (hson->parentID.has_value())
        return Application::instance->getService<ObjectService>()->getObject(hson->parentID.value());

    return nullptr;
}

bool ObjectService::Object::hasParent() const {
    return hson->parentID.has_value() && getParent();
}

bool ObjectService::Object::hasChildren() const {
    return children.size() != 0;
}

void ObjectService::Object::updateChildren() {
    children.clear();

    auto* objService = Application::instance->getService<ObjectService>();

    for (auto* obj : objService->objects)
        if (obj->getParent() == this)
            children.push_back(obj);
}

void ObjectService::Object::updateModel() {

    ModelData modelData = Application::instance->getService<TemplateManager>()->currentTemplate->getModelData(this);
    if (modelData.vertices != nullptr) {
        model->clearMeshes();
        model->addMesh(modelData.vertices, modelData.vertexCount, modelData.indices, modelData.indexCount, nullptr);
        delete modelData.vertices;
        delete modelData.indices;
    }
    else if (model->indices.size() == 0) {
        gfx::BaseVertex vertices[]{
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}}
        };

        unsigned short indices[]{
            0, 1, 2,
            2, 3, 0
        };

        model->addMesh(vertices, ARRAY_SIZE(vertices), indices, ARRAY_SIZE(indices), nullptr);
    }
}

void ObjectService::Object::updateModelMat() {
    model->setWorldMatrix(getWorldMatrix());
    for (auto* child : children) child->updateModelMat();
}

void ObjectService::Object::updateDebugVisual() {
    auto* tem = Application::instance->getService<TemplateManager>()->currentTemplate;
    auto* debugVisual = Application::instance->getService<DebugVisualService>();

    debugVisual->removeMeshes((int)this);
    tem->addDebugVisual(this);
}

ObjectService::Object* ObjectService::addObject(const hl::guid& guid, hl::hson::object* hson, hl::hson::project* proj) {
    auto* object = new Object{ guid, hson, proj };
    objects.push_back(object);
    return object;
}

void ObjectService::removeObject(Object* object, bool removeFromLayer) {
    auto* app = Application::instance;
    
    auto* objectSelectMgr = app->getService<ObjectSelectionManager>();
    if (objectSelectMgr->selected == object)
        objectSelectMgr->deselect();

    if (auto* parent = object->getParent()) {
        object->hson->parentID = hl::guid{ nullptr };
        parent->updateChildren();
    }

    if (object->owner) {
        hl::ordered_map<hl::guid, hl::hson::object>& objs{ object->owner->objects };
        int idx{ 0 };
        for (auto it = objs.begin(); it != objs.end(); ++it) {
            if (it->first == object->guid)
                break;
            idx++;
        }

        objs.erase(objs.begin() + idx);

        if (removeFromLayer) {
            auto* projMgr = app->getService<ProjectManager>();

            if (auto* layer = projMgr->getLayer(object->owner))
                layer->objects.erase(std::remove(layer->objects.begin(), layer->objects.end(), object), layer->objects.end());
        }
    }

    objects.erase(std::remove(objects.begin(), objects.end(), object));

    delete object;
}

void ObjectService::removeObject(const hl::guid& guid, bool removeFromLayer) {
    removeObject(getObject(guid), removeFromLayer);
}

ObjectService::Object* ObjectService::getObject(const hl::guid& guid) {
    for (auto* object : objects)
        if (object->guid == guid)
            return object;

    return nullptr;
}

ObjectService::Object* ObjectService::createObject(hl::hson::project* proj) {
    auto* hsonPair = proj->objects.emplace(hl::guid::random(), hl::hson::object{}).first;
    auto* obj = new Object{ hsonPair->first, proj->objects.get(hsonPair->first), proj };
    objects.push_back(obj);

    auto* app = ulvl::Application::instance;
    auto* projMgr = app->getService<ProjectManager>();

    if (auto* layer = projMgr->getLayer(proj))
        layer->objects.push_back(obj);

    return obj;
}

ObjectService::Object* ObjectService::createObject(hl::hson::project* proj, std::string& typeName, hl::set_object_type* type) {
    auto* app = ulvl::Application::instance;
    auto* tem = app->getService<TemplateManager>()->currentTemplate->hsonTemplate;

    auto* obj = createObject(proj);
    auto* hson = obj->hson;

    hson->type = typeName;
    hson->parameters = ut::createStruct(tem->structs[type->structType]);

    return obj;
}

ObjectService::Object* ObjectService::createInstanceOf(hl::hson::project* proj, const hl::guid& baseObj) {
    auto* obj = createObject(proj);
    obj->hson->instanceOf = baseObj;
    return obj;
}
