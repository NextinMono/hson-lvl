#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/quaternion_float.hpp>

#define DEFAULT_FORWARD_VECTOR glm::vec3{0, 0, -1}
#define DEFAULT_RIGHT_VECTOR glm::vec3{-1, 0, 0}

class Camera {
public:
	glm::vec3 position{ 0, 0, 0 };
	glm::vec3 rotation{ 0, 0, 0 };
	float fov{ 70 };
	float aspectRatio{ 1280.0f / 720.0f };
	float nearZ{ 0.1f };
	float farZ{ 1000 };

private:
	glm::mat4x4 viewMat{};
	glm::mat4x4 projMat{};
	glm::vec3 vecForward{ 0, 0, 1 };
	glm::vec3 vecRight{ 1, 0, 0 };
	void updateProjMat();
	void updateViewMat();

public:
	Camera();
	Camera(unsigned int width, unsigned int height);

    glm::vec3 front() const;
    glm::vec3 right() const;
	glm::mat4 viewMatrix() const;
	glm::mat4 projMatrix() const;

	void setPosition(const glm::vec3& pos);
	void addPosition(const glm::vec3& pos);
	void setRotation(const glm::vec3& rot);
	void addRotation(const glm::vec3& rot);
	void setFov(float fov);
	void setAspectRatio(float aspect);
	void setNearZ(float nearZ);
	void setFarZ(float farZ);
};
