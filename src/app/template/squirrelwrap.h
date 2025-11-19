#pragma once
#include <squirrel.h>
#include "../object-service.h"
#include <filesystem>

#define RegisterFunc(FUNC) registerFunction(FUNC, #FUNC)

namespace ulvl::app {
	struct ModelData {
		void* vertices;
		size_t vertexCount;
		size_t vertexStride;
		unsigned short* indices;
		size_t indexCount;
	};

	struct SquirrelWrap {
		typedef SQInteger(*SQFUNCTION)(HSQUIRRELVM);
		HSQUIRRELVM vm;

		~SquirrelWrap() { sq_close(vm); }
		void init();
		void loadFile(const std::filesystem::path& path);
		void registerFunction(SQFUNCTION func, const char* funcName);
		ModelData callGetModelData(ObjectService::Object* obj);
		void callAddDebugVisual(ObjectService::Object* obj);
		void callAddDynamicDebugVisual(ObjectService::Object* obj);
		void callDynamicDebugVisualEnd(ObjectService::Object* obj);
	};
}
