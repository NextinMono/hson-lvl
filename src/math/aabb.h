#pragma once
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ulvl::math {
	struct Aabb {
		glm::vec3 min{ 0, 0, 0 };
		glm::vec3 max{ 1, 1, 1 };
	};
}
