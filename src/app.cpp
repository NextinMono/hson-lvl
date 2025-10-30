#include "app.h"
#include "panels/object-inspector.h"
#include "app/camera-service.h"
#include "app/object-service.h"
#include "app/project-manager.h"
#include <imgui_impl_sdl3.h>

using namespace ulvl;

Application* Application::instance = nullptr;

void Application::init() {
	graphics = new gfx::Graphics;
	graphics->init();

    instance = this;

    addPanel<ObjectInspector>();

    addService<app::CameraService>();
    addService<app::ObjectService>();
    addService<app::ProjectManager>();
    getService<app::ProjectManager>()->loadProject("E:\\Steam\\steamapps\\common\\SonicFrontiers\\image\\x64\\raw\\gedit\\w6d01_gedit\\w6d01_obj_area01.hson");
}

void Application::loop()
{
    while (isRunning) {
        frameTimer.update();
        update();
        graphics->renderBegin();
        render();
        for (auto* listener : appListeners)
            listener->Render();
        graphics->renderEnd();
    }
}

Application::~Application()
{
    graphics->shutdown();
    delete graphics;
}

void Application::render()
{
    for (auto* panel : panels)
        panel->Render();
}

void Application::update() {
    for (auto* service : services)
        service->Update();

    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL3_ProcessEvent(&e);

        for (auto* listener : appListeners)
            listener->EventCallback(e);

        switch (e.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            isRunning = false;
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            graphics->resize(e.window.data1, e.window.data2);
            break;
        }
    }
}

void Application::FrameTimer::update() {
    auto now = clock::now();
    std::chrono::duration<float> delta = now - lastTime;
    deltaTime = delta.count();
    fps = 1.0f / deltaTime;
    lastTime = now;
}
