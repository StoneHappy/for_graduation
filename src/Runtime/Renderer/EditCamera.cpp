#include "EditCamera.h"
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<algorithm>
#include <Global/CoreContext.h>
#include <QString>
namespace GU
{
    EditCamera::EditCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov), m_aspectRatio(aspectRatio), m_nearClip(nearClip), m_farClip(farClip), m_projection(glm::perspective((float)glm::radians(fov), aspectRatio, nearClip, farClip))
    {
        updateView();
        updateProjection();
    }

    EditCamera::EditCamera()
        : m_FOV(45.0), m_aspectRatio(1.778f), m_nearClip(0.1f), m_farClip(1000.0f), m_projection(glm::perspective((float)glm::radians(45.0f), 1.778f, 0.1f, 1000.0f))
    {
        updateView();
    }

    glm::quat EditCamera::getOrientation() const
    {
        return glm::quat(glm::vec3(-m_pitch, -m_yaw, 0.0f));
    }

    glm::vec3 EditCamera::getForwardDirection() const
    {
        return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 EditCamera::getRightDirection() const
    {
        return glm::rotate(getOrientation(), glm::vec3(1.0, 0.0, 0.0));
    }

    glm::vec3 EditCamera::getUpDirection() const
    {
        return glm::rotate(getOrientation(), glm::vec3(0.0, 1.0, 0.0));
    }

    void EditCamera::updateView()
    {
        m_position = calculatePosition();

        glm::quat orientation = getOrientation();

        m_viewMatrix = glm::translate(glm::mat4(1), m_position) * glm::toMat4(orientation);;
        m_viewMatrix = glm::inverse(m_viewMatrix);
    }

    void EditCamera::updateProjection()
    {
        m_aspectRatio = g_CoreContext.g_winWidth / g_CoreContext.g_winHeight;
        m_projection = glm::perspective(m_FOV, m_aspectRatio, m_nearClip, m_farClip);
    }

    glm::vec3 EditCamera::calculatePosition()
    {
        return m_focalPoint - getForwardDirection() * m_distance;
    }

    std::pair<float, float> EditCamera::panSpeed() const
    {
        float x = std::min(m_viewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(m_viewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }


    void EditCamera::mousePan(const glm::vec2& delta)
    {
        auto [xSpeed, ySpeed] = panSpeed();
        m_focalPoint += -getRightDirection() * delta.x * xSpeed * m_distance;
        m_focalPoint += getUpDirection() * delta.y * ySpeed * m_distance;
    }

    void EditCamera::mouseRotate(const glm::vec2& delta)
    {
        float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
        m_yaw += yawSign * delta.x * 0.8f;
        m_pitch += delta.y * 0.8f;
    }

    void EditCamera::mouseZoom(float delta)
    {
        m_distance -= delta * zoomSpeed();
        if (m_distance < 1.0f)
        {
            //m_focalPoint += getForwardDirection();
            m_distance = 1.0f;
        }
    }

    float EditCamera::zoomSpeed() const
    {
        float distance = m_distance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 30.0f); // max speed = 100
        return speed;
    }
}