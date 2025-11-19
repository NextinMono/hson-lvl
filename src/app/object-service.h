#pragma once
#include "service.h"
#include "applistener.h"
#include "../graphics/model.h"
#include <hedgelib/hh/hl_hh_gedit.h>
#include <hedgelib/sets/hl_set_obj_type.h>

namespace ulvl::app {
	class ObjectService : public Service, public ApplicationListener {
	public:
		struct Object {
			hl::guid guid;
			hl::hson::object* hson;
			gfx::Model* model;
			std::vector<Object*> children;
			hl::hson::project* owner;

			Object(const hl::guid& guid, hl::hson::object* hson, hl::hson::project* proj);
			~Object();
			void setPosition(const glm::vec3& pos);
			void setRotation(const glm::vec3& rot);
			void setQuaternion(const glm::quat& rot);
			void setLocalPosition(const glm::vec3& pos);
			void setLocalRotation(const glm::vec3& rot);
			void setLocalQuaternion(const glm::quat& rot);
			glm::vec3 getLocalPosition() const;
			glm::vec3 getLocalRotation() const;
			glm::quat getLocalQuaternion() const;
			glm::vec3 getPosition() const;
			glm::vec3 getRotation() const;
			glm::quat getQuaternion() const;
			glm::mat4 getWorldMatrix() const;
			glm::mat4 getLocalMatrix() const;
			Object* getParent() const;
			bool hasParent() const;
			bool hasChildren() const;
			void updateChildren();
			inline const std::string getDisplayName() const {
				if (hson->name.has_value())
					return hson->name.value();

				return std::string{ hson->type + " - " + guid.as_string() };
			}
			void updateModel();
			void updateModelMat();
			void updateDebugVisual();
			void addDynamicVisual();
			void dynamicVisualReset();
			void removeDynamicVisual();
		};

		std::vector<Object*> objects;

		Object* addObject(const hl::guid& guid, hl::hson::object* hson, hl::hson::project* proj);
		void removeObject(Object* object, bool removeFromLayer = true);
		void removeObject(const hl::guid& guid, bool removeFromLayer = true);
		Object* getObject(const hl::guid& guid);
		Object* createObject(hl::hson::project* proj);
		Object* createObject(hl::hson::project* proj, std::string& typeName, hl::set_object_type* type);
		Object* createInstanceOf(hl::hson::project* proj, const hl::guid& baseObj);
	};
}
