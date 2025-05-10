#pragma once

#include "vector.h"
#include "matrix.h"

class Camera {
public:
    Camera();
    Camera(const Vec3& position, const Vec3& target, const Vec3& up,
           float fov, float aspectRatio, float nearPlane, float farPlane);

    void setPosition(const Vec3& position) { m_position = position; m_viewDirty = true; }
    void setTarget(const Vec3& target) { m_target = target; m_viewDirty = true; }
    void setUp(const Vec3& up) { m_up = up; m_viewDirty = true; }
    void setFOV(float fov) { m_fov = fov; m_projectionDirty = true; }
    void setAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; m_projectionDirty = true; }
    void setNearPlane(float nearPlane) { m_nearPlane = nearPlane; m_projectionDirty = true; }
    void setFarPlane(float farPlane) { m_farPlane = farPlane; m_projectionDirty = true; }

    const Vec3& getPosition() const { return m_position; }
    const Vec3& getTarget() const { return m_target; }
    const Vec3& getUp() const { return m_up; }
    float getFOV() const { return m_fov; }
    float getAspectRatio() const { return m_aspectRatio; }
    float getNearPlane() const { return m_nearPlane; }
    float getFarPlane() const { return m_farPlane; }

    const Matrix4x4& getViewMatrix();
    const Matrix4x4& getProjectionMatrix();
    Matrix4x4 getViewProjectionMatrix();

    void moveForward(float distance);
    void moveRight(float distance);
    void moveUp(float distance);
    void rotateYaw(float angle);
    void rotatePitch(float angle);

private:
    Vec3 m_position;
    Vec3 m_target;
    Vec3 m_up;

    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;

    Matrix4x4 m_viewMatrix;
    Matrix4x4 m_projectionMatrix;

    bool m_viewDirty;
    bool m_projectionDirty;

    void updateViewMatrix();
    void updateProjectionMatrix();
};
