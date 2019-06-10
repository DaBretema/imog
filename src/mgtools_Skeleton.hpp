#pragma once

#include <set>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "gltools_IO.hpp"
#include "gltools_Math.hpp"
#include "gltools_Camera.hpp"
#include "gltools_Transform.hpp"

#include "mgtools_Motion.hpp"

#include "cpptools_Logger.hpp"

namespace imog {
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

  // Alpha value for linked motions
  float m_linkedAlpha;
  float alphaStep() const;
  Frame transitionedLinkedFrame() const;

  // Last frame taking into count displacement offset
  unsigned int lastFrame() const;

  // Frame step
  void frameCounter();

  // Compute hierarchy of the skeleton based on current frame and motion
  void hierarchy();

  // Verify if motion exist on motion map
  template <typename T>
  bool motionExistsOnMap(const std::unordered_map<std::string, T>& map,
                         const std::string&                        name) const;
  // Check for motion in mix map
  bool mixMotionExists(const std::string& name) const;
  // Check for motion in regular map
  bool motionExists(const std::string& name) const;

  // Compute and draw a bone of a gived joint and its parent
  void drawBone(const std::shared_ptr<Joint>& J) const;

  // Jump from current motion to next motion modifying also
  // the value of the current frame
  void loadNextMotion();


public:
  Skeleton(const std::shared_ptr<imog::Camera>& camera,
           float                                scale = 1.f,
           float                                speed = 1.f);

  ~Skeleton();

  bool                    userInput{true};
  bool                    play;
  float                   speed;
  std::shared_ptr<Camera> camera;
  Transform               transform;
  _IO_FUNC                userFn;

  // Get current motion
  float footHeight() const { return m_currMotion->joints.at(5)->matrix[3].y; }

  // link with the camera
  void toggleCameraFollow();

  // manage speed
  void incSpeed();
  void decSpeed();

  // manage linked motion alpha and steps to lerp
  unsigned int linkedSteps;
  void         incLinkedAlpha();
  void         decLinkedAlpha();

  // Compute displacement to apply on next user input
  float step() const;

  // Run a detached thread for animation process
  void animate();

  // Compute joints models and draw its renderable bone
  void draw() const;

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
             _IO_FUNC repeat  = []() {}) const;
};

} // namespace imog
