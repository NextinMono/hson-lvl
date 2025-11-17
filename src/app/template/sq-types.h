#pragma once
#include <squirrel.h>
#include "sq-funcs.h"

#define NEW_CLASS(NAME, REGISTER_BODY) { \
	sq_pushroottable(vm); \
	sq_pushstring(vm, #NAME, -1); \
	sq_newclass(vm, SQFalse); \
	REGISTER_BODY \
	sq_newslot(vm, -3, SQFalse); \
	sq_pop(vm, 1); \
}

#define NEW_FUNC(NAME, FUNC) sq_pushstring(vm, NAME, -1); sq_newclosure(vm, FUNC, 0); sq_newslot(vm, -3, SQFalse);
#define NEW_MEMBER_RW(NAME, GET, SET) NEW_FUNC(NAME, GET) NEW_FUNC(NAME, SET)
#define NEW_MEMBER_R(NAME, GET) NEW_FUNC(NAME, GET)

namespace ulvl::app {
	void registerTypes(SquirrelWrap& wrap) {
		auto& vm = wrap.vm;

		NEW_CLASS(
			String,
			NEW_FUNC("constructor", StringCtor)
			NEW_FUNC("c_str", StringToCString)
		);

		NEW_CLASS(
			Vec3, 
			NEW_FUNC("constructor", Vec3Ctor)
			NEW_MEMBER_R("x", Vec3GetX, Vec3SetX)
			NEW_MEMBER_R("y", Vec3GetY, Vec3SetY)
			NEW_MEMBER_R("z", Vec3GetZ, Vec3SetZ)
		);

		NEW_CLASS(
			Vec4,
			NEW_FUNC("constructor", Vec4Ctor)
			NEW_MEMBER_R("x", Vec3GetX, Vec3SetX)
			NEW_MEMBER_R("y", Vec3GetY, Vec3SetY)
			NEW_MEMBER_R("z", Vec3GetZ, Vec3SetZ)
			NEW_MEMBER_R("w", Vec4GetW, Vec4SetW)
		);

		NEW_CLASS(
			Parameters,
			NEW_FUNC("GetParameterFloat", GetParameterFloat)
			NEW_FUNC("GetParameterInt", GetParameterInt)
			NEW_FUNC("GetParameterUInt", GetParameterUInt)
			NEW_FUNC("GetParameterString", GetParameterString)
			NEW_FUNC("GetParameterArray", GetParameterArray)
			NEW_FUNC("GetParameterObject", GetParameterObject)
		);

		NEW_CLASS(
			Object,
			NEW_MEMBER_R("position", GetObjWorldPosition)
			NEW_MEMBER_R("localPosition", GetObjLocalPosition)
			NEW_MEMBER_R("name", GetObjName)
			NEW_MEMBER_R("parameters", GetObjParameters)
			NEW_MEMBER_R("parent", GetObjParent)
			NEW_MEMBER_R("id", GetObjID)
			NEW_FUNC("HasParent", GetObjHasParent)
		);

		NEW_CLASS(
			ModelData,
			NEW_FUNC("GetVertexCount", ModelDataGetVertexCount)
			NEW_FUNC("SetVertexCount", ModelDataSetVertexCount)
			NEW_FUNC("GetVertexStride", ModelDataGetVertexStride)
			NEW_FUNC("SetVertexStride", ModelDataSetVertexStride)
			NEW_FUNC("SetVertices", ModelDataSetVertices)

			NEW_FUNC("GetIndexCount", ModelDataGetIndexCount)
			NEW_FUNC("SetIndexCount", ModelDataSetIndexCount)
			NEW_FUNC("SetIndices", ModelDataSetIndices)
		);

		NEW_CLASS(
			DebugVisual,
			NEW_FUNC("DrawCube", DebugVisualDrawCube)
			NEW_FUNC("DrawSphere", DebugVisualDrawSphere)
			NEW_FUNC("DrawCylinder", DebugVisualDrawCylinder)
		);
	}
}
