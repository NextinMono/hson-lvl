#pragma once
#include <vector>
#include <filesystem>
#include <chrono>

#include "app/applistener.h"
#include "app/service.h"
#include "graphics/graphics.h"
#include "ui/panel.h"

namespace ulvl {
	class Application {
	public:
		struct FrameTimer {
			using clock = std::chrono::high_resolution_clock;
			std::chrono::time_point<clock> lastTime = clock::now();
			float deltaTime = 0.0f;
			float fps = 0.0f;

			void update();
		};

		static Application* instance;

		gfx::Graphics* graphics;
		bool isRunning{ true };
		FrameTimer frameTimer;
		SDL_Event e;
		std::vector<Panel*> panels;
		std::vector<app::Service*> services;
		std::vector<app::ApplicationListener*> appListeners;

		void init();
		void loop();
		template<typename T>
		void addPanel() {
			panels.push_back(new T{});
		}
		template<typename T>
		void addService() {
			services.push_back(new T{});
			services[services.size() - 1]->AddCallback();
		}
		template<typename T>
		T* getService() {
			for (auto* service : services)
				if (T* tService = dynamic_cast<T*>(service))
					return tService;
			return nullptr;
		}
		inline void addListener(app::ApplicationListener* listener) {
			appListeners.push_back(listener);
		}
		~Application();

	private:
		void render();
		void update();
	};
}
