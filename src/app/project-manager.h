#pragma once
#include "service.h"
#include "object-service.h"
#include <filesystem>
#include <hedgelib/hh/hl_hh_gedit.h>

namespace ulvl::app {
	class ProjectManager : public Service {
	public:
		struct Project {
			std::filesystem::path hsonPath;
			hl::hson::project* hson;
			std::vector<ObjectService::Object*> objects;

			Project(const std::filesystem::path& hsonPath);
		};

		std::vector<Project*> projects;

		void loadProject(const std::filesystem::path& hsonPath);
	};
}
