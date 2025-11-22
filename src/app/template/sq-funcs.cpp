#include "sq-funcs.h"
#include "../object-service.h"
#include "../debug-visual-service.h"
#include "../../app.h"
#include <glm/gtx/quaternion.hpp>

using namespace ulvl::app;

SQInteger ulvl::app::ToBytes(HSQUIRRELVM vm) {
	switch (sq_gettype(vm, 2)) {
	case OT_FLOAT: {
		SQFloat f{};
		sq_getfloat(vm, 2, &f);

		char* b = (char*)&f;

		sq_newarray(vm, 0);
		for (auto i = 0; i < sizeof(float); i++) {
			sq_pushinteger(vm, b[i]);
			sq_arrayappend(vm, -2);
		}
		break;
	}
	}

	return 1;
}

// std::string

SQInteger ulvl::app::StringCtor(HSQUIRRELVM vm) {
	const char* str = "";
	sq_getstring(vm, 2, &str);

	std::string* s = new std::string(str);

	sq_setinstanceup(vm, 1, s);
	sq_setreleasehook(vm, 1, StringReleaseHook);

	return 0;
}

SQInteger ulvl::app::StringReleaseHook(SQUserPointer p, SQInteger size) {
	delete (std::string*)p;
	return 0;
}

SQInteger ulvl::app::StringToCString(HSQUIRRELVM vm) {
	std::string* self{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);
	sq_pushstring(vm, self->c_str(), -1);
	return 1;
}

// hl::radix_tree<hl::hson::parameter>

SQInteger ulvl::app::GetParameterFloat(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	float value{ 0 };

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = param.second.value_floating();
			break;
		}
	}

	sq_pushfloat(vm, value);

	return 1;
}

SQInteger ulvl::app::GetParameterInt(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	int value{ 0 };

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = param.second.value_int();
			break;
		}
	}

	sq_pushinteger(vm, value);

	return 1;
}

SQInteger ulvl::app::GetParameterUInt(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	unsigned int value{ 0 };

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = param.second.value_uint();
			break;
		}
	}

	sq_pushinteger(vm, value);

	return 1;
}

SQInteger ulvl::app::GetParameterString(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	std::string* value{};

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = &param.second.value_string();
			break;
		}
	}

	sq_pushroottable(vm);
	sq_pushstring(vm, "String", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_remove(vm, -2);
	sq_remove(vm, -2);
	sq_setinstanceup(vm, -1, value);
	return 1;
}

SQInteger ulvl::app::GetParameterArray(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	std::vector<hl::hson::parameter> value;

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = param.second.value_array();
			break;
		}
	}

	sq_newarray(vm, 0);

	for (auto& param : value) {
		switch (param.type()) {
		case hl::hson::parameter_type::boolean: sq_pushbool(vm, param.value_bool()); break;
		case hl::hson::parameter_type::signed_integer: sq_pushinteger(vm, param.value_int()); break;
		case hl::hson::parameter_type::unsigned_integer: sq_pushinteger(vm, param.value_uint()); break;
		case hl::hson::parameter_type::floating: sq_pushfloat(vm, (float)param.value_floating()); break;
		case hl::hson::parameter_type::string: {
			sq_pushroottable(vm);
			sq_pushstring(vm, "String", -1);
			sq_get(vm, -2);
			sq_createinstance(vm, -1);
			sq_remove(vm, -2);
			sq_remove(vm, -2);
			std::string* string = new std::string{ param.value_string() };
			sq_setinstanceup(vm, -1, string);
			sq_setreleasehook(vm, -1, StringReleaseHook);
			break;
		}
		case hl::hson::parameter_type::object: {
			sq_pushroottable(vm);
			sq_pushstring(vm, "Parameters", -1);
			sq_get(vm, -2);
			sq_createinstance(vm, -1);
			sq_remove(vm, -2);
			sq_remove(vm, -2);
			hl::radix_tree<hl::hson::parameter>* parameters = new hl::radix_tree<hl::hson::parameter>{ param.value_object() };
			sq_setinstanceup(vm, -1, parameters);
			sq_setreleasehook(vm, -1, RadixTreeReleaseHook);
			break;
		}
		}
		sq_arrayappend(vm, -2);
	}

	HSQOBJECT arrObj;
	sq_getstackobj(vm, -1, &arrObj);
	sq_addref(vm, &arrObj);

	return 1;
}

SQInteger ulvl::app::GetParameterObject(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	hl::radix_tree<hl::hson::parameter>* value{};

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = &param.second.value_object();
			break;
		}
	}

	sq_pushroottable(vm);
	sq_pushstring(vm, "Parameters", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, value);
	return 1;
}

SQInteger ulvl::app::RadixTreeReleaseHook(SQUserPointer p, SQInteger size) {
	delete (hl::radix_tree<hl::hson::parameter>*)p;
	return 0;
}

// Object

SQInteger ulvl::app::GetObjLocalPosition(HSQUIRRELVM vm) {
	ObjectService::Object* self = nullptr;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	glm::vec3* copy = new glm::vec3{ self->getLocalPosition() };
	sq_setinstanceup(vm, -1, copy);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);
	return 1;
}

SQInteger ulvl::app::GetObjWorldPosition(HSQUIRRELVM vm) {
	ObjectService::Object* self = nullptr;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	glm::vec3* copy = new glm::vec3{ self->getPosition() };
	sq_setinstanceup(vm, -1, copy);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);
	return 1;
}
SQInteger ulvl::app::GetObjRotation(HSQUIRRELVM vm) {
	ObjectService::Object* self = nullptr;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec4", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	glm::quat rot = self->getQuaternion();
	glm::vec4* copy = new glm::vec4(rot.w, rot.x, rot.y, rot.z);
	sq_setinstanceup(vm, -1, copy);
	sq_setreleasehook(vm, -1, Vec4ReleaseHook);
	return 1;
}
SQInteger ulvl::app::GetObjName(HSQUIRRELVM vm) {
	ObjectService::Object* self = nullptr;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "String", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, &self->hson->name.value());
	return 1;
}

SQInteger ulvl::app::GetObjParameters(HSQUIRRELVM vm) {
	ObjectService::Object* self{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Parameters", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, &self->hson->parameters);
	return 1;
}

SQInteger ulvl::app::GetObjParent(HSQUIRRELVM vm) {
	ObjectService::Object* self{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Object", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, self->getParent());
	return 1;
}

SQInteger ulvl::app::GetObjID(HSQUIRRELVM vm) {
	ObjectService::Object* self{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	auto guid = self->guid.as_string();
	sq_pushstring(vm, guid.c_str(), -1);
	return 1;
}

SQInteger ulvl::app::GetObjHasParent(HSQUIRRELVM vm) {
	ObjectService::Object* self{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushbool(vm, self->hasParent());
	return 1;
}

SQInteger ulvl::app::GetObjByID(HSQUIRRELVM vm) {
	const char* id{};
	sq_getstring(vm, 2, &id);

	hl::guid guid{ id };
	auto* obj = Application::instance->getService<app::ObjectService>()->getObject(guid);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Object", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, obj);
	return 1;
}

// glm::vec3

SQInteger ulvl::app::Vec3Ctor(HSQUIRRELVM vm) {
	SQFloat x = 0, y = 0, z = 0;

	sq_getfloat(vm, 2, &x);
	sq_getfloat(vm, 3, &y);
	sq_getfloat(vm, 4, &z);

	glm::vec3* obj = new glm::vec3{ x, y, z };
	sq_setinstanceup(vm, 1, obj);
	sq_setreleasehook(vm, 1, Vec3ReleaseHook);

	return 0;
}

SQInteger ulvl::app::Vec3GetX(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);

	sq_pushfloat(vm, vec3->x);

	return 1;
}

SQInteger ulvl::app::Vec3SetX(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);
	SQFloat val{};
	sq_getfloat(vm, 2, &val);
	vec3->x = val;
	return 0;
}

SQInteger ulvl::app::Vec3GetY(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);

	sq_pushfloat(vm, vec3->y);

	return 1;
}

SQInteger ulvl::app::Vec3SetY(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);
	SQFloat val{};
	sq_getfloat(vm, 2, &val);
	vec3->y = val;
	return 0;
}

SQInteger ulvl::app::Vec3GetZ(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);

	sq_pushfloat(vm, vec3->z);

	return 1;
}
SQInteger ulvl::app::Vec3RotateByQuat(HSQUIRRELVM vm) {
	//Easier to do it this way than remake it in squirrel

	glm::vec3* vec;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec, 0, SQFalse);
	glm::vec4* quat;
	sq_getinstanceup(vm, 2, (SQUserPointer*)&quat, 0, SQFalse);

	glm::quat q = glm::quat(quat->w, quat->x, quat->y, quat->z);
	glm::vec3 rotated = q * (*vec);
	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_remove(vm, -2);

	sq_createinstance(vm, -1);
	glm::vec3* retVec = new glm::vec3(rotated);
	sq_setinstanceup(vm, -1, retVec);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);

	return 1;
}

SQInteger ulvl::app::Vec3SetZ(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);
	SQFloat val{};
	sq_getfloat(vm, 2, &val);
	vec3->z = val;
	return 0;
}

SQInteger ulvl::app::Vec3ReleaseHook(SQUserPointer p, SQInteger size) {
	delete (glm::vec3*)p;
	return 0;
}

// glm::vec4
SQInteger ulvl::app::Vec4Ctor(HSQUIRRELVM vm) {
	SQFloat x = 0, y = 0, z = 0, w = 0;

	sq_getfloat(vm, 2, &x);
	sq_getfloat(vm, 3, &y);
	sq_getfloat(vm, 4, &z);
	sq_getfloat(vm, 5, &w);

	glm::vec4* obj = new glm::vec4{ x, y, z, w };
	sq_setinstanceup(vm, 1, obj);
	sq_setreleasehook(vm, 1, Vec4ReleaseHook);

	return 0;
}

SQInteger ulvl::app::Vec4GetW(HSQUIRRELVM vm) {
	glm::vec4* vec4{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec4, nullptr, SQFalse);

	sq_pushfloat(vm, vec4->w);

	return 1;
}

SQInteger ulvl::app::Vec4SetW(HSQUIRRELVM vm) {
	glm::vec4* vec4{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec4, nullptr, SQFalse);
	SQFloat val{};
	sq_getfloat(vm, 2, &val);
	vec4->w = val;
	return 0;
}

SQInteger ulvl::app::Vec4ReleaseHook(SQUserPointer p, SQInteger size) {
	delete (glm::vec4*)p;
	return 0;
}

// ModelData

SQInteger ulvl::app::ModelDataGetVertexCount(HSQUIRRELVM vm) {
	ModelData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	sq_pushinteger(vm, modelData->vertexCount);

	return 1;
}

SQInteger ulvl::app::ModelDataSetVertexCount(HSQUIRRELVM vm) {
	ModelData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	sq_getinteger(vm, 2, (SQInteger*)&modelData->vertexCount);

	return 0;
}

SQInteger ulvl::app::ModelDataGetVertexStride(HSQUIRRELVM vm) {
	ModelData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	sq_pushinteger(vm, modelData->vertexStride);

	return 1;
}

SQInteger ulvl::app::ModelDataSetVertexStride(HSQUIRRELVM vm) {
	ModelData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	sq_getinteger(vm, 2, (SQInteger*)&modelData->vertexStride);

	return 0;
}

SQInteger ulvl::app::ModelDataSetVertices(HSQUIRRELVM vm) {
	ModelData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	if (modelData->vertices) delete modelData->vertices;

	modelData->vertices = new char[modelData->vertexCount * modelData->vertexStride];
	for (size_t x = 0; x < (modelData->vertexCount * modelData->vertexStride); x++) {
		sq_pushinteger(vm, x);
		sq_get(vm, 2);
		SQInteger val{};
		sq_getinteger(vm, -1, &val);
		((char*)modelData->vertices)[x] = (char)val;
		sq_pop(vm, 1);
	}

	return 0;
}

SQInteger ulvl::app::ModelDataGetIndexCount(HSQUIRRELVM vm) {
	ModelData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	sq_pushinteger(vm, modelData->indexCount);

	return 1;
}

SQInteger ulvl::app::ModelDataSetIndexCount(HSQUIRRELVM vm) {
	ModelData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	sq_getinteger(vm, 2, (SQInteger*)&modelData->indexCount);

	return 0;
}

SQInteger ulvl::app::ModelDataSetIndices(HSQUIRRELVM vm) {
	ModelData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	if (modelData->indices) delete modelData->indices;

	modelData->indices = new unsigned short[modelData->indexCount];
	for (size_t x = 0; x < modelData->indexCount; x++) {
		sq_pushinteger(vm, x);
		sq_get(vm, 2);
		SQInteger val{};
		sq_getinteger(vm, -1, &val);
		modelData->indices[x] = val;
		sq_pop(vm, 1);
	}

	return 0;
}

SQInteger ulvl::app::DebugVisualDrawCube(HSQUIRRELVM vm) {
	DebugVisualService* debugVisual{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&debugVisual, nullptr, SQFalse);

	glm::vec4* color{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&color, nullptr, SQFalse);

	glm::vec3* pos{};
	sq_getinstanceup(vm, 3, (SQUserPointer*)&pos, nullptr, SQFalse);

	glm::vec4* rot{};
	sq_getinstanceup(vm, 4, (SQUserPointer*)&rot, nullptr, SQFalse);

	glm::vec3* scale{};
	sq_getinstanceup(vm, 5, (SQUserPointer*)&scale, nullptr, SQFalse);

	ObjectService::Object* object{};
	sq_getinstanceup(vm, 6, (SQUserPointer*)&object, nullptr, SQFalse);

	gfx::InstancedMesh mesh{};
	mesh.id = (int)object;
	mesh.color = *color;
	mesh.worldMatrix = glm::translate(glm::mat4{ 1 }, *pos) * glm::toMat4(*(glm::quat*)rot) * glm::scale(glm::mat4{ 1 }, *scale);
	debugVisual->addCube(mesh);

	return 0;
}

SQInteger ulvl::app::DebugVisualDrawSphere(HSQUIRRELVM vm) {
	DebugVisualService* debugVisual{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&debugVisual, nullptr, SQFalse);

	glm::vec4* color{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&color, nullptr, SQFalse);

	glm::vec3* pos{};
	sq_getinstanceup(vm, 3, (SQUserPointer*)&pos, nullptr, SQFalse);

	float radius{ 0 };
	sq_getfloat(vm, 4, &radius);

	ObjectService::Object* object{};
	sq_getinstanceup(vm, 5, (SQUserPointer*)&object, nullptr, SQFalse);

	gfx::InstancedMesh mesh{};
	mesh.id = (int)object;
	mesh.color = *color;
	mesh.worldMatrix = glm::translate(glm::mat4{ 1 }, *pos) * glm::scale(glm::mat4{ 1 }, glm::vec3{ radius * 2 });
	debugVisual->addSphere(mesh);

	return 0;
}

SQInteger ulvl::app::DebugVisualDrawCylinder(HSQUIRRELVM vm) {
	DebugVisualService* debugVisual{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&debugVisual, nullptr, SQFalse);

	glm::vec4* color{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&color, nullptr, SQFalse);

	glm::vec3* pos{};
	sq_getinstanceup(vm, 3, (SQUserPointer*)&pos, nullptr, SQFalse);

	glm::vec4* rot{};
	sq_getinstanceup(vm, 4, (SQUserPointer*)&rot, nullptr, SQFalse);

	float radius{ 0 };
	sq_getfloat(vm, 5, &radius);

	float height{ 0 };
	sq_getfloat(vm, 6, &height);

	ObjectService::Object* object{};
	sq_getinstanceup(vm, 7, (SQUserPointer*)&object, nullptr, SQFalse);

	gfx::InstancedMesh mesh{};
	mesh.id = (int)object;
	mesh.color = *color;
	mesh.worldMatrix = glm::translate(glm::mat4{ 1 }, *pos) * glm::toMat4(*(glm::quat*)rot) * glm::scale(glm::mat4{ 1 }, glm::vec3{ radius * 2, height, radius * 2 });
	debugVisual->addCylinder(mesh);

	return 0;
}
