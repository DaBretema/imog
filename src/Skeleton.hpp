#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>

#include <dac/Async.hpp>
#include <dac/Logger.hpp>

#include "Math.hpp"
#include "Camera.hpp"
#include "Transform.hpp"


namespace brave {
class Skeleton {

public:
  struct Joint {
    std::string            name{""};
    glm::vec3              offset{0.f};
    glm::mat4              transformAsMatrix{1.f};
    std::shared_ptr<Joint> parent;
    std::shared_ptr<Joint> endsite;
    Joint(const std::string& name, std::shared_ptr<Joint> parent)
        : name(name), parent(parent) {}
  };
  struct Frame {
    std::vector<glm::vec3> rotations;
    glm::vec3              translation;
  };
  struct Motion {
    std::string                         name;
    std::vector<std::shared_ptr<Joint>> joints;
    std::vector<Frame>                  frames;
    float                               timeStep;
  };

private:
  enum struct directions { front = 8, back = 4, left = 2, right = 1 };

  std::mutex m_mutex;

  bool           m_animThread;
  std::once_flag animationOnceFlag;

  float        m_scale;
  unsigned int m_currFrame;

  std::shared_ptr<Camera> m_camera;

  // For intermediate steps between two motions, we create two new motions
  // one from animation 1 to animation 2 and viceversa, storing them
  // in that map, setting the key as "idxFrom_idxTo" (for both cases)
  // to easy swith from one to other
  std::unordered_map<std::string, std::shared_ptr<Skeleton::Motion>> m_motions;
  std::string m_currMotion;

  // Animation steps
  float step();
  // void  hierarchy();
  void hierarchy(const std::string& motionName, unsigned int frame);
  void drawBone(const std::shared_ptr<Joint>& J);

public:
  Transform transform;
  int       move;
  bool      play;


  Skeleton(const std::shared_ptr<brave::Camera>& camera, float scale = 1.f);
  ~Skeleton();

  // Add motions to skeleton motion map
  void addMotion(const std::string& name, const std::string& file);

  // Modify current motion
  void currMotion(const std::string& motionName);


  /* Movement will be defined by the following truth table.
      F B L R / Val
                                                          - Stillness cases
      0 0 0 0 / 0  : No input             : Ø
      0 0 1 1 / 3  : Right and Left       : Ø
      1 1 0 0 / 12 : Forward and Backward : Ø
                                                          - Basic moves
      0 0 0 1 / 1  : Right    : ➡
      0 0 1 0 / 2  : Left     : ⬅
      0 1 0 0 / 4  : Backward : ↓
      1 0 0 0 / 8  : Forward  : ↑
                                                          - Combo moves
      1 0 0 1 / 9  : Right + Forward  : ↗
      1 0 1 0 / 10 : Left + Forward   : ↖
      0 1 0 1 / 5  : Right + Backward : ↘
      0 1 1 0 / 6  : Left + Backward  : ↙
  */
  // Move forward
  void moveFront(bool active);
  // Move to the right
  void moveRight(bool active);
  // Move to the left
  void moveLeft(bool active);
  // Move backward
  void moveBack(bool active);

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
        dInfo("TriggerKey {} is already defined.", triggerKey);
        return;
      }
    }
    // Check for motion name repetition
    if (m_motions.count(newMotion->name) > 0) {
      dInfo("Motion {} is already defined.", newMotion->name);
      return;
    }

    for (auto& motion : m_motions) {
		// Do something with newMotion
    }

    m_motions.insert({newMotion->name, {triggerKey, newMotion}});
  }
*/
