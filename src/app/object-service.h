#pragma once
#include "service.h"
#include <hedgelib/hh/hl_hh_gedit.h>

namespace ulvl::app {
	class ObjectService : public Service {
	public:
		struct Object {
			hl::hson::object* hson;
		};

		std::vector<Object*> objects;
	};
}
