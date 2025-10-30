#include "camera-service.h"
#include "../graphics/graphics.h"
#include "../app.h"
#include <imgui.h>

using namespace ulvl::app;
using namespace ulvl::gfx;

void CameraService::AddCallback() {
    Application::instance->addListener(this);
}

void CameraService::Update() {
    ImGuiIO& io{ ImGui::GetIO() };
    if (io.WantCaptureKeyboard) return;

    auto* graphics = Graphics::instance;
    auto* camera = graphics->camera;
    auto& frameTimer = Application::instance->frameTimer;
    const float cameraSpeed = 2.5f * frameTimer.deltaTime;

    const bool* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_W]) camera->addPosition(cameraSpeed * camera->front());
    if (state[SDL_SCANCODE_S]) camera->addPosition(cameraSpeed * -camera->front());
    if (state[SDL_SCANCODE_D]) camera->addPosition(cameraSpeed * camera->right());
    if (state[SDL_SCANCODE_A]) camera->addPosition(cameraSpeed * -camera->right());
    if (state[SDL_SCANCODE_SPACE]) camera->addPosition(cameraSpeed * glm::vec3{ 0,1,0 });
    if (state[SDL_SCANCODE_LCTRL]) camera->addPosition(cameraSpeed * glm::vec3{ 0,-1,0 });

    if (io.WantCaptureMouse) return;

    // Mouse capture
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RIGHT)
        SDL_SetWindowRelativeMouseMode(graphics->window, true);
    else
        SDL_SetWindowRelativeMouseMode(graphics->window, false);
}

void CameraService::EventCallback(SDL_Event e) {
    ImGuiIO& io{ ImGui::GetIO() };
    static double lastX = 400.0, lastY = 300.0;
    static bool firstMouse = true;
    auto* graphics = Graphics::instance;

    switch (e.type) {
    case SDL_EVENT_MOUSE_MOTION:
        if (io.WantCaptureMouse) break;
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RIGHT)
        {
            double xpos = e.motion.x;
            double ypos = e.motion.y;

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = static_cast<float>(xpos - lastX);
            float yoffset = -static_cast<float>(ypos - lastY);
            lastX = xpos;
            lastY = ypos;

            const float sensitivity = 0.01f;
            graphics->camera->addRotation(glm::vec3{
                yoffset * sensitivity,
                xoffset * sensitivity,
                0.0f
                });
        }
        break;

    case SDL_EVENT_MOUSE_WHEEL:
        if (io.WantCaptureMouse) break;
        graphics->camera->setFov(graphics->camera->fov - static_cast<float>(e.wheel.y));
        if (graphics->camera->fov < 1.0f) graphics->camera->setFov(1.0f);
        if (graphics->camera->fov > 90.0f) graphics->camera->setFov(90.0f);
        break;
    }
}
