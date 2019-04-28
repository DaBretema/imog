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
  auto p1 = m_currMotion->frames.at(m_currFrame).translation;
  auto p2 = m_currMotion->frames.at(m_currFrame + 1).translation;
  return glm::distance2(p2, p1);
}

// ====================================================================== //
// ====================================================================== //
// Compute translation displacement per component to apply on next user input
// ====================================================================== //

glm::vec3 Skeleton::tStep3() {
  auto cmFrames = m_currMotion->frames;
  auto p1       = cmFrames.at(m_currFrame).translation;
  auto p2       = cmFrames.at(m_currFrame + 1).translation;
  return p2 - p1;
}

// ====================================================================== //
// ====================================================================== //
// Compute rotation displacement per component to apply on next user input
// ====================================================================== //

glm::vec3 Skeleton::rStep3() {
  auto cmFrames = m_currMotion->frames;
  auto p1       = cmFrames.at(m_currFrame).rotations.at(0);
  auto p2       = cmFrames.at(m_currFrame + 1).rotations.at(0);
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
  auto frameLimit =
      (m_lastFrame < 0) ? m_currMotion->frames.size() - 2 : m_lastFrame;
  // Limit reached
  if (m_currFrame >= frameLimit) {
    if (!m_nextMotion.empty()) {
      auto nm = m_nextMotion; // ! copy assigned next motion before clear it
      m_nextMotion.clear();   // ! must be cleared before call setMotion
      this->_setMotion(nm);
    } else
      m_currFrame = 0;
  } else {
    ++m_currFrame;
  }
}

// ====================================================================== //
// ====================================================================== //
// Compute hierarchy of the skeleton based on current frame and motion
// ====================================================================== //

void Skeleton::hierarchy() {
  auto joints      = m_currMotion->joints;
  auto targetFrame = m_currMotion->frames.at(m_currFrame);

  // -------------------------------------------------
  // * ROOT JOINT
  // -------------------------------------------------

  auto rtm = &joints[0]->transformAsMatrix;
  transform.pos += transform.front() * this->step(); //this->tStep3();
  transform.rot += this->rStep3();                   //this->tStep3();

  *rtm = transform.asMatrix();
  Math::translate(*rtm, glm::vec3(0.f, tStep3().y, 0.f));
  // Math::translate(*rtm, glm::vec3(0.f, this->step3().y * 2.f, 0.f));
  // Math::rotateXYZ(*rtm, this->rStep3());
  // Math::translate(*rtm, targetFrame.translation);
  // Math::rotateXYZ(*rtm, targetFrame.rotations.at(0));


  // -------------------------------------------------
  // * OTHERS JOINTS
  // -------------------------------------------------

  for (auto idx = 1u; idx < joints.size(); ++idx) {
    auto joint = joints.at(idx);
    auto jtm   = &joint->transformAsMatrix;
    *jtm       = joint->parent->transformAsMatrix;

    // Process current joint
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

  auto timestepFn = [&]() {
    return (m_currMotion) ? m_currMotion->timeStep : 0.5f;
  };

  auto animationFn = [&]() {
    if (!this->play or !m_currMotion) return;
    hierarchy();
    frameStep();
    // rootMovement();
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
  auto vB2                 = bone->transform.pos + glm::vec3(0, 0.5f, 0);
  auto vB1                 = bone->transform.pos - glm::vec3(0, 0.5f, 0);
  auto vJ                  = glm::normalize(JPos - JParentPos);
  auto vB                  = glm::normalize(vB2 - vB1);
  bone->transform.rotAngle = glm::angle(vB, vJ);
  bone->transform.rotAxis  = glm::cross(vB, vJ);

  // Scale
  auto jointsMidDistance = glm::distance(JPos, JParentPos) * 0.5f;
  bone->transform.scl    = glm::vec3{1.f, jointsMidDistance, 1.0f};

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
  if (!m_currMotion) return;
  auto joints = m_currMotion->joints;

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

void Skeleton::_setMotion(const std::string& _dest) {
  auto dest = Strings::toLower(_dest);
  // Don't make any operation if motion name doesn't exist
  if (!motionExists(dest)) return;

  // LOGD("currmotion = {}", m_currMotion->name);
  // LOGD("dest = {}", dest);

  // Mix motion completed
  if (Motion::isMix(dest)) {
    m_lastFrame  = -1;
    m_currMotion = m_motions.at(dest);
    m_currFrame  = 0;
    // Puts next simple motion in 'queue'
    m_nextMotion = Strings::split(dest, "_")[1];
  }

  // From mix to dest motion
  else if (m_currMotion->isMix()) {
    m_currFrame  = m_currMotion->frameB;
    m_currMotion = m_motions.at(dest);
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

void Skeleton::setMotion(const std::string& _dest) {
  if (!m_currMotion) return;
  auto dest = Strings::toLower(_dest);
  // Don't make any operation if motion name doesn't exist
  if (!motionExists(dest)) return;
  // Ignore if is on transition
  if (m_currMotion->isMix()) return;

  std::string KEY = m_currMotion->name + "_" + dest;
  if (motionExists(KEY) && m_currMotion->name != dest) {
    m_lastFrame  = m_motions.at(KEY)->frameA;
    m_nextMotion = KEY; // Put mix in 'queue'
  }
}

// ====================================================================== //
// ====================================================================== //
// ! Add motions to skeleton motion map
// ====================================================================== //

void Skeleton::addMotion(const std::shared_ptr<Motion> m2) {

  // @lambda : Manage motion list and wrap the process of mix two motions
  auto _mix = [&](auto _m1, auto _m2) {
    std::string key = _m1->name + "_" + _m2->name;
    auto        aux = _m1->mix(_m2);
    aux->name       = key;
    if (m_motions.count(key) < 1) m_motions.try_emplace(key, aux);
  };

  // ----
  //

  if (m2->isMix()) {
    LOGE("Motion names can NOT contains '_' is reserved for mixed motions");
    return;
  }
  m2->name = Strings::toLower(m2->name);

  // Compute transition for current motions
  for (const auto& [_, m1] : m_motions) {
    if (m1->isMix()) continue;
    _mix(m1, m2);
    _mix(m2, m1);
  }

  m_motions.try_emplace(m2->name, m2);
  m_currMotion = m_motions.at(m2->name);
}

// * ----------------------
// * Object
// * ----------------------

// ====================================================================== //
// ====================================================================== //
// ! Param constructor
// ====================================================================== //

Skeleton::Skeleton(const std::shared_ptr<brave::Camera>& camera, float scale)
    : m_camera(camera),
      m_animThread(true),
      m_nextMotion(""),
      m_scale(scale),
      m_currFrame(0u),
      m_lastFrame(-1),
      m_currMotion(nullptr),
      play(true) {

  if (m_camera) m_camera->target = std::shared_ptr<Transform>(&transform);
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
