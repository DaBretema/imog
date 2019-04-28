#pragma once

#include <set>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "IO.hpp"
#include "Math.hpp"
#include "Camera.hpp"
#include "Motion.hpp"
#include "Transform.hpp"

namespace brave {
class Skeleton {

private:
  std::shared_ptr<Camera> m_camera;

  // enum struct directions { F = 8, B = 4, L = 2, R = 1 };
  // const std::set<int> m_validMoves = {1, 2, 4, 8, 9, 10, 5, 6};

  bool           m_animThread;
  std::string    m_nextMotion;
  std::once_flag m_animationOnceFlag;

  float        m_scale;
  unsigned int m_currFrame;
  int          m_lastFrame;

  // The name of current motion
  // std::string m_currMotion;
  std::shared_ptr<Motion> m_currMotion;

  // Transitions between motions are computed as new motions,
  // as new motions come in map.
  std::unordered_map<std::string, std::shared_ptr<Motion>> m_motions;

public:
  bool      play;
  Transform transform;


  // * ----------------------
  // * Helpers
  // * ----------------------

private:
  // Compute displacement to apply on next user input
  float step();

  // Compute translation displacement per component to apply on next user input
  glm::vec3 tStep3();
  // Compute rotation displacement per component to apply on next user input
  glm::vec3 rStep3();

  // Verify if motion exist on motion map
  bool motionExists(const std::string& dest);


  // * ----------------------
  // * Animation
  // * ----------------------

private:
  // Compute hierarchy of the skeleton based on current frame and motion
  void hierarchy();

  // Frame step
  void frameStep();

public:
  // Run a detached thread for animation process
  void animate();


  // * ----------------------
  // * Draw
  // * ----------------------

private:
  // Compute and draw a bone of a gived joint and its parent
  void drawBone(const std::shared_ptr<Joint>& J);

public:
  // Compute joints models and draw its renderable bone
  void draw();


  // * ----------------------
  // * Motions setup
  // * ----------------------

private:
  // Modify current motion (intern call)
  //-> Always lowercase
  void _setMotion(const std::string& dest);

public:
  // Modify current motion (user call)
  //-> Always lowercase
  void setMotion(const std::string& dest);
  // Add motions to skeleton motion map
  //-> The motion name is converted to lowercase
  void addMotion(const std::shared_ptr<Motion> motion);


  // * ----------------------
  // * Object
  // * ----------------------

public:
  ~Skeleton();
  Skeleton(const std::shared_ptr<brave::Camera>& camera, float scale = 1.f);

  // Define actions on key state
  void onKey(int key, _IO_FUNC press = []() {}, _IO_FUNC release = []() {});
};

} // namespace brave
