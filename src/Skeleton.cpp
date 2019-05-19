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

// ====================================================================== //
// ====================================================================== //
// Param constructor
// ====================================================================== //

Skeleton::Skeleton(const std::shared_ptr<brave::Camera>& camera,
                   float                                 scale,
                   float                                 speed)
    : m_scale(scale),
      m_animThread(true),
      m_currFrame(0u),
      m_nextFrame(0u),
      m_currMotion(nullptr),
      m_nextMotion(nullptr),
      play(true),
      speed(speed),
      camera(camera),
      allowedRots(0.f, 1.f, 0.f),
      allowedTrans(0.f),
      lerpAtFlyAlpha(0.f) {
  if (camera) camera->target = std::shared_ptr<Transform>(&transform);
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


// * --- Private --------------------------------------------------------- //

// ====================================================================== //
// ====================================================================== //
// Frame counter
// ====================================================================== //

void Skeleton::frameCounter() {
  auto add          = (m_currMotion->linked) ? 10u : 0u;
  bool limitReached = m_currFrame >= m_currMotion->frames.size() - 2u + add;
  (limitReached) ? loadNextMotion() : (void)++m_currFrame;
}

// ====================================================================== //
// ====================================================================== //
// Compute hierarchy of the skeleton based on current frame and motion
// ====================================================================== //

void Skeleton::hierarchy() {
  auto                   joints = m_currMotion->joints;
  std::vector<glm::vec3> currRots;
  glm::vec3              currTrans;

  // lerp at fly
  if (m_currMotion->linked) {
    auto m1     = m_currMotion->frames;
    auto m2     = m_currMotion->linked->frames;
    auto factor = (float)m2.size() / (float)m1.size();

    if (m_currFrame >= m1.size() - 2u) {
      LOG("hereeeeee");
      float _alpha = (m_currFrame - m1.size() - 2u) * 0.1;
      LOG(_alpha);

      auto m1R1 = m1.at(m1.size() - 2u).rotations;
      auto m1T1 = m1.at(m1.size() - 2u).translation;
      auto m2R1 = m2.at(m2.size() - 2u).rotations;
      auto m2T1 = m2.at(m2.size() - 2u).translation;

      Frame f1;
      for (auto i = 0u; i < m1R1.size(); ++i) {
        auto newRot = glm::mix(m1R1[i], m2R1[i], lerpAtFlyAlpha);
        f1.rotations.push_back(newRot);
      }
      f1.translation = glm::mix(m1T1, m2T1, lerpAtFlyAlpha);

      auto m1R2 = m1.at(0u).rotations;
      auto m2R2 = m1.at(0u).rotations;
      auto m1T2 = m1.at(0u).translation;
      auto m2T2 = m2.at(0u).translation;

      Frame f2;
      for (auto i = 0u; i < m1R2.size(); ++i) {
        auto newRot = glm::mix(m1R2[i], m2R2[i], lerpAtFlyAlpha);
        f2.rotations.push_back(newRot);
      }
      f2.translation = glm::mix(m1T2, m2T2, lerpAtFlyAlpha);

      // Aqui interpolar entre F1 y F2
      Frame frame;
      frame.translation = glm::mix(f1.translation, f2.translation, _alpha);
      for (auto i = 0u; i < f1.rotations.size(); ++i) {
        auto newRot = glm::mix(f1.rotations[i], f2.rotations[i], _alpha);
        frame.rotations.push_back(newRot);
      }
      currRots  = frame.rotations;
      currTrans = frame.translation;

    } else {
      auto currFrame = ceilf(m_currFrame * factor);
      auto m1R       = m1.at(currFrame).rotations;
      auto m1T       = m1.at(currFrame).translation;
      auto m2R       = m2.at(currFrame).rotations;
      auto m2T       = m2.at(currFrame).translation;

      Frame f;
      for (auto i = 0u; i < m1R.size(); ++i) {
        auto newRot = glm::mix(m1R[i], m2R[i], lerpAtFlyAlpha);
        f.rotations.push_back(newRot);
      }
      currRots  = f.rotations;
      currTrans = glm::mix(m1T, m2T, lerpAtFlyAlpha);
    }
  } else {
    currRots  = m_currMotion->frames.at(m_currFrame).rotations;
    currTrans = m_currMotion->frames.at(m_currFrame).translation;
  }

  // Root joint
  //
  // if(userInput){
  transform.rot += this->rStep3() * allowedRots;
  transform.pos.y = currTrans.y;
  //} else {
  // transform.rot = currRot;
  // transform.pos = currTrans;
  //}

  joints[0]->transformAsMatrix = transform.asMatrix();

  // Rest of joints
  for (auto idx = 1u; idx < joints.size(); ++idx) {
    auto joint = joints.at(idx);
    auto jtm   = &joint->transformAsMatrix;
    *jtm       = joint->parent->transformAsMatrix;

    // Process current joint
    Math::translate(*jtm, joint->offset * m_scale);
    Math::rotateXYZ(*jtm, currRots.at(idx));

    // Process joint end-site (if exists)
    if (auto je = joint->endsite) {
      je->transformAsMatrix = *jtm;
      Math::translate(je->transformAsMatrix, je->offset * m_scale);
    }
  }
}


// ====================================================================== //
// ====================================================================== //
// Steps
// ====================================================================== //

// Distance
float Skeleton::step() {
  auto t1 = m_currMotion->frames.at(m_currFrame).translation;
  auto t2 = m_currMotion->frames.at(m_currFrame + 1).translation;
  return glm::distance(t2, t1);
}

// 3-axes displacement
glm::vec3 Skeleton::tStep3() {
  auto t1 = m_currMotion->frames.at(m_currFrame).translation;
  auto t2 = m_currMotion->frames.at(m_currFrame + 1).translation;
  return t2 - t1;
}

// 3-axes rotation
glm::vec3 Skeleton::rStep3() {
  auto r1 = m_currMotion->frames.at(m_currFrame).rotations.at(0);
  auto r2 = m_currMotion->frames.at(m_currFrame + 1).rotations.at(0);
  return r2 - r1;
}

// ====================================================================== //
// ====================================================================== //
// Verify if motion exist on motion map
// ====================================================================== //

template <typename T>
bool Skeleton::motionExistsOnMap(const std::unordered_map<std::string, T>& map,
                                 const std::string& name) {
  if (map.count(name) < 1) {
    if (!Settings::quiet) LOGE("Zero motions with name {}.", name);
    return false;
  }
  return true;
}
// Check for motion in mix map
bool Skeleton::mixMotionExists(const std::string& name) {
  return motionExistsOnMap(m_motionMap, name);
}
// Check for motion in regular map
bool Skeleton::motionExists(const std::string& name) {
  return motionExistsOnMap(m_motions, name);
}

// ====================================================================== //
// ====================================================================== //
// Compute and draw a bone of a gived joint and its parent
// ====================================================================== //

void Skeleton::drawBone(const std::shared_ptr<Joint>& J) {
  auto P1    = J->transformAsMatrix[3].xyz();
  auto P2    = J->parent->transformAsMatrix[3].xyz();
  auto scale = glm::distance(P1, P2) * 0.5f;
  auto bone  = Renderable::cylBetween2p(P1, P2, scale);

  bone->transform.scl *= glm::vec3(2.5f, 1.f, 2.5f);
  bone->draw(camera);
  if (J->endsite) drawBone(J->endsite);
}

// ====================================================================== //
// ====================================================================== //
// Jump from current motion to next motion modifying also
// the value of the current frame
// ====================================================================== //

void Skeleton::loadNextMotion() {
  if (!m_currMotion) return;

  if (!m_nextMotion) {
    m_currFrame = 0u;
    return;
  }

  m_currFrame  = m_nextFrame;
  m_currMotion = m_nextMotion;

  m_nextFrame  = 0u;
  m_nextMotion = nullptr;
}


// * --- Public --------------------------------------------------------- //

// ====================================================================== //
// ====================================================================== //
// Run a detached thread for animation process
// ====================================================================== //

void Skeleton::animate() {
  auto timestepFn = [&]() {
    // return speed * ((m_currMotion) ? m_currMotion->timeStep : 0.5f);
    return ((m_currMotion) ? m_currMotion->timeStep : 0.5f);
  };

  auto animationFn = [&]() {
    if (!this->play or !m_currMotion) return;
    hierarchy();
    frameCounter();
    if (!Settings::pollEvents) { glfwPostEmptyEvent(); }
  };

  // Thread lauch
  std::call_once(m_animationOnceFlag, [&]() {
    Async::periodic(timestepFn, &m_animThread, animationFn);
  });
}


// ====================================================================== //
// ====================================================================== //
// Compute joints models and draw its renderable bone
// ====================================================================== //

void Skeleton::draw() {
  if (!m_currMotion) return;
  auto joints = m_currMotion->joints;

  // Draw joints
  for (auto idx = 0u; idx < joints.size(); ++idx) {
    auto J = joints.at(idx);
    if (!J->parent) continue;

    if (J->name == "Head" and J->endsite) {
      auto headRE = Renderable::getByName("Monkey");

      auto aux = J->endsite->transformAsMatrix;
      aux      = glm::translate(aux, glm::vec3{0.f, 0.5f, 0.f});
      aux      = glm::scale(aux, glm::vec3{2.f});

      headRE->transform.overrideMatrix = aux;
      headRE->draw(camera);
    }

    drawBone(J);
  }
}

// ====================================================================== //
// ====================================================================== //
// Modify current motion (user call)
//-> Always lowercase
// ====================================================================== //
//
void Skeleton::setMotion(const std::string& dest) {
  auto        _dest = Strings::toLower(dest);
  std::string key   = m_currMotion->name + "_" + _dest;

  if (m_currMotion->name == _dest || m_currMotion->isMix() || !m_currMotion ||
      !motionExists(_dest) || !mixMotionExists(key))
    return;

  auto t = m_motionMap.at(key).at(m_currFrame);

  m_nextFrame  = t.first;
  m_nextMotion = m_motions.at(_dest);

  m_currFrame  = 0u;
  m_currMotion = t.second;
}

// ====================================================================== //
// ====================================================================== //
// Add motions to skeleton motion map
//-> The motion name is converted to lowercase
// ====================================================================== //

void Skeleton::addMotion(const std::shared_ptr<Motion> m2) {
  // @lambda : Manage motion list and wrap the process of mix two motions
  auto _mix = [&](const std::shared_ptr<Motion>& _m1,
                  const std::shared_ptr<Motion>& _m2) {
    std::string key    = _m1->name + "_" + _m2->name;
    auto        mixMap = _m1->mix(_m2);
    this->m_motionMap.insert({key, mixMap});
  };

  // ----

  if (m2->isMix()) {
    LOGE("Motion names can NOT contains '_' is reserved for mixed motions");
    return;
  }
  m2->name = Strings::toLower(m2->name); // ! must

  // Compute transitions
  for (const auto& [_, m1] : m_motions) {
    if (m1->isMix()) continue;
    _mix(m1, m2);
    _mix(m2, m1);
  }

  m_motions.try_emplace(m2->name, m2);
  m_currMotion = m_motions.at(m2->name);
}


// ====================================================================== //
// ====================================================================== //
// Define actions on key state
// ====================================================================== //

void Skeleton::onKey(int      key,
                     _IO_FUNC press,
                     _IO_FUNC release,
                     _IO_FUNC repeat) {
  IO::keyboardAddAction(key, IO::kbState::press, press);
  IO::keyboardAddAction(key, IO::kbState::release, release);
  IO::keyboardAddAction(key, IO::kbState::repeat, repeat);
}

} // namespace brave
