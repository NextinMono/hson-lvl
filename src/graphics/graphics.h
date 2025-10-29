#pragma once
#include "model.h"
#include "camera.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <plume_vulkan.h>
#include <plume_render_interface.h>

/*
* Initialization and use of plume taken from plume's example.
*/

namespace ulvl::gfx {
	struct MainCBuffer {
		glm::mat4 view;
		glm::mat4 projection;
	};

	class Graphics {
	public:
		static Graphics* instance;

		struct RenderContext {
			std::unique_ptr<plume::RenderInterface> renderInterface;
			plume::RenderWindow renderWindow{};
			std::unique_ptr<plume::RenderDevice> device;
			std::unique_ptr<plume::RenderCommandQueue> commandQueue;
			std::unique_ptr<plume::RenderCommandList> commandList;
			std::unique_ptr<plume::RenderCommandFence> fence;
			std::unique_ptr<plume::RenderSwapChain> swapChain;
			std::unique_ptr<plume::RenderCommandSemaphore> acquireSemaphore;
			std::vector<std::unique_ptr<plume::RenderCommandSemaphore>> releaseSemaphores;
			std::unique_ptr<plume::RenderCommandFence> commandFence;
			std::vector<std::unique_ptr<plume::RenderFramebuffer>> framebuffers;
			std::unique_ptr<plume::RenderBuffer> mainCBuffer;
		};

		void createFramebuffers();
		void resize(unsigned int width, unsigned int height);
		void initializeRenderResources();

		SDL_Window* window;
		unsigned int width{ 1280 }, height{ 720 };
		float deltaTime{ 0 };
		Camera* camera;
		const char* name{ "hson-lvl" };
		std::vector<Model*> models;
		MainCBuffer mainCbufferData;
		RenderContext renderCtx;

		inline void updateMainCbuffer() {
			void* mapped = renderCtx.mainCBuffer->map();
			memcpy(mapped, &mainCbufferData, sizeof(MainCBuffer));
			renderCtx.mainCBuffer->unmap();
		}

		bool init();
		void renderBegin();
		void renderEnd();
		void shutdown();
		~Graphics();
	};
}
