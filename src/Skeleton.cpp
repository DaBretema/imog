#include "Skeleton.hpp"

#include "IO.hpp"
#include "Strings.hpp"
#include "Async.hpp"
#include "Logger.hpp"
#include "Loader.hpp"
#include "Settings.hpp"
#include "Renderable.hpp"
#include "helpers/Consts.hpp"

namespace brave {


// * ----------------------
// * Helpers
// * ----------------------

// ====================================================================== //
// ====================================================================== //
// Compute distance to apply on next user input
// ====================================================================== //

float Skeleton::step() {
  auto p1 = m_motions.at(m_currMotion)->frames.at(m_currFrame).translation;
  auto p2 = m_motions.at(m_currMotion)->frames.at(m_currFrame + 1).translation;
  return glm::distance2(p2, p1);
}

// ====================================================================== //
// ====================================================================== //
// Compute displacement per component to apply on next user input
// ====================================================================== //

glm::vec3 Skeleton::step3() {
  auto p1 = m_motions.at(m_currMotion)->frames.at(m_currFrame).translation;
  auto p2 = m_motions.at(m_currMotion)->frames.at(m_currFrame + 1).translation;
  return p2 - p1;
}

// ====================================================================== //
// ====================================================================== //
// Verify if motion exist on motion map
// ====================================================================== //

bool Skeleton::motionExists(const std::string& dest) {
  if (m_motions.count(dest) < 1) {
    if (!Settings::quiet) LOGE("Zero motions with name {}.", dest);
    return false;
  }
  return true;
}


// * ----------------------
// * Animation
// * ----------------------

// ====================================================================== //
// ====================================================================== //
// Frame step
// ====================================================================== //

void Skeleton::frameStep() {
  auto frameLimit = (m_lastFrame < 0)
                        ? m_motions.at(m_currMotion)->frames.size() - 2
                        : m_lastFrame;
  // Limit reached
  if (m_currFrame >= frameLimit) {
    if (!m_nextMotion.empty()) {
      auto nm = m_nextMotion; // ! copy assigned next motion before clear it
      m_nextMotion.clear();   // ! must be cleared before call setMotion
      _setMotion(nm);
    } else
      m_currFrame = 0;
  } else
    ++m_currFrame;
}

// ====================================================================== //
// ====================================================================== //
// User input movement over root node
// ====================================================================== //

void Skeleton::rootMovement() {
  // Camera rotation sync when skeleton is executing a valid move
  // if (m_validMoves.find(m_move) != m_validMoves.end()) {
  //   m_transform.rot.y = m_camera->pivot.rot.y;
  // }

  // Move actions
  switch ((directions)m_move) {
    case directions::F:
      m_transform.pos += m_camera->pivot.frontXZ() * step();
      break;
    default: break;
  }
}

// ====================================================================== //
// ====================================================================== //
// Compute hierarchy of the skeleton based on current frame and motion
// ====================================================================== //

void Skeleton::hierarchy() {
  auto joints      = m_motions.at(m_currMotion)->joints;
  auto targetFrame = m_motions.at(m_currMotion)->frames.at(m_currFrame);

  // -------------------------------------------------
  // * ROOT JOINT
  // -------------------------------------------------

  auto root = joints[0];
  auto rtm  = &root->transformAsMatrix;
  // Root movement
  *rtm       = m_transform.asMatrix();
  auto yDist = this->step3().y * 2.f;
  Math::translate(*rtm, glm::vec3(0.f, yDist, 0.f));
  // Root rotation (blocking X and Z rots)
  auto rot = targetFrame.rotations.at(0);
  if (m_motions.at(m_currMotion)->lockRotOnXZ) {
    auto yAngle = (rot.x + rot.z) * 0.5 + rot.y;
    Math::rotate(*rtm, glm::radians(yAngle), Math::unitVecY);
  } else {
    Math::rotateXYZ(*rtm, rot);
  }


  // -------------------------------------------------
  // * OTHERS JOINTS
  // -------------------------------------------------

  for (auto idx = 1u; idx < joints.size(); ++idx) {
    auto joint = joints.at(idx);
    auto jtm   = &joint->transformAsMatrix;
    // Process current joint
    *jtm = joint->parent->transformAsMatrix;
    Math::translate(*jtm, joint->offset * m_scale);
    Math::rotateXYZ(*jtm, targetFrame.rotations.at(idx));
    // Process joint end-site (if exists)
    if (auto je = joint->endsite) {
      je->transformAsMatrix = *jtm;
      Math::translate(je->transformAsMatrix, je->offset * m_scale);
    }
  }
}

// ====================================================================== //
// ====================================================================== //
// ! Run a detached thread for animation process
// ====================================================================== //

void Skeleton::animate() {

  auto timestepFn = [&]() { return m_motions.at(m_currMotion)->timeStep; };

  auto animationFn = [&]() {
    if (!this->play) return;
    hierarchy();
    frameStep();
    rootMovement();
    if (!Settings::pollEvents) { glfwPostEmptyEvent(); }
  };

  // Thread lauch
  std::call_once(m_animationOnceFlag, [&]() {
    Async::periodic(timestepFn, &m_animThread, animationFn);
  });
}


// * ----------------------
// * Draw
// * ----------------------

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

// ====================================================================== //
// ====================================================================== //
// ! Compute joints transform matrices and draw its renderable bone
// ====================================================================== //

void Skeleton::draw() {
  auto joints = m_motions.at(m_currMotion)->joints;

  // Draw joints
  for (auto idx = 0u; idx < joints.size(); ++idx) {
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
  for (auto idx = 0u; idx < joints.size(); ++idx) {
    auto J = joints.at(idx);
    if (!J->parent) continue;
    // if (J->name != "Head") { drawBone(J); }
    drawBone(J);
  }
}

// * ----------------------
// * Motions setup
// * ----------------------

// ====================================================================== //
// ====================================================================== //
// Modify current motion (intern call)
// ====================================================================== //

void Skeleton::_setMotion(const std::string& dest) {
  // Don't make any operation if motion name doesn't exist
  if (!motionExists(dest)) return;

  // Mix motion completed
  if (Motion::isMix(dest)) {
    m_lastFrame  = -1;
    m_currMotion = dest;
    m_currFrame  = 0;
    // Puts next simple motion in 'queue'
    m_nextMotion = Strings::split(dest, "_")[1];
  }

  // From mix to dest motion
  else if (Motion::isMix(m_currMotion)) {
    m_currFrame  = m_motions.at(m_currMotion)->frameB;
    m_currMotion = dest;
  }

  // Never...
  else {
    LOGE("SHOULD NOT BE HERE!!!");
  }
}


// ====================================================================== //
// ====================================================================== //
// ! Modify current motion (user call)
// ====================================================================== //

void Skeleton::setMotion(const std::string& dest) {
  // Don't make any operation if motion name doesn't exist
  if (!motionExists(dest)) return;
  // Ignore if is on transition
  if (Motion::isMix(m_currMotion)) return;

  std::string KEY = m_currMotion + "_" + dest;
  if (motionExists(KEY) && m_currMotion != dest) {
    m_lastFrame  = m_motions.at(KEY)->frameA;
    m_nextMotion = KEY; // Put mix in 'queue'
  }
}

// ====================================================================== //
// ====================================================================== //
// ! Add motions to skeleton motion map
// ====================================================================== //

void Skeleton::addMotion(const std::string& name,
                         const std::string& file,
                         bool               makeLoop) {

  if (Motion::isMix(name)) {
    LOGE("Motion names can NOT contains '_' is reserved for mixed motions");
    return;
  }

  m_motions.try_emplace(name, loader::BVH(file, makeLoop));
  m_motions.at(name)->name = m_currMotion = name;

  // Compute transition for current motions
  for (const auto& m1 : m_motions) {
    if (m1.second->isMix()) continue;
    for (const auto& m2 : m_motions) {
      if (m1.first == m2.first or m2.second->isMix()) continue;
      // Keys
      std::string key1 = m1.first + "_" + m2.first;
      std::string key2 = m2.first + "_" + m1.first;
      // Motion A-B
      if (m_motions.count(key1) < 1)
        m_motions.try_emplace(key1, m1.second->mix(m2.second));
      // Motion B-A
      if (m_motions.count(key2) < 1)
        m_motions.try_emplace(key2, m2.second->mix(m1.second));
    }
  }
}



// * ----------------------
// * Movement
// * ----------------------

int __move(bool active, int direction) {
  return ((active) ? 1 : -1) * direction;
}
void Skeleton::moveF(bool active) {
  // move += __move(active, (int)directions::F);
  m_move = __move(active, (int)directions::F);
}
// void Skeleton::moveR(bool active) {
//   move += __move(active, (int)directions::R);
// }
// void Skeleton::moveL(bool active) {
//   move += __move(active, (int)directions::L);
// }
// void Skeleton::moveB(bool active) {
//   move += __move(active, (int)directions::B);
// }



// * ----------------------
// * Object
// * ----------------------

// ====================================================================== //
// ====================================================================== //
// ! Param constructor
// ====================================================================== //

Skeleton::Skeleton(const std::shared_ptr<brave::Camera>& camera, float scale)
    : m_camera(camera),
      m_move(0),
      m_animThread(true),
      m_nextMotion(""),
      m_scale(scale),
      m_currFrame(0u),
      m_lastFrame(-1),
      m_currMotion(""),
      play(true) {

  if (m_camera) m_camera->target = std::shared_ptr<Transform>(&m_transform);
}

// ====================================================================== //
// ====================================================================== //
// ! Destructor
// ====================================================================== //

Skeleton::~Skeleton() {
  if (!Settings::quiet) LOGD("Skeleton destroyed!");
  this->play   = false;
  m_animThread = false;
}

// ====================================================================== //
// ====================================================================== //
// ! Define actions on key state
// ====================================================================== //

void Skeleton::onKey(int key, _IO_FUNC press, _IO_FUNC release) {
  IO::keyboardAddAction(key, IO::kbState::release, release);
  IO::keyboardAddAction(key, IO::kbState::press, press);
}

} // namespace brave
