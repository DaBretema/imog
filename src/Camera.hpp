#pragma once

#include <memory>

#include <dac/Logger.hpp>

#include "Math.hpp"

#include "IO.hpp"
#include "Renderable.hpp"


namespace brave {

enum struct CamDir { front, back, right, left, up, down };

class Camera {
  static constexpr float m_near = 0.0001f;
  static constexpr float m_far  = 10000.f;

private:
  glm::mat4 m_view;
  glm::mat4 m_proj;
  glm::mat4 m_viewproj;

  glm::vec3 m_pos;
  glm::vec3 m_front;
  glm::vec3 m_right;
  glm::vec3 m_up;

  float m_fov;
  float m_speed;
  float m_multSpeed;
  float m_pitch;
  float m_yaw;

  std::shared_ptr<Renderable> m_target;
  bool                        m_following;
  bool                        m_centeredOnTarget;

  // Get position of the current target
  glm::vec3 targetPos();


public:
  Camera(const glm::vec3& pos,
         float            speed = 1.f,
         float            fov   = 0.5f,
         float            pitch = 0.f,
         float            yaw   = -90.f);


  // Getter for view
  glm::mat4 view() const;

  // Getter for proj
  glm::mat4 proj() const;

  // Getter for viewproj
  glm::mat4 viewproj() const;

  // G/Setter for pos
  glm::vec3 pos() const;
  void      pos(const glm::vec3& newPos);

  // G/Setter for speed
  float speed() const;
  void  speed(float newSpeed);


  // Attach a renderable object to follow it
  void attach(const std::shared_ptr<Renderable>& objToFollow = nullptr);

  // Detach a previously attached renderable object
  void detach();

  // MultSpeed multiply m_speed by passed factor
  void multSpeed(float factor);

  // Move the camera position based on its local axis
  void move(CamDir dir);

  // Rotate the camera over X axis
  void rotateX(float angle);

  // Rotate the camera over Y axis
  void rotateY(float angle);

  // Modify camera fov (a.k.a. zoom)
  void zoom(float variation);

  /// Compute m_proj and m_view per frame
  void frame();
};

} // namespace brave
