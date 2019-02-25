#include "Skeleton.hpp"

#include "Renderable.hpp"
#include "Loader.hpp"
#include "IO.hpp"
#include "Core.hpp"
#include "Settings.hpp"

namespace brave {

Skeleton::Skeleton(float scale)
    : m_model(1.f),
      m_animThread(true),
      // m_rotateLeft(false),
      // m_rotateRight(false),
      // m_moveForward(false),
      // m_moveBackward(false),
      m_play(true),
      m_scale(scale),
      m_currFrame(0u),
      m_currMotion("Idle"),
      ptrPos(std::make_shared<glm::vec3>(0.f)) {
  // Load idle animation //!(define its path on Consts.hpp)
  // and set mechanisms to load it easyly
  // Core::camera->attach(ptrPos);

  // Move
  IO::keyboardAddAction(
      GLFW_KEY_I, IO::kbState::release, [&]() { m_move = 0; });
  IO::keyboardAddAction(GLFW_KEY_I, IO::kbState::press, [&]() { m_move = 1; });
  IO::keyboardAddAction(GLFW_KEY_I, IO::kbState::repeat, [&]() { m_move = 2; });
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
          if (joint->name == "Root") joint->model = m_model;
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


      // auto angle = -glm::angle(-Core::camera->frontY(), this->front()) * 2.f;
      // dInfo("angle: {}", angle);
      // auto angle = glm::angle(Core::camera->frontY(), this->front());



      // *ptrPos        = m_model[3].xyz();
      // auto radCamYaw = glm::radians(-Core::camera->yaw() * 2.f);

      // m_model = glm::translate(glm::mat4(1.f), *ptrPos);
      // m_model = glm::rotate(m_model, radCamYaw, Math::unitVecY);
      // // Core::camera->pos(currPos + this->front() * 3.5f +
      //                   Settings::mainCameraPos);

      // User input reply
      if (m_move == 1 || m_move == 2)
        m_model = glm::translate(m_model, -this->front() * step);

      hierarchy();
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
    auto vJ             = glm::normalize(JPos - JParentPos);
    auto vB2            = bonePos + glm::vec3(0, .5f, 0);
    auto vB1            = bonePos - glm::vec3(0, .5f, 0);
    auto vB             = glm::normalize(vB2 - vB1);
    BONE->transform.rot = glm::rotate(
        BONE->transform.rot, glm::angle(vB, vJ), glm::cross(vB, vJ));
    // S
    auto boneSize = glm::vec3{1.f, glm::distance(JPos, JParentPos) * 0.5f, 1.f};
    BONE->transform.scl = boneSize;
    // Draw
    Renderable::getByName("Bone")->draw();
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
