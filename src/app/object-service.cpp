#include "object-service.h"
#include "../graphics/graphics.h"

using namespace ulvl::app;

ObjectService::Object::Object(hl::hson::object* object) : hson{ object } {
	model = new gfx::Model{};

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

    auto& hsonPos = hson->position.value();
    auto glmPos = glm::vec3{ hsonPos.x, hsonPos.y, hsonPos.z };

    model->setPosition(glmPos);

    gfx::Graphics::instance->models.push_back(model);
}

ObjectService::Object* ObjectService::addObject(hl::hson::object* hson) {
    auto* object = new Object{ hson };
    objects.push_back(object);
    return object;
}
