#include "Camera.hpp"

#include "Settings.hpp"

namespace brave {

// ====================================================================== //
// ====================================================================== //
// Param constructor
// ====================================================================== //

Camera::Camera(const glm::vec3& pos,
               float            speed,
               float            pitch,
               float            yaw,
               float            fov)
    : m_fov(fov),
      m_speed(speed),
      m_multSpeed(1.f),
      m_pitch(pitch),
      m_yaw(-90.f + yaw),
      target(nullptr),
      m_following(false),
      m_centeredOnTarget(false) {
  // transform.pos = pos;
  transform.pos = glm::vec3(0, 0, -3.f);
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



// ====================================================================== //
// ====================================================================== //
// Modify camera speed
// ====================================================================== //

void Camera::multSpeed(float factor) { m_multSpeed = factor; }


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


  // auto auxRot = target->rot;
  // auto auxPos = target->pos - glm::vec3(0, 0, -3.f);

  // m_view = glm::mat4(1.f);
  // m_view = glm::translate(m_view, -target->pos);
  // m_view = glm::rotate(m_view, target->rot.y, Math::unitVecY);
  // m_view = glm::translate(m_view, target->pos);


  // Math::translate(auxView, transform.pos);
  // Math::rotateXYZ(auxView, transform.rot);
  // Math::translate(auxView, transform.pos - glm::vec3(0, 0, -3.f));
  // Math::translate(auxView, auxPos);

  m_view =
      glm::lookAt(target->pos - target->front() * -10.f + Math::unitVecY * 10.f,
                  target->pos,
                  Math::unitVecY);
  m_proj = glm::perspective(m_fov, IO::windowAspectRatio(), m_near, m_far);

  m_viewproj = m_proj * m_view;
}

} // namespace brave
