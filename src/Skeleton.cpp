#include "Skeleton.hpp"

#include "IO.hpp"
#include "Async.hpp"
#include "Logger.hpp"
#include "Loader.hpp"
#include "Settings.hpp"
#include "Renderable.hpp"
#include "helpers/Consts.hpp"

namespace brave {


// * private



// ====================================================================== //
// ====================================================================== //
// Compute hierarchy of the skeleton based on current frame and motion
// ====================================================================== //

void Skeleton::hierarchy(const std::string& motionName, unsigned int frame) {

  if (m_motions.count(motionName) < 1) {
    if (!Settings::quiet) LOGE("Zero motions with name {}.", motionName);
    return;
  }

  auto joints      = m_motions.at(motionName)->joints;
  auto targetFrame = m_motions.at(motionName)->frames.at(frame);

  for (auto idx = 0u; idx < joints.size(); ++idx) {

    auto joint = joints.at(idx);
    auto jtm   = &joint->transformAsMatrix;

    // On root joint
    if (joint->name == "Root") { *jtm = this->transform.asMatrix(); }

    // On all joints
    if (joint->parent) { *jtm = joint->parent->transformAsMatrix; }
    Math::translate(*jtm, joint->offset * m_scale);
    Math::rotateXYZ(*jtm, targetFrame.rotations.at(idx));

    // On end-sites
    if (auto je = joint->endsite) {
      je->transformAsMatrix = *jtm;
      Math::translate(je->transformAsMatrix, je->offset * m_scale);
    }
  }
}

// ====================================================================== //
// ====================================================================== //
// Compute displacement to apply on next user input
// ====================================================================== //

float Skeleton::step() {
  auto p1 = m_motions.at(m_currMotion)->frames.at(m_currFrame).translation;
  auto p2 = m_motions.at(m_currMotion)->frames.at(m_currFrame + 1).translation;
  return glm::distance2(p2, p1);
}

// ====================================================================== //
// ====================================================================== //
// Compute and draw a bone of a gived joint and its parent
// ====================================================================== //

void Skeleton::drawBone(const std::shared_ptr<Joint>& J) {
  auto bone = Renderable::getByName("Bone");

  // Joints positions
  auto JPos       = J->transformAsMatrix[3].xyz();
  auto JParentPos = J->parent->transformAsMatrix[3].xyz();

  // Position
  bone->transform.pos = (JPos + JParentPos) * 0.5f;

  // Rotation
  auto vB2                 = bone->transform.pos + glm::vec3(0, .5f, 0);
  auto vB1                 = bone->transform.pos - glm::vec3(0, .5f, 0);
  auto vJ                  = glm::normalize(JPos - JParentPos);
  auto vB                  = glm::normalize(vB2 - vB1);
  bone->transform.rotAngle = glm::angle(vB, vJ);
  bone->transform.rotAxis  = glm::cross(vB, vJ);

  // Scale
  auto jointsMidDistance = glm::distance(JPos, JParentPos) * 0.5f;
  bone->transform.scl    = glm::vec3{1.f, jointsMidDistance, 1.f};

  // Draw
  bone->draw(m_camera);

  // Draw also its endsite
  if (J->endsite) drawBone(J->endsite);
}



// * public



// ====================================================================== //
// ====================================================================== //
// Param constructor
// ====================================================================== //

Skeleton::Skeleton(const std::shared_ptr<brave::Camera>& camera, float scale)
    : m_animThread(true),
      m_scale(scale),
      m_currFrame(0u),
      m_camera(camera),
      m_currMotion(""),
      move(0),
      play(true) {

  if (m_camera) m_camera->target = std::shared_ptr<Transform>(&this->transform);
}

// ====================================================================== //
// ====================================================================== //
// Destructor
// ====================================================================== //

Skeleton::~Skeleton() {
  if (!Settings::quiet) LOGD("Skeleton destroyed!");
  this->play   = false;
  m_animThread = false;
}

// ====================================================================== //
// ====================================================================== //
// Define actions on key state
// ====================================================================== //

void Skeleton::onKey(int key, _IO_FUNC release, _IO_FUNC press) {
  IO::keyboardAddAction(key, IO::kbState::release, release);
  IO::keyboardAddAction(key, IO::kbState::press, press);
}

// ====================================================================== //
// ====================================================================== //
// Add motions to skeleton motion map
// ====================================================================== //

void Skeleton::addMotion(const std::string& name, const std::string& file) {
  if (name.find("_") != std::string::npos) {
    LOGE("Motion names can NOT contains '_'");
    return;
  }

  m_motions.try_emplace(name, loader::BVH(file));
  m_motions[name]->name = m_currMotion = name;

  // Compute mix for current motions
  //todo: ignore itself, change loop to i-type
  for (const auto& m1 : m_motions) {
    for (const auto& m2 : m_motions) {
      std::string key1 = m1.first + "_" + m2.first;
      std::string key2 = m2.first + "_" + m1.first;
      //? create 2 motions, m1->m2 , m2->m1 (Is not so many dup info?)
      if (m_motions.count(key1) < 1 and m_motions.count(key2) < 1) {
        m_motions.try_emplace(key1, m1.second->mix(m2.second));
      }
    }
  }
}

// ====================================================================== //
// ====================================================================== //
// Modify current motion
// ====================================================================== //

void Skeleton::currMotion(const std::string& motionName) {

  // Don't make any operation if motion name doesn't exist
  if (m_motions.count(motionName) < 1) {
    if (!Settings::quiet) LOGE("Zero motions with name {}.", motionName);
    return;
  }

  // Reset curr frame, because not every motion have the same length
  // and may incur a forbidden memory access
  m_currFrame = 0;

  // Modify current motion
  std::string key1 = m_currMotion + "_" + motionName;
  std::string key2 = motionName + "_" + m_currMotion;

  //todo: if reply to (?) on line 162 is false... when flip motion order and...
  m_currMotion = (m_motions.count(key1) < 1)
                     ? key1
                     : (m_motions.count(key2) < 1) ? key2 : motionName;
}


// ====================================================================== //
// ====================================================================== //
// Moves
// ====================================================================== //

int __move(bool active, int direction) {
  return ((active) ? 1 : -1) * direction;
}
void Skeleton::moveF(bool active) {
  move += __move(active, (int)directions::F);
}
void Skeleton::moveR(bool active) {
  move += __move(active, (int)directions::R);
}
void Skeleton::moveL(bool active) {
  move += __move(active, (int)directions::L);
}
void Skeleton::moveB(bool active) {
  move += __move(active, (int)directions::B);
}


// ====================================================================== //
// ====================================================================== //
// Run a detached thread for animation process
// ====================================================================== //

void Skeleton::animation() {
  std::call_once(animationOnceFlag, [&]() {
    Async::periodic(
        [&]() { return m_motions.at(m_currMotion)->timeStep; },
        &m_animThread,
        [&]() {
          if (!this->play) return;

          // Update hierarchy
          hierarchy(m_currMotion, m_currFrame);

          // Update frame counter
          auto frameLimit = m_motions.at(m_currMotion)->frames.size() - 2;
          (m_currFrame >= frameLimit) ? m_currFrame = 0 : ++m_currFrame;

          // Camera rotation sync when skeleton is moved
          if (!(move == 0 or move == 3 or move == 12)) {
            this->transform.rot.y = m_camera->pivot.rot.y;
          }

          // Move actions
          switch (move) {
            case (int)directions::F:
              this->transform.pos += m_camera->pivot.frontXZ() * step();
              break;
            default: break;
          }

          // Shoot an empty event if polling is not used
          if (!Settings::pollEvents) { glfwPostEmptyEvent(); }
        });
  });
} // namespace brave


// ====================================================================== //
// ====================================================================== //
// Compute joints transform matrices and draw its renderable bone
// ====================================================================== //

void Skeleton::draw() {
  auto joints = m_motions.at(m_currMotion)->joints;

  // Draw joints
  for (auto idx = 0u; idx < joints.size() - 2; ++idx) {
    auto J = joints.at(idx);
    if (!J->parent) continue;

    if (J->name == "Head" and J->endsite) {
      auto headRE                      = Renderable::getByName("MonkeyHead");
      headRE->transform.overrideMatrix = J->endsite->transformAsMatrix;
      headRE->draw(m_camera);
      continue;
    }

    auto jointRE                      = Renderable::getByName("Joint");
    jointRE->transform.overrideMatrix = J->transformAsMatrix;
    jointRE->draw(m_camera);
  }

  // Draw bones
  for (auto idx = 0u; idx < joints.size() - 2; ++idx) {
    auto J = joints.at(idx);
    if (!J->parent) continue;
    if (J->name != "Head") { drawBone(J); }
  }
}

} // namespace brave
