#include "Camera.hpp"

#include "IO.hpp"
#include "Settings.hpp"

namespace brave {

// ====================================================================== //
// ====================================================================== //
// Param constructor
// ====================================================================== //

Camera::Camera(float speed, float fov)
    : m_fov(fov),
      m_speed(speed),
      m_yaw(-90.0f),
      m_pitch(0.0f),
      m_multSpeed(1.0f),
      m_following(false),
      m_centeredOnTarget(false),
      target(nullptr) {

  pivot.rot = glm::vec3(Settings::mainCameraRot, 0.0f);
}

// ====================================================================== //
// ====================================================================== //
// Getters for view, proj and viewproj
// ====================================================================== //

glm::mat4 Camera::view() const { return m_view; }
glm::mat4 Camera::proj() const { return m_proj; }
glm::mat4 Camera::viewproj() const { return m_viewproj; }

// ====================================================================== //
// ====================================================================== //
// G/Setter for speed
// ====================================================================== //

float Camera::speed() const { return m_speed * m_multSpeed; }
void  Camera::speed(float newSpeed) { m_speed = newSpeed; }
void  Camera::multSpeed(float factor) { m_multSpeed = factor; }

// ====================================================================== //
// ====================================================================== //
// Radian based zoom == fov variation
// ====================================================================== //

void Camera::zoom(float variation) { m_fov += glm::radians(variation); }

// ====================================================================== //
// ====================================================================== //
// Define what to make every frame to update the viewport
// ====================================================================== //

void Camera::frame() {
  if (target) { pivot.pos = target->pos; }

  auto modZ = pivot.front() * Settings::mainCameraPos.z;
  // auto modY = Math::unitVecY * Settings::mainCameraPos.y;
  auto eye = pivot.pos - modZ; //+modY;

  m_proj     = glm::perspective(m_fov, IO::windowAspectRatio(), m_near, m_far);
  m_view     = glm::lookAt(eye, pivot.pos, Math::unitVecY);
  m_viewproj = m_proj * m_view;
}

} // namespace brave
