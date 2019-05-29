#pragma once

#include <memory>

#include "Math.hpp"
#include "Transform.hpp"


namespace brave {

class Camera {
  static constexpr float m_near = 0.1f;
  static constexpr float m_far  = 1000.f;

private:
  glm::mat4 m_view;
  glm::mat4 m_proj;
  glm::mat4 m_viewproj;

  float m_fov;
  glm::vec3 m_offset;
  bool  m_centeredOnTarget;

public:
  float                      speed;
  Transform                  pivot;
  std::shared_ptr<Transform> target;
  Transform                  transform;
  bool                       cinemaLike;

  Camera(float speed = 1.f, float fov = 0.5f);
  ~Camera();

  // Getter for view
  glm::mat4 view() const;
  // Getter for proj
  glm::mat4 proj() const;
  // Getter for viewproj
  glm::mat4 viewproj() const;

  // Modify camera fov (a.k.a. zoom)
  void zoom(float variation);

  /// Compute m_proj and m_view per frame
  void frame();
};

} // namespace brave
