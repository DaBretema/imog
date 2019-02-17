#include "Skeleton.hpp"

#include "Renderable.hpp"

#include "Loader.hpp"

namespace brave {


Skeleton::Skeleton(float scale)
    : m_model(1.f),
      m_animThread(true),
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
}

void Skeleton::setAnimFromBVH(const std::string& name,
                              const std::string& file) {
  m_motions.try_emplace(name, loader::BVH(file));
  m_motions[name]->name = name;
  m_currMotion          = name;
}

// ====================================================================== //
// ====================================================================== //
// Run a detached thread for animation process
// ====================================================================== //

void Skeleton::animation() {

  auto skRotation = [&](bool right, float step = 1.f) {
    float dir = (right) ? 1.f : -1.f;
    Math::rotateXYZ(m_model, Math::unitVecY * dir * step);
  };

  auto skMovement = [&](bool forward, float step = 1.f) {
    float dir = (forward) ? 1.f : -1.f;
    Math::translate(m_model, glm::normalize(dir * m_model[2]) * step);
  };


  std::call_once(animationOnceFlag, [&]() {
    dac::Async::periodic(moTimeStep(), &m_animThread, [&]() {
      bool isIdle =
          !(m_rotateLeft && m_rotateRight && m_moveForward && m_moveBackward);

      if (!m_play || isIdle) {
        // playIdle();
        if (!m_play) return;
      }

      // Displacement step
      auto  currFramePos = moCurrFrame().translation;
      auto  nextFramePos = moNextFrame().translation;
      float step         = glm::distance(nextFramePos, currFramePos);

      // User input reply // TODO: "step" calculation.
      if (m_rotateLeft) { skRotation(false); }
      if (m_rotateRight) { skRotation(true); }
      if (m_moveForward) { skMovement(true, step); }
      if (m_moveBackward) { skMovement(false, step); }

      // Update hierarchy
      // ? If we wanna make the movement based on user input, this also must wait user input
      int rotIdx = 0;
      for (const auto& joint : moJoints()) {
        if (joint->name == "Root") joint->model = m_model;

        // Hierarchy of joints
        if (joint->parent) { joint->model = joint->parent->model; }
        Math::translate(joint->model, joint->offset * m_scale);

        // Motion of joints
        Math::rotateXYZ(joint->model, moCurrFrame().rotations.at(rotIdx++));

        // Hierarchy of end-sites
        if (joint->endsite) {
          joint->endsite->model = joint->model;
          Math::translate(joint->endsite->model,
                          joint->endsite->offset * m_scale);
        }
      }

      // Frame counter
      (m_currFrame >= moFrames().size() - 2) ? m_currFrame = 0 : ++m_currFrame;
    });
  });
} // namespace brave

// ====================================================================== //
// ====================================================================== //
// Compute joints models and draw its renderable bone
// ====================================================================== //

void Skeleton::draw() {

  auto bone = [&](const std::shared_ptr<Joint>& J) {
    glm::mat4 boneModel(1.f);
    auto      JPos       = J->model[3].xyz();
    auto      JParentPos = J->parent->model[3].xyz();
    // T
    auto bonePos = (JPos + JParentPos) * 0.5f;
    Math::translate(boneModel, bonePos);
    // R
    auto vJ  = glm::normalize(JPos - JParentPos);
    auto vB2 = bonePos + glm::vec3(0, .5f, 0);
    auto vB1 = bonePos - glm::vec3(0, .5f, 0);
    auto vB  = glm::normalize(vB2 - vB1);
    Math::rotate(boneModel, glm::angle(vB, vJ), glm::cross(vB, vJ));
    // S
    auto boneSize = glm::vec3{1.f, glm::distance(JPos, JParentPos) * 0.5f, 1.f};
    Math::scale(boneModel, boneSize);
    // Draw
    Renderable::getByName("Bone")->model(boneModel);
    Renderable::getByName("Bone")->draw();
  };


  for (const auto& J : moJoints()) {
    if (!J->parent) continue;

    if (J->name == "Head" && J->endsite) {
      auto head = Renderable::getByName("Monkey");
      head->model(J->endsite->model);
      head->draw();
    }

    bone(J);
    if (J->endsite) bone(J->endsite);
  }
}

} // namespace brave
