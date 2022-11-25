#pragma once
#include <glm/glm.hpp>
#include <utility>
namespace GU
{
    class EditCamera
    {
    public:
        EditCamera();
        EditCamera(float fov, float aspectRatio, float nearClip, float farClip);

        glm::mat4 getProjectionViewMatrix() const { return m_projection * m_viewMatrix; }
        glm::mat4 getProjectionMatrix() const { return m_projection; }
        inline void setViewportSize(float width, float height) { m_viewportWidth = width; m_viewportHeight = height; updateProjection(); }

        const glm::vec3& getPosition() const { return m_position; }

        const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
    public:
        float m_FOV = 45.0f, m_aspectRatio = 1.778f, m_nearClip = 0.1f, m_farClip = 1000.0f;
        float m_pitch = 0.0f, m_yaw = 0.0f;

        float m_viewportWidth = 1280, m_viewportHeight = 720;

        glm::vec3 m_position = { 0.0, 0.0, 0.0 };

        glm::vec3 m_focalPoint = { 0.0, 0.0, 0.0 };
        float m_distance = 10.0;

        glm::vec2 m_initialMousePosition = { 0.0, 0.0 };

        glm::mat4 m_viewMatrix;
        glm::mat4 m_projection;

        void updateView();
        void updateProjection();

        std::pair<float, float> panSpeed() const;
        void mousePan(const glm::vec2& delta);
        void mouseRotate(const glm::vec2& delta);

        void mouseZoom(float delta);
        float zoomSpeed() const;

        glm::quat getOrientation() const;
        glm::vec3 getForwardDirection() const;
        glm::vec3 getRightDirection() const;
        glm::vec3 getUpDirection() const;
        glm::vec3 calculatePosition();
    };
}