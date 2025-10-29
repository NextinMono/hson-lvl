#include "camera.h"
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

void Camera::updateProjMat() {
    projMat = glm::perspective(glm::radians(fov), aspectRatio, nearZ, farZ);
}

void Camera::updateViewMat() {
    glm::vec3 pos{ position };
    glm::vec3 rot{ rotation };

    glm::mat4 camRotationMatrix{ glm::yawPitchRoll(rot.y, rot.x, rot.z) };

    glm::vec3 camTarget{ glm::vec3{ camRotationMatrix * glm::vec4{ 0, 0, 1, 1.0f } } };
    camTarget += pos;

    glm::vec3 upDir{ glm::vec3{ camRotationMatrix * glm::vec4{ 0, -1, 0, 1.0f } } };

    viewMat = glm::lookAtLH(pos, camTarget, upDir);

    glm::mat4 vecRotationMatrix{ glm::yawPitchRoll(rot.y, 0.0f, 0.0f) };

    vecForward = glm::vec3{ vecRotationMatrix * glm::vec4{ DEFAULT_FORWARD_VECTOR, 0.0f } };
    vecRight = glm::vec3{ vecRotationMatrix * glm::vec4{ DEFAULT_RIGHT_VECTOR, 0.0f } };
}

Camera::Camera() {
    updateViewMat();
    updateProjMat();
}

Camera::Camera(unsigned int width, unsigned int height) {
    updateViewMat();
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    updateProjMat();
}

glm::vec3 Camera::front() const {
    return vecForward;
}

glm::vec3 Camera::right() const {
    return vecRight;
}

glm::mat4 Camera::viewMatrix() const {
    return viewMat;
}

glm::mat4 Camera::projMatrix() const {
    return projMat;
}

void Camera::setPosition(const glm::vec3& pos) {
    position = pos;
    updateViewMat();
}

void Camera::addPosition(const glm::vec3& pos) {
    setPosition(position + pos);
}

void Camera::setRotation(const glm::vec3& rot) {
    rotation = rot;
    updateViewMat();
}

void Camera::addRotation(const glm::vec3& rot) {
    setRotation(rotation + rot);
}

void Camera::setFov(float fov) {
    this->fov = fov;
    updateProjMat();
}

void Camera::setAspectRatio(float aspect) {
    aspectRatio = aspect;
    updateProjMat();
}

void Camera::setNearZ(float nearZ) {
    this->nearZ = nearZ;
    updateProjMat();
}

void Camera::setFarZ(float farZ) {
    this->farZ = farZ;
    updateProjMat();
}
