#pragma once

#include <memory>



#include "Math.hpp"
#include "Transform.hpp"


namespace brave {

class Camera {
  static constexpr float m_near = 0.0001f;
  static constexpr float m_far  = 10000.f;

private:
  glm::mat4 m_view;
  glm::mat4 m_proj;
  glm::mat4 m_viewproj;

  float m_fov;
  float m_speed;

  float m_yaw;
  float m_pitch;
  float m_multSpeed;

  bool m_following;
  bool m_centeredOnTarget;

public:
  Transform                  pivot;
  std::shared_ptr<Transform> target;
  Transform                  transform;

  Camera(float speed = 1.f, float fov = 0.5f);
  ~Camera();

  // Getter for view
  glm::mat4 view() const;
  // Getter for proj
  glm::mat4 proj() const;
  // Getter for viewproj
  glm::mat4 viewproj() const;

  // G/Setter for speed
  float speed() const;
  void  speed(float newSpeed);
  // MultSpeed multiply m_speed by passed factor
  void multSpeed(float factor);

  // Modify camera fov (a.k.a. zoom)
  void zoom(float variation);

  /// Compute m_proj and m_view per frame
  void frame();
};

} // namespace brave
