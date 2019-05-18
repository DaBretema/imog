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
      allowedTrans(0.f) {
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
  bool limitReached = m_currFrame >= m_currMotion->frames.size() - 2;
  (limitReached) ? loadNextMotion() : (void)++m_currFrame;
}

// ====================================================================== //
// ====================================================================== //
// Compute hierarchy of the skeleton based on current frame and motion
// ====================================================================== //

void Skeleton::hierarchy() {
  auto joints    = m_currMotion->joints;
  auto currRots  = m_currMotion->frames.at(m_currFrame).rotations;
  auto currTrans = m_currMotion->frames.at(m_currFrame).translation;

  // Root joint
  transform.rot += this->rStep3() * allowedRots;
  transform.pos.y = currTrans.y;
  // transform.pos += this->tStep3() * Math::unitVecY; //* allowedTrans;

  auto rtm = &joints[0]->transformAsMatrix;
  *rtm     = transform.asMatrix();

  // LOG(currTrans.y)

  // Math::translate(*rtm, currTrans * Math::unitVecY);


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
  // Transform tr;
  // tr.rot = m_currMotion->frames.at(m_currFrame).rotations.at(0);
  // auto x = glm::orientedAngle(Math::unitVecY, tr.up(), Math::unitVecX);
  // auto y = glm::orientedAngle(Math::unitVecZ, tr.front(), Math::unitVecY);
  // auto z = glm::orientedAngle(Math::unitVecX, tr.right(), Math::unitVecZ);
  // auto out = glm::degrees(glm::vec3{x, y, z});
  //
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

// ====================================================================== //
// ====================================================================== //
// Modify current motion (intern call)
// ====================================================================== //

// void Skeleton::_setMotion(const std::string& dest) {
//   auto _dest = Strings::toLower(dest);
//   if (!motionExists(_dest)) return;

//   // Mix motion completed
//   if (Motion::isMix(_dest)) {
//     m_lastFrame  = -1;
//     m_currMotion = m_motions.at(_dest);
//     m_currFrame  = 0;
//     // Puts next simple motion in 'queue'
//     m_nextMotion = Strings::split(_dest, "_")[1];
//   }

//   // From mix to dest motion
//   else if (m_currMotion->isMix()) {
//     m_currFrame  = m_currMotion->frameB;
//     m_currMotion = m_motions.at(_dest);
//   }

//   // Never...
//   else {
//     LOGE("SHOULD NOT BE HERE!!!");
//   }
// }

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

  if (!m_currMotion || !motionExists(_dest) || m_currMotion->isMix() ||
      !(mixMotionExists(key) && m_currMotion->name != _dest))
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
