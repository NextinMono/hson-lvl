#pragma once
#include "service.h"
#include "object-service.h"
#include "applistener.h"
#include <filesystem>
#include <hedgelib/hh/hl_hh_gedit.h>

namespace ulvl::app {
	class ProjectManager : public Service, public ApplicationListener {
	public:
		struct Layer {
			std::filesystem::path hsonPath;
			hl::hson::project* hson;
			std::vector<ObjectService::Object*> objects;

			Layer(const std::filesystem::path& hsonPath);
			Layer(const std::string& name);
			void save();
		};

		struct Project {
			std::filesystem::path folderPath;
			std::vector<Layer*> layers;

			Project(const std::filesystem::path& folderPath);
			Project(const std::string& name);
			void loadLayer(const std::filesystem::path& hsonPath);
			Layer* getLayer(const hl::hson::project* project);
			void save();
		};

		std::vector<Project*> projects;
		std::vector<Layer*> layers;
		bool unsaved{ false };

		virtual void AddCallback() override;

		void loadProject(const std::filesystem::path& folderPath);
		void loadLayer(const std::filesystem::path& hsonPath);
		void addProject(const std::string& name);
		void addLayer(const std::string& name);
		Layer* getLayer(const hl::hson::project* project);
		void setUnsaved(bool unsaved);
		void newProj();
		void newLayer();
		void saveAll();

		virtual void EventCallback(SDL_Event e) override;
	};
}
