#include "graphics.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include <imgui_impl_plume.h>

#include <ImGuizmo.h>

using namespace ulvl::gfx;

Graphics* Graphics::instance = nullptr;

namespace plume {
#ifdef WIN32
    extern std::unique_ptr<RenderInterface> CreateVulkanInterface();
#else
    extern std::unique_ptr<RenderInterface> CreateVulkanInterface(RenderWindow sdlWindow);
#endif
    extern std::unique_ptr<RenderInterface> CreateD3D12Interface();
}

void Graphics::createFramebuffers(){
    renderCtx.framebuffers.clear();

    for (unsigned int i = 0; i < renderCtx.swapChain->getTextureCount(); i++) {
        const plume::RenderTexture* colorAttachment = renderCtx.swapChain->getTexture(i);

        plume::RenderFramebufferDesc fbDesc;
        fbDesc.colorAttachments = &colorAttachment;
        fbDesc.colorAttachmentsCount = 1;
        fbDesc.depthAttachment = nullptr;

        auto framebuffer = renderCtx.device->createFramebuffer(fbDesc);
        renderCtx.framebuffers.push_back(std::move(framebuffer));
    }
}

void Graphics::resize(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;

    if (renderCtx.swapChain) {
        renderCtx.framebuffers.clear();

        if (!renderCtx.swapChain->resize())
            LOG("resize: Failed to resize swapchain!")

        createFramebuffers();
    }
}

void Graphics::initializeRenderResources() {
    renderCtx.device = renderCtx.renderInterface->createDevice();
    renderCtx.commandQueue = renderCtx.device->createCommandQueue(plume::RenderCommandListType::DIRECT);
    renderCtx.fence = renderCtx.device->createCommandFence();
    renderCtx.swapChain = renderCtx.commandQueue->createSwapChain(renderCtx.renderWindow, 2, plume::RenderFormat::B8G8R8A8_UNORM, 2);
    renderCtx.swapChain->resize();
    renderCtx.commandList = renderCtx.commandQueue->createCommandList();
    renderCtx.acquireSemaphore = renderCtx.device->createCommandSemaphore();
    renderCtx.commandFence = renderCtx.device->createCommandFence();
    createFramebuffers();

    plume::RenderBufferDesc cBufferDesc{};
    cBufferDesc.size = sizeof(MainCBuffer);
    cBufferDesc.flags = plume::RenderBufferFlag::CONSTANT;
    cBufferDesc.heapType = plume::RenderHeapType::UPLOAD;
    renderCtx.mainCBuffer = renderCtx.device->createBuffer(cBufferDesc);
}

bool Graphics::init() {
    if (!SDL_Init(SDL_INIT_VIDEO))
        LOGB("SDL: Failed to init SDL!");

    window = SDL_CreateWindow(name, width, height, SDL_WINDOW_RESIZABLE);
    if (!window)
        LOGB("SDL: Failed to init SDL window!");

#ifdef WIN32
    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    if (props == 0)
        LOGB("SDL: No window properties were loaded!");

    renderCtx.renderWindow = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    renderCtx.renderInterface = plume::CreateVulkanInterface();
#else
    renderCtx.renderWindow = window;
    renderCtx.renderInterface = plume::CreateVulkanInterface(renderCtx.renderWindow);
#endif

    initializeRenderResources();

    // imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOther(window);
    ImGui_ImplPlume_InitInfo initInfo{
        .rInterface = renderCtx.renderInterface.get(),
        .device = renderCtx.device.get()
    };
    ImGui_ImplPlume_Init(initInfo);
    ImGui_ImplPlume_CreateFontsTexture();

    camera = new Camera{ width, height };

    instance = this;

    return true;
}

static uint32_t imageIndex{ 0 };
static plume::RenderTexture* swapChainTexture;
void Graphics::renderBegin()
{
    renderCtx.swapChain->acquireTexture(renderCtx.acquireSemaphore.get(), &imageIndex);

    renderCtx.commandList->begin();

    swapChainTexture = renderCtx.swapChain->getTexture(imageIndex);
    renderCtx.commandList->barriers(plume::RenderBarrierStage::GRAPHICS, plume::RenderTextureBarrier(swapChainTexture, plume::RenderTextureLayout::COLOR_WRITE));

    const plume::RenderFramebuffer* framebuffer{ renderCtx.framebuffers[imageIndex].get() };
    renderCtx.commandList->setFramebuffer(framebuffer);

    uint32_t width{ renderCtx.swapChain->getWidth() };
    uint32_t height{ renderCtx.swapChain->getHeight() };
    const plume::RenderViewport viewport{ 0.0f, 0.0f, (float)width, (float)height };
    const plume::RenderRect scissor{ 0, 0, (int32_t)width, (int32_t)height };

    renderCtx.commandList->setViewports(viewport);
    renderCtx.commandList->setScissors(scissor);

    plume::RenderColor clearColor{ 0.1f, 0.12f, 0.15f, 1.0f };
    renderCtx.commandList->clearColor(0, clearColor);

    mainCbufferData.view = camera->viewMatrix();
    mainCbufferData.projection = camera->projMatrix();
    updateMainCbuffer();

    for (auto* model : models)
        model->render();

    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("camera");
    ImGui::Text("Position: %f, %f, %f", camera->position.x, camera->position.y, camera->position.z);
    ImGui::Text("Rotation: %f, %f, %f", camera->rotation.x, camera->rotation.y, camera->rotation.z);
    ImGui::End();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, (float)width, (float)height);
}

void Graphics::renderEnd()
{
    ImGui::Render();
    ImGui_ImplPlume_RenderDrawData(ImGui::GetDrawData(), renderCtx.commandList.get());

    renderCtx.commandList->barriers(plume::RenderBarrierStage::NONE, plume::RenderTextureBarrier{ swapChainTexture, plume::RenderTextureLayout::PRESENT });

    renderCtx.commandList->end();

    while (renderCtx.releaseSemaphores.size() < renderCtx.swapChain->getTextureCount())
        renderCtx.releaseSemaphores.emplace_back(renderCtx.device->createCommandSemaphore());

    const plume::RenderCommandList* cmdList = renderCtx.commandList.get();
    plume::RenderCommandSemaphore* waitSemaphore = renderCtx.acquireSemaphore.get();
    plume::RenderCommandSemaphore* signalSemaphore = renderCtx.releaseSemaphores[imageIndex].get();

    renderCtx.commandQueue->executeCommandLists(&cmdList, 1, &waitSemaphore, 1, &signalSemaphore, 1, renderCtx.fence.get());

    renderCtx.swapChain->present(imageIndex, &signalSemaphore, 1);
    renderCtx.commandQueue->waitForCommandFence(renderCtx.fence.get());
}

void Graphics::shutdown()
{
    if (auto* vkDevice = dynamic_cast<plume::VulkanDevice*>(renderCtx.device.get()))
        vkDeviceWaitIdle(vkDevice->vk);

    ImGui_ImplPlume_Shutdown();

    for (auto* model : models)
        model->shutdown();

    renderCtx.commandList.reset();
    renderCtx.fence.reset();
    renderCtx.commandFence.reset();
    renderCtx.acquireSemaphore.reset();
    renderCtx.releaseSemaphores.clear();

    renderCtx.framebuffers.clear();
    renderCtx.mainCBuffer.reset();

    renderCtx.swapChain.reset();

    renderCtx.commandQueue.reset();

    renderCtx.device.reset();

    renderCtx.renderInterface.reset();
}

Graphics::~Graphics()
{
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(window);
    SDL_Quit();
}
