#include "camera-service.h"
#include "../graphics/graphics.h"
#include "../app.h"
#include "../panels/viewport.h"
#include <imgui.h>
#include <ImGuizmo.h>

using namespace ulvl::app;
using namespace ulvl::gfx;

void CameraService::AddCallback() {
    Application::instance->addListener(this);
}

static bool mouseAlreadyHeld{ false };
void CameraService::Update() {
    ImGuiIO& io{ ImGui::GetIO() };
    auto* graphics = Graphics::instance;
    auto* camera = graphics->camera;
    auto& frameTimer = Application::instance->frameTimer;

    float dt = frameTimer.deltaTime;
    float speed = cameraSpeed * dt;

    bool viewportHovered = Application::instance->getPanel<Viewport>()->isHovered;
    bool gizmoActive = ImGuizmo::IsUsing();

    if (!viewportHovered || gizmoActive) {
        mouseAlreadyHeld = false;
        SDL_SetWindowRelativeMouseMode(graphics->window, false);
        return;
    }

    const bool* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_W]) camera->addPosition(speed * camera->front());
    if (keys[SDL_SCANCODE_S]) camera->addPosition(speed * -camera->front());
    if (keys[SDL_SCANCODE_D]) camera->addPosition(speed * camera->right());
    if (keys[SDL_SCANCODE_A]) camera->addPosition(speed * -camera->right());
    if (keys[SDL_SCANCODE_SPACE]) camera->addPosition(speed * glm::vec3(0, 1, 0));
    if (viewportHovered && keys[SDL_SCANCODE_LCTRL]) camera->addPosition(speed * glm::vec3(0, -1, 0));

    bool rightMouseHeld = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);

    if (rightMouseHeld) {
        mouseAlreadyHeld = true;
        SDL_SetWindowRelativeMouseMode(graphics->window, true);
    }
    else {
        mouseAlreadyHeld = false;
        SDL_SetWindowRelativeMouseMode(graphics->window, false);
    }
}

void CameraService::EventCallback(SDL_Event e) {
    ImGuiIO& io{ ImGui::GetIO() };
    auto* graphics = Graphics::instance;
    bool viewportHovered = Application::instance->getPanel<Viewport>()->isHovered;
    bool gizmoActive = ImGuizmo::IsUsing();
    bool rightMouseHeld = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);

    switch (e.type) {
        case SDL_EVENT_MOUSE_MOTION: {
            if (!viewportHovered || gizmoActive) {
                mouseAlreadyHeld = false;
                break;
            }

            if (rightMouseHeld) {
                mouseAlreadyHeld = true;

                float xoffset = (float)e.motion.xrel;
                float yoffset = (float)-e.motion.yrel;

                const float sensitivity = 0.01f;
                graphics->camera->addRotation(glm::vec3{
                    yoffset * sensitivity,
                    xoffset * sensitivity,
                    0.0f
                    });
            }
            else mouseAlreadyHeld = false;

            break;
        }

        case SDL_EVENT_MOUSE_WHEEL: {
            if (!viewportHovered || gizmoActive) break;

            this->cameraSpeed += (float)e.wheel.y;
            this->cameraSpeed = std::clamp(this->cameraSpeed, 1.0f, 60.0f);
            break;
        }
    }
}
