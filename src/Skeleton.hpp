#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>

#include <dac/Async.hpp>
#include <dac/Logger.hpp>

#include "Math.hpp"
#include "Transform.hpp"


namespace brave {
class Skeleton {

public:
  struct Joint {
    std::string            name{""};
    glm::mat4              model{1.f};
    glm::vec3              offset{0.f};
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
  bool           m_animThread;
  std::once_flag animationOnceFlag;

  int m_moving = 0;
  int m_move   = -1;

  bool m_rotateLeft;
  bool m_rotateRight;
  bool m_moveForward;
  bool m_moveBackward;

  bool         m_play;
  float        m_scale;
  unsigned int m_currFrame;

  // For intermediate steps between two motions, we create two new motions
  // one from animation 1 to animation 2 and viceversa, storing them
  // in that map, setting the key as "idxFrom_idxTo" (for both cases)
  // to easy swith from one to other
  std::unordered_map<std::string, std::shared_ptr<Skeleton::Motion>> m_motions;
  std::string m_currMotion;

  // Easy motion wrappers
  auto moTimeStep() const { return m_motions.at(m_currMotion)->timeStep; };
  auto moName() const { return m_motions.at(m_currMotion)->name; };
  auto moFrames() const { return m_motions.at(m_currMotion)->frames; };
  auto moCurrFrame() const { return moFrames().at(m_currFrame); }
  auto moNextFrame() const { return moFrames().at(m_currFrame + 1); }
  auto moJoints() const { return m_motions.at(m_currMotion)->joints; };

public:
  Transform transform;
  Skeleton(float scale = 1.f);
  ~Skeleton();

  // tmp
  void setAnimFromBVH(const std::string& name, const std::string& file);

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
