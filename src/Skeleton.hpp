#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>

#include <dac/Async.hpp>
#include <dac/Logger.hpp>

#include "Math.hpp"


namespace brave {


class Skeleton {

  using motion_t = std::shared_ptr<Motion>;

public:
  struct Joint {
    std::string            name;
    glm::vec3              offset;
    std::shared_ptr<Joint> parent;
  };
  struct Frame {
    glm::vec3 translation;
    /*
	Rotations should be stored with channel correction,
	which may differ, between motions (bvh files), so the channel
	should be corrected during bvh file parsing.
  */
    std::vector<glm::vec3> rotations;
  };
  struct Motion {
    std::string        name;
    std::vector<Joint> joints;
    std::vector<Frame> frames;
    float              frameTime;
  };

private:
  glm::mat4      m_model;
  bool           m_animThread;
  std::once_flag animationOnceFlag;

  bool m_rotateLeft;
  bool m_rotateRight;
  bool m_moveForward;
  bool m_moveBackward;

  // For intermediate steps between two motions, we create two new motions
  // one from animation 1 to animation 2 and viceversa, storing them
  // in that map, setting the key as "idxFrom_idxTo" (for both cases)
  // to easy swith from one to other
  std::unordered_map<std::string, motion_t> m_motions;
  std::string                               m_currMotion;
  // Easy wrappers
  auto moName() const { return m_motions.at(m_currMotion)->name; };
  auto moFrames() const { return m_motions.at(m_currMotion)->frames; };
  auto moJoints() const { return m_motions.at(m_currMotion)->joints; };

public:
  Skeleton(const std::string& idlePath);
  ~Skeleton();

  // Run a detached thread for animation process
  void animation() {

    auto skRotation = [&](bool right, float step = 1.f) {
      float dir = (right) ? 1.f : -1.f;
      Math::rotateXYZ(m_model, Math::unitVecY * dir * step);
    };

    auto skMovement = [&](bool forward, float step = 1.f) {
      float dir = (forward) ? 1.f : -1.f;
      Math::translate(m_model, glm::normalize(dir * m_model[2]) * step);
    };

    std::call_once(animationOnceFlag, [&]() {
      dac::Async::periodic(1.f, &m_animThread, [&]() {
        /*
			Movement by user input
			! [KEYBOARD] Add flag for RELEASE or PRESS. (The 2nd parameter)

			1. SETUP
			IO::keyboardAddAction(GLFW_KEY_I, true, [&]() { m_moveForward = true });
			IO::keyboardAddAction(GLFW_KEY_I, false, [&]() { m_moveForward = false });
			IO::keyboardAddAction(GLFW_KEY_K, true, [&]() { m_moveBackward = true });
			IO::keyboardAddAction(GLFW_KEY_K, false, [&]() { m_moveBackward = false });
			IO::keyboardAddAction(GLFW_KEY_J, true, [&]() { m_rotateLeft = true });
			IO::keyboardAddAction(GLFW_KEY_J, false, [&]() { m_rotateLeft = false });
			IO::keyboardAddAction(GLFW_KEY_L, true, [&]() { m_rotateRight = true });
			IO::keyboardAddAction(GLFW_KEY_L, false, [&]() { m_rotateRight = false });

			2. FOR MORE REALISM...
			Right and Left keys, should not rotate, should generate a motion
			like those that we can see in "unity blend tree" and rotate the
			model in that direction and then move forward.
		*/
        if (m_rotateLeft) { skRotation(false); }
        if (m_rotateRight) { skRotation(true); }
        if (m_moveForward) { skMovement(true); }
        if (m_moveBackward) { skMovement(false); }

        // Idle state
        if (!(m_rotateLeft && m_rotateRight && m_moveForward &&
              m_moveBackward)) {}

        // Update hierarchy
        for (const auto& joint : moJoints()) {}
      });
    });
  }
};

Skeleton::Skeleton(const std::string& idlePath) {}

Skeleton::~Skeleton() {}



} // namespace brave


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
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
