#include "camera.h"
#include <cmath>

Camera::Camera()
    : m_position(0.0f, 0.0f, 5.0f),
      m_target(0.0f, 0.0f, 0.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_fov(60.0f * 3.14159f / 180.0f),
      m_aspectRatio(4.0f / 3.0f),
      m_nearPlane(0.1f),
      m_farPlane(100.0f),
      m_viewDirty(true),
      m_projectionDirty(true) {
}

Camera::Camera(const Vec3& position, const Vec3& target, const Vec3& up,
               float fov, float aspectRatio, float nearPlane, float farPlane)
    : m_position(position),
      m_target(target),
      m_up(up),
      m_fov(fov),
      m_aspectRatio(aspectRatio),
      m_nearPlane(nearPlane),
      m_farPlane(farPlane),
      m_viewDirty(true),
      m_projectionDirty(true) {
}

const Matrix4x4& Camera::getViewMatrix() {
    if (m_viewDirty) {
        updateViewMatrix();
        m_viewDirty = false;
    }
    return m_viewMatrix;
}

const Matrix4x4& Camera::getProjectionMatrix() {
    if (m_projectionDirty) {
        updateProjectionMatrix();
        m_projectionDirty = false;
    }
    return m_projectionMatrix;
}

Matrix4x4 Camera::getViewProjectionMatrix() {
    return getProjectionMatrix() * getViewMatrix();
}

void Camera::updateViewMatrix() {
    m_viewMatrix = Matrix4x4::lookAt(m_position, m_target, m_up);
}

void Camera::updateProjectionMatrix() {
    m_projectionMatrix = Matrix4x4::perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::moveForward(float distance) {
    Vec3 direction = (m_target - m_position).normalized();
    m_position = m_position + direction * distance;
    m_target = m_target + direction * distance;
    m_viewDirty = true;
}

void Camera::moveRight(float distance) {
    Vec3 direction = (m_target - m_position).normalized();
    Vec3 right = direction.cross(m_up).normalized();
    m_position = m_position + right * distance;
    m_target = m_target + right * distance;
    m_viewDirty = true;
}

void Camera::moveUp(float distance) {
    m_position = m_position + m_up * distance;
    m_target = m_target + m_up * distance;
    m_viewDirty = true;
}

void Camera::rotateYaw(float angle) {
    Vec3 direction = m_target - m_position;

    Matrix4x4 rotation = Matrix4x4::rotationY(angle);
    Vec4 rotatedDir = rotation * Vec4(direction, 0.0f);
    m_target = m_position + Vec3(rotatedDir.x, rotatedDir.y, rotatedDir.z);

    m_viewDirty = true;
}

void Camera::rotatePitch(float angle) {
    Vec3 direction = m_target - m_position;
    Vec3 right = direction.cross(m_up).normalized();

    float cosA = std::cos(angle);
    float sinA = std::sin(angle);

    Vec3 rotatedDir = Vec3(
        direction.x,
        direction.y * cosA - direction.z * sinA,
        direction.y * sinA + direction.z * cosA
    );

    m_target = m_position + rotatedDir;

    Vec3 newDirection = (m_target - m_position).normalized();
    m_up = right.cross(newDirection).normalized();

    m_viewDirty = true;
}
