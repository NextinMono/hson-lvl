#pragma once
#include "service.h"
#include "applistener.h"

namespace ulvl::app {
	class CameraService : public Service, public ApplicationListener {
	public:
		virtual void Update() override;
		virtual void EventCallback(SDL_Event e) override;
	};
}
