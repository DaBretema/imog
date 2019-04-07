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
  enum struct directions { F = 8, B = 4, L = 2, R = 1 };
  const std::set<int> m_validMoves = {1, 2, 4, 8, 9, 10, 5, 6};

  int            m_move;
  bool           m_animThread;
  std::string    m_nextMotion;
  std::once_flag m_animationOnceFlag;

  float        m_scale;
  unsigned int m_currFrame;
  int          m_lastFrame;
  Transform    m_transform;

  // The name of current motion
  std::string m_currMotion;

  // Transitions between motions are computed as new motions,
  // as new motions come in map.
  std::unordered_map<std::string, std::shared_ptr<Motion>> m_motions;

public:
  bool play;


  // * ----------------------
  // * Helpers
  // * ----------------------

private:
  // Compute displacement to apply on next user input
  float step();

  // Compute displacement on 3 axis and get a vec3
  glm::vec3 step3();

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

  // User input movement over root node
  void rootMovement();

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
  void _setMotion(const std::string& dest);

public:
  // Modify current motion (user call)
  void setMotion(const std::string& dest);


  // Add motions to skeleton motion map
  void addMotion(const std::string& name,
                 const std::string& file,
                 bool               makeLoop = true);


  // * ----------------------
  // * Movement
  // * ----------------------

public:
  // Movement will be defined by the following truth table.
  //                            F B L R  |  Value  |     Direction      | Sign
  //                            -------- - ------- - ------------------ - ----
  void moveR(bool active);  //  0 0 0 1  |    1    |  Right             |  ➡
  void moveL(bool active);  //  0 0 1 0  |    2    |  Left              |  ⬅
  void moveB(bool active);  //  0 1 0 0  |    4    |  Backward          |  ↓
  void moveF(bool active);  //  1 0 0 0  |    8    |  Forward           |  ↑
  void moveFR(bool active); //  1 0 0 1  |    9    |  Right + Forward   |  ↗
  void moveFL(bool active); //  1 0 1 0  |    10   |  Left + Forward    |  ↖
  void moveBR(bool active); //  0 1 0 1  |    5    |  Right + Backward  |  ↘
  void moveBL(bool active); //  0 1 1 0  |    6    |  Left + Backward   |  ↙


  // * ----------------------
  // * Object
  // * ----------------------

public:
  Skeleton(const std::shared_ptr<brave::Camera>& camera, float scale = 1.f);
  ~Skeleton();

  // Define actions on key state
  void onKey(int key, _IO_FUNC press = []() {}, _IO_FUNC release = []() {});
};

} // namespace brave
