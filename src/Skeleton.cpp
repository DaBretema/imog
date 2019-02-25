#include "Skeleton.hpp"

#include "Renderable.hpp"
#include "Loader.hpp"
#include "IO.hpp"
#include "Core.hpp"
#include "Settings.hpp"

namespace brave {

Skeleton::Skeleton(float scale)
    : m_animThread(true),
      m_rotateLeft(false),
      m_rotateRight(false),
      m_moveForward(false),
      m_moveBackward(false),
      m_play(true),
      m_scale(scale),
      m_currFrame(0u),
      m_currMotion("Idle") {
  // Load idle animation //!(define its path on Consts.hpp)
  // and set mechanisms to load it easyly
  // Core::camera->attach(ptrPos);
  Core::camera->target = std::shared_ptr<Transform>(&this->transform);

  // Move
  IO::keyboardAddAction(
      GLFW_KEY_I, IO::kbState::release, [&]() { m_move = 0; });
  IO::keyboardAddAction(GLFW_KEY_I, IO::kbState::press, [&]() { m_move = 1; });
  IO::keyboardAddAction(GLFW_KEY_I, IO::kbState::repeat, [&]() { m_move = 2; });
}
Skeleton::~Skeleton() {
  if (!Settings::quiet) dInfo("Skeleton destroyed!");
  // stop();
  m_animThread = false;
}

void Skeleton::setAnimFromBVH(const std::string& name,
                              const std::string& file) {
  m_motions.try_emplace(name, loader::BVH(file));
  m_motions[name]->name = m_currMotion = name;
}

// ====================================================================== //
// ====================================================================== //
// Run a detached thread for animation process
// ====================================================================== //

void Skeleton::animation() {

  std::call_once(animationOnceFlag, [&]() {
    dac::Async::periodic(moTimeStep() * m_scale, &m_animThread, [&]() {
      // Hierarchy data update
      auto hierarchy = [&]() {
        int rotIdx = 0;
        for (const auto& joint : moJoints()) {
          if (joint->name == "Root") joint->model = this->transform.model();
          // 1. Hierarchy of joints
          if (joint->parent) { joint->model = joint->parent->model; }
          Math::translate(joint->model, joint->offset * m_scale);
          // 2. Motion of joints
          Math::rotateXYZ(joint->model, moCurrFrame().rotations.at(rotIdx++));
          // 3. Hierarchy of end-sites
          if (joint->endsite) {
            joint->endsite->model = joint->model;
            Math::translate(joint->endsite->model,
                            joint->endsite->offset * m_scale);
          }
        }
        // Frame counter
        (m_currFrame >= moFrames().size() - 2) ? m_currFrame = 0
                                               : ++m_currFrame;
      };

      // Displacement step
      auto  currFramePos = moCurrFrame().translation;
      auto  nextFramePos = moNextFrame().translation;
      float step         = glm::distance2(nextFramePos, currFramePos);

      // Camera sync

      if (m_move == 1 || m_move == 2) {
        auto camFront       = Core::camera->pivot.front();
        camFront.y          = 0.f;
        this->transform.rot = Core::camera->pivot.rot;
        this->transform.pos += camFront * step;
      }

      hierarchy();

      // Call to action if it's not automatically done
      if (!Settings::pollEvents) { glfwPostEmptyEvent(); }
    });
  });
}

// ====================================================================== //
// ====================================================================== //
// Compute joints models and draw its renderable bone
// ====================================================================== //

void Skeleton::draw() {

  auto bone = [&](const std::shared_ptr<Joint>& J) {
    auto BONE       = Renderable::getByName("Bone");
    auto JPos       = J->model[3].xyz();
    auto JParentPos = J->parent->model[3].xyz();
    // T
    auto bonePos        = (JPos + JParentPos) * 0.5f;
    BONE->transform.pos = bonePos;
    // R
    auto vJ                  = glm::normalize(JPos - JParentPos);
    auto vB2                 = bonePos + glm::vec3(0, .5f, 0);
    auto vB1                 = bonePos - glm::vec3(0, .5f, 0);
    auto vB                  = glm::normalize(vB2 - vB1);
    BONE->transform.rotAngle = glm::angle(vB, vJ);
    BONE->transform.rotAxis  = glm::cross(vB, vJ);
    // S
    auto boneSize = glm::vec3{1.f, glm::distance(JPos, JParentPos) * 0.5f, 1.f};
    BONE->transform.scl = boneSize;
    // Draw
    BONE->draw();
  };


  for (const auto& J : moJoints()) {
    if (!J->parent) continue;

    if (J->name == "Head" && J->endsite) {
      auto head = Renderable::getByName("Monkey");
      // head->transform.pos = J->
      // head->model(J->endsite->model);
      // head->draw();
    }

    bone(J);
    if (J->endsite) bone(J->endsite);
  }
}

} // namespace brave
