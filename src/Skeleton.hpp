#pragma once

#include <set>
// #include <vector>
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

  std::mutex _modifyDestMotion;

private:
  enum struct directions { F = 8, B = 4, L = 2, R = 1 };
  const std::set<int> m_validMoves = {1, 2, 4, 8, 9, 10, 5, 6};

  bool           m_animThread;
  std::once_flag animationOnceFlag;

  float        m_scale;
  unsigned int m_currFrame;
  int          m_lastFrame = -1;

  std::shared_ptr<Camera> m_camera;

  // For intermediate steps between two motions, we create two new motions
  // one from animation 1 to animation 2 and viceversa, storing them
  // in that map, setting the key as "From_To" (for both cases)
  // to easy swith from one to other
  std::unordered_map<std::string, std::shared_ptr<Motion>> m_motions;
  std::string                                              m_currMotion;

  // Compute hierarchy of the skeleton based on current frame and motion
  void hierarchy(const std::string& motionName, unsigned int frame);

  // Compute displacement to apply on next user input
  float step();

  // Compute displacement on 3 axis and get a vec3
  glm::vec3 step3();

  // Compute and draw a bone of a gived joint and its parent
  void drawBone(const std::shared_ptr<Joint>& J);


public:
  Transform transform;
  int       move;
  bool      play;

  Skeleton(const std::shared_ptr<brave::Camera>& camera, float scale = 1.f);
  ~Skeleton();

  // Define actions on key state
  void onKey(int key, _IO_FUNC press = []() {}, _IO_FUNC release = []() {});

  // Add motions to skeleton motion map
  void addMotion(const std::string& name,
                 const std::string& file,
                 loopMode           lm = loopMode::firstFrame);

  // Modify current motion
  void currMotion(const std::string& motionName, unsigned int targetFrame = 0);

  /* Movement will be defined by the following truth table.
      F B L R / Val
      0 0 0 1 / 1  : Right            : ➡
      0 0 1 0 / 2  : Left             : ⬅
      0 1 0 0 / 4  : Backward         : ↓
      1 0 0 0 / 8  : Forward          : ↑
      1 0 0 1 / 9  : Right + Forward  : ↗
      1 0 1 0 / 10 : Left + Forward   : ↖
      0 1 0 1 / 5  : Right + Backward : ↘
      0 1 1 0 / 6  : Left + Backward  : ↙
  */
  void moveF(bool active);
  void moveR(bool active);
  void moveL(bool active);
  void moveB(bool active);

  // Run a detached thread for animation process
  void animation();

  // Compute joints models and draw its renderable bone
  void draw();
};

} // namespace brave


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/*
  * SETUP User input for movement
    ! [KEYBOARD] Add flag for RELEASE or PRESS. (The 2nd parameter)
    IO::keyboardAddAction(GLFW_KEY_I, true, [&]() { m_moveForward = true });
    IO::keyboardAddAction(GLFW_KEY_I, false, [&]() { m_moveForward = false });
    IO::keyboardAddAction(GLFW_KEY_K, true, [&]() { m_moveBackward = true });
    IO::keyboardAddAction(GLFW_KEY_K, false, [&]() { m_moveBackward = false });
    IO::keyboardAddAction(GLFW_KEY_J, true, [&]() { m_rotateLeft = true });
    IO::keyboardAddAction(GLFW_KEY_J, false, [&]() { m_rotateLeft = false });
    IO::keyboardAddAction(GLFW_KEY_L, true, [&]() { m_rotateRight = true });
    IO::keyboardAddAction(GLFW_KEY_L, false, [&]() { m_rotateRight = false });
  * FOR MORE REALISM...
    Right and Left keys, should not rotate, should generate a motion
    like those that we can see in "unity blend tree" and rotate the
    model in that direction and then move forward.
 */



/*
// ? Maybe user input mechanism to switch between animation is too complex.
// ? By now, let it for next iteration.

  std::vector<motion_t> m_motions;
  std::unordered_map<std::string, std::pair<int, motion_t>> m_motions;

  void addMotion(int triggerKey, motion_t newMotion) {
    // Check for trigger key repetition
    for (const auto& motion : m_motions) {
      if (motion.second.first == triggerKey) {
        LOGD("TriggerKey {} is already defined.", triggerKey);
        return;
      }
    }
    // Check for motion name repetition
    if (m_motions.count(newMotion->name) > 0) {
      LOGD("Motion {} is already defined.", newMotion->name);
      return;
    }

    for (auto& motion : m_motions) {
		// Do something with newMotion
    }

    m_motions.insert({newMotion->name, {triggerKey, newMotion}});
  }
*/
