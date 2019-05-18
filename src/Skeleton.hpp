#pragma once

#include <set>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "IO.hpp"
#include "Math.hpp"
#include "Camera.hpp"
#include "Motion.hpp"
#include "Logger.hpp"
#include "Transform.hpp"

namespace brave {
class Skeleton {

private:
  float          m_scale;
  bool           m_animThread;
  std::once_flag m_animationOnceFlag;

  unsigned int m_currFrame;
  unsigned int m_nextFrame;

  std::shared_ptr<Motion> m_currMotion;
  std::shared_ptr<Motion> m_nextMotion;

  std::unordered_map<std::string, std::shared_ptr<Motion>> m_motions;
  std::unordered_map<std::string, Motion::mixMap>          m_motionMap;


  // Frame step
  void frameCounter();

  // Compute hierarchy of the skeleton based on current frame and motion
  void hierarchy();

  // Compute translation displacement per component to apply on next user input
  glm::vec3 tStep3();

  // Compute rotation displacement per component to apply on next user input
  glm::vec3 rStep3();

  // Verify if motion exist on motion map
  template <typename T>
  bool motionExistsOnMap(const std::unordered_map<std::string, T>& map,
                         const std::string&                        name);
  // Check for motion in mix map
  bool mixMotionExists(const std::string& name);
  // Check for motion in regular map
  bool motionExists(const std::string& name);

  // Compute and draw a bone of a gived joint and its parent
  void drawBone(const std::shared_ptr<Joint>& J);

  // Jump from current motion to next motion modifying also
  // the value of the current frame
  void loadNextMotion();

  // Modify current motion (intern call)
  void _setMotion(const std::string& dest);



public:
  Skeleton(const std::shared_ptr<brave::Camera>& camera,
           float                                 scale = 1.f,
           float                                 speed = 1.f);

  ~Skeleton();

  bool                    play;
  float                   speed;
  std::shared_ptr<Camera> camera;
  Transform               transform;
  glm::vec3               allowedRots;
  glm::vec3               allowedTrans;

  // Compute displacement to apply on next user input
  float step();

  // Run a detached thread for animation process
  void animate();

  // Compute joints models and draw its renderable bone
  void draw();

  // Modify current motion (user call)
  //-> Always lowercase
  void setMotion(const std::string& dest);

  // Add motions to skeleton motion map
  //-> The motion name is converted to lowercase
  void addMotion(const std::shared_ptr<Motion> motion);

  // Define actions on key state
  void onKey(int      key,
             _IO_FUNC press   = []() {},
             _IO_FUNC release = []() {},
             _IO_FUNC repeat  = []() {});
};

} // namespace brave
