#include "Camera.hpp"

#include "IO.hpp"
#include "Settings.hpp"
#include "Logger.hpp"
#include <mutex>
#include <cstdlib>

namespace brave {

// ====================================================================== //
// ====================================================================== //
// Param constructor
// ====================================================================== //

Camera::Camera(float speed, float fov)
    : m_fov(fov), m_centeredOnTarget(false), speed(speed), cinemaLike(false) {

  pivot.rot = glm::vec3(Settings::mainCameraRot, 0.0f);
}

Camera::~Camera() { this->target.reset(); }

// ====================================================================== //
// ====================================================================== //
// Getters for view, proj and viewproj
// ====================================================================== //

glm::mat4 Camera::view() const { return m_view; }
glm::mat4 Camera::proj() const { return m_proj; }
glm::mat4 Camera::viewproj() const { return m_viewproj; }

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
  static glm::vec3 modY;

  if (this->target) {
    // Get center of the skeleton
    if (!m_centeredOnTarget) {
      m_centeredOnTarget = true;
      modY               = Math::unitVecY * this->target->pos.y;
    }
    // Follow on XZ
    pivot.pos = (this->target->pos * Math::vecXZ) + modY;

    // Cinema
    if (this->cinemaLike) { pivot.rot.y += 0.35f; }

  } else {
    m_centeredOnTarget = false;
  }

  auto modZ = pivot.front() * Settings::mainCameraPos.z;
  auto eye  = pivot.pos - modZ;

  auto modX = (this->cinemaLike && this->target)
                  ? pivot.right() * Settings::mainCameraPos.x
                  : Math::nullVec;

  m_proj     = glm::perspective(m_fov, IO::windowAspectRatio(), m_near, m_far);
  m_view     = glm::lookAt(eye, pivot.pos + modX, Math::unitVecY);
  m_viewproj = m_proj * m_view;

} // namespace brave

} // namespace brave
