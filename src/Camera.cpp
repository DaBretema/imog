
#include "../incl/Camera.hpp"

namespace BRAVE {

Camera::Camera(const glm::vec3& pos,
               float            speed,
               float            fov,
               float            pitch,
               float            yaw)
    : m_pos(pos),
      m_fov(fov),
      m_speed(speed),
      m_pitch(pitch),
      m_yaw(yaw),
      m_target(nullptr),
      m_following(false),
      m_centeredOnTarget(false) {}



// View
glm::mat4 Camera::view() const { return m_view; }
glm::mat4 Camera::proj() const { return m_proj; }
glm::mat4 Camera::viewproj() const { return m_viewproj; }

// Pos
glm::vec3 Camera::pos() const { return m_pos; }
void      Camera::pos(const glm::vec3& newPos) { m_pos = newPos; }



// Attach a renderable object to follow it
void Camera::attach(const std::shared_ptr<Renderable>& target) {
  if (target != nullptr) {
    m_target = target;
    m_pos    = glm::vec3{0.f};
  }
}

// Get position of the current target
glm::vec3 Camera::targetPos() {
  if (m_target != nullptr) {
    m_following = true;
    return m_target->model()[3].xyz();
  }
  m_following = false;
  return glm::vec3{0.f};
}

// Detach a previously attached renderable object
void Camera::detach() {
  auto tp = targetPos();
  tp.y    = m_pos.y;
  m_pos   = tp;

  m_following        = false;
  m_centeredOnTarget = false;
  m_target           = nullptr;
}


// Modify camera speed
void Camera::multSpeed(float factor) { m_speed *= factor; }

// Define camera moves (tricked for Orbital)
void Camera::move(CamDir dir) {

  float xSpeedMod = (m_following) ? 0.1f : 1.f;

  switch (dir) {
    case CamDir::front: m_pos.z -= m_speed; break;
    case CamDir::back: m_pos.z += m_speed; break;
    case CamDir::right: m_pos.x += xSpeedMod * m_speed; break;
    case CamDir::left: m_pos.x -= xSpeedMod * m_speed; break;
    case CamDir::up: m_pos.y += m_speed; break;
    case CamDir::down: m_pos.y -= m_speed; break;

    default: DacLog_ERR("Invalid camera move"); break;
  }
}

// Define camera rotation over X axis (unused for Orbital)
void Camera::rotateX(float angle) {
  if (!m_following) { return; }
  m_pitch -= angle * m_speed;
  if (m_pitch > 89.0f) { m_pitch = 89.0f; }
  if (m_pitch < -89.0f) { m_pitch = -89.0f; }
}
// Define camera rotation over Y axis (unused for Orbital)
void Camera::rotateY(float angle) {
  if (!m_following) { return; }
  m_yaw += angle * m_speed;
}


// Radian based zoom == fov variation
void Camera::zoom(float variation) { m_fov += glm::radians(variation); }

// Define what to make every frame to update the viewport
void Camera::frame() {

  if (!m_following) { // =========================================== FPS ======

    auto      xRads = glm::radians(m_pitch);
    auto      yRads = glm::radians(m_yaw);
    glm::vec3 newFront;

    newFront.x = cosf(yRads) * cosf(xRads);
    newFront.y = sinf(xRads);
    newFront.z = sinf(yRads) * cosf(xRads);

    m_front = glm::normalize(newFront);
    m_right = glm::normalize(glm::cross(m_front, Math::VEC_Y));
    m_up    = glm::normalize(glm::cross(m_right, m_front));

    m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);

  } else { // ==================================================== ORBITAL ====

    auto target = targetPos();
    if (m_centeredOnTarget) { target.y = m_pos.y; }
    m_centeredOnTarget = true; // Allow vertical move of the camera

    float posX   = sin(m_pos.x) * m_pos.z;
    float posZ   = cos(m_pos.x) * m_pos.z;
    auto  newPos = glm::vec3(posX, 0.f, posZ) + target;

    m_view = glm::lookAt(newPos, target, Math::VEC_Y);
  }

  m_proj     = glm::perspective(m_fov, IO::windowAspectRatio(), m_near, m_far);
  m_viewproj = m_proj * m_view;
}

} // namespace BRAVE
