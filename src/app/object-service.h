#pragma once
#include "service.h"
#include "../graphics/model.h"
#include <hedgelib/hh/hl_hh_gedit.h>

namespace ulvl::app {
	class ObjectService : public Service {
	public:
		struct Object {
			hl::hson::object* hson;
			gfx::Model* model;

			Object(hl::hson::object* hson);
		};

		std::vector<Object*> objects;

		Object* addObject(hl::hson::object* hson);
	};
}
