#pragma once
#include "imgui.h"

namespace ulvl {
	class Panel {
	public:
		struct Properties {
			const char* name;
			ImVec2 position;
			ImVec2 size;
			ImVec2 pivot;
		};

		virtual Properties GetProperties() { return {}; }
		virtual void RenderPanel() {}
		void Render() {
			Properties traits{ GetProperties() };

			ImGui::SetNextWindowPos(traits.position, ImGuiCond_FirstUseEver, traits.pivot);
			ImGui::SetNextWindowSize(traits.size, ImGuiCond_FirstUseEver);

			if (ImGui::Begin(traits.name, 0, ImGuiWindowFlags_NoCollapse))
				RenderPanel();

			ImGui::End();
		}
	};
}
