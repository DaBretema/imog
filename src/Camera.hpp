#pragma once

#include <memory>

#include <dac/Logger.hpp>

#include "Math.hpp"
#include "Transform.hpp"

#include "IO.hpp"
#include "Renderable.hpp"


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
  float m_multSpeed;
  float m_pitch;
  float m_yaw;

  bool m_following;
  bool m_centeredOnTarget;


public:
  Transform                  transform;
  std::shared_ptr<Transform> target;

  Camera(const glm::vec3& pos,
         float            speed = 1.f,
         float            pitch = 0.f,
         float            yaw   = 0.f,
         float            fov   = 0.5f);

  // Getter for view
  glm::mat4 view() const;

  // Getter for proj
  glm::mat4 proj() const;

  // Getter for viewproj
  glm::mat4 viewproj() const;

  // G/Setter for speed
  float speed() const;
  void  speed(float newSpeed);


  // // Attach a renderable object to follow it
  // void attach(const std::shared_ptr<glm::vec3>& objToFollow = nullptr);

  // // Detach a previously attached renderable object
  // void detach();

  // MultSpeed multiply m_speed by passed factor
  void multSpeed(float factor);

  // Move the camera position based on its local axis
  // void move(CamDir dir);

  // Rotate the camera over X axis
  // void rotateX(float angle);

  // Rotate the camera over Y axis
  // void rotateY(float angle);

  // Modify camera fov (a.k.a. zoom)
  void zoom(float variation);

  /// Compute m_proj and m_view per frame
  void frame();
};

} // namespace brave
