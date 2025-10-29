#pragma once

namespace ulvl::app {
	class Service {
	public:
		virtual ~Service() {};
		virtual void AddCallback() {};
		virtual void Update() {};
	};
}
