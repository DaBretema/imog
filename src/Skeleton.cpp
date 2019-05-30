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
      m_linkedAlpha(0.f),
      play(true),
      speed(speed),
      camera(camera),
      allowedRots(0.f, 1.f, 0.f),
      allowedTrans(0.f),
      linkedSteps(10u) {
  // if (camera) camera->target = std::shared_ptr<Transform>(&transform);
  // if (camera) camera->target = nullptr; //! why is dangerous ?
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
// Counters
// ====================================================================== //

float Skeleton::alphaStep() const {
  return 1.f / glm::clamp((float)linkedSteps, 0.f, 100.f);
}

Frame Skeleton::transitionedLinkedFrame() const {
  auto  F1    = m_currMotion->linkedFrame(lastFrame(), m_linkedAlpha);
  auto  F2    = m_currMotion->linkedFrame(0u, m_linkedAlpha);
  float step  = alphaStep();
  float alpha = step + (float)(m_currFrame - lastFrame()) * step;
  return F1.lerpOne(F2, alpha);
};

unsigned int Skeleton::lastFrame() const {
  return m_currMotion->frames.size() - 2u;
}

void Skeleton::frameCounter() {
  auto offsetForNewFrames = (m_currMotion->linked) ? (linkedSteps - 1u) : 0u;
  bool limitReached = m_currFrame >= lastFrame() + offsetForNewFrames - 1u;
  (limitReached) ? loadNextMotion() : (void)++m_currFrame;
}

// ====================================================================== //
// ====================================================================== //
// Compute hierarchy of the skeleton based on current frame and motion
// ====================================================================== //

void Skeleton::hierarchy() {
  auto joints = m_currMotion->joints;

  Frame F = (m_currMotion->linked)
                ? (m_currFrame >= lastFrame())
                      ? transitionedLinkedFrame()
                      : m_currMotion->linkedFrame(m_currFrame, m_linkedAlpha)
                : m_currMotion->frames.at(m_currFrame);

  // === ROOT ===
  // XZ trans is by user, only apply Y
  // transform.pos.y = F.translation.y;
  transform.pos.y = F.translation.y;
  // Y rot is by user, only apply XZ
  transform.rot.x = F.rotations.at(0).x;
  transform.rot.z = F.rotations.at(0).z;

  // Store
  joints[0]->transformAsMatrix = transform.asMatrix();



  // === JOINTS ===
  for (auto idx = 1u; idx < joints.size(); ++idx) {
    auto joint = joints.at(idx);
    auto jtm   = &joint->transformAsMatrix;
    *jtm       = joint->parent->transformAsMatrix;

    // Process current joint
    Math::translate(*jtm, joint->offset * m_scale);
    Math::rotateXYZ(*jtm, F.rotations.at(idx));

    // Process joint end-site (if exists)
    if (auto je = joint->endsite) {
      je->transformAsMatrix = *jtm;
      Math::translate(je->transformAsMatrix, je->offset * m_scale);
    }
  }
}

// ====================================================================== //
// ====================================================================== //
// Verify if motion exist on motion map
// ====================================================================== //

template <typename T>
bool Skeleton::motionExistsOnMap(const std::unordered_map<std::string, T>& map,
                                 const std::string& name) const {
  if (map.count(name) < 1) {
    if (!Settings::quiet) LOGE("Zero motions with name {}.", name);
    return false;
  }
  return true;
}
// Check for motion in mix map
bool Skeleton::mixMotionExists(const std::string& name) const {
  return motionExistsOnMap(m_motionMap, name);
}
// Check for motion in regular map
bool Skeleton::motionExists(const std::string& name) const {
  return motionExistsOnMap(m_motions, name);
}

// ====================================================================== //
// ====================================================================== //
// Compute and draw a bone of a gived joint and its parent
// ====================================================================== //

void Skeleton::drawBone(const std::shared_ptr<Joint>& J) const {
  auto P1    = J->transformAsMatrix[3].xyz();
  auto P2    = J->parent->transformAsMatrix[3].xyz();
  auto scale = glm::distance(P1, P2) * 0.5f;
  auto bone  = Renderable::cylBetween2p(P1, P2, scale);

  bone->transform.scl *= glm::vec3(3.5f, 1.f, 3.5f);
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

void Skeleton::incLinkedAlpha() {
  m_linkedAlpha += alphaStep();
  m_linkedAlpha = glm::clamp(m_linkedAlpha, 0.f, 1.f);
}
void Skeleton::decLinkedAlpha() {
  m_linkedAlpha -= alphaStep();
  m_linkedAlpha = glm::clamp(m_linkedAlpha, 0.f, 1.f);
}

// ====================================================================== //
// ====================================================================== //
// Translation step
// ====================================================================== //

float Skeleton::step() const {
  glm::vec3 t1, t2;
  float     maxStep = m_currMotion->maxStep();
  auto      cf      = glm::clamp(m_currFrame, 0u, lastFrame());


  if (m_currMotion->linked and m_currFrame >= lastFrame()) {
    t1 = m_currMotion->linkedFrame(cf, m_linkedAlpha).translation;
    t2 = m_currMotion->linkedFrame(cf + 1u, m_linkedAlpha).translation;

    auto lMaxStep = m_currMotion->linked->maxStep();
    maxStep       = glm::lerp(maxStep, lMaxStep, m_linkedAlpha);
  }

  else {
    t1 = m_currMotion->frames.at(cf).translation;
    t2 = m_currMotion->frames.at(cf + 1u).translation;
  }

  auto step = glm::distance(t2, t1);
  step      = glm::clamp(step, 0.f, maxStep);

  return step * speed;
}

// ====================================================================== //
// ====================================================================== //
// Compute rotation displacement per component to apply on next user input
// ====================================================================== //

glm::vec3 Skeleton::rotSteps() const {
  glm::vec3 r1, r2;
  auto      cf = glm::clamp(m_currFrame, 0u, lastFrame());

  if (m_currMotion->linked and m_currFrame >= lastFrame()) {
    r1 = m_currMotion->linkedFrame(cf, m_linkedAlpha).rotations.at(0);
    r2 = m_currMotion->linkedFrame(cf + 1u, m_linkedAlpha).rotations.at(0);
  }

  else {
    r1 = m_currMotion->frames.at(cf).rotations.at(0);
    r2 = m_currMotion->frames.at(cf + 1u).rotations.at(0);
  }

  return r1 - r2;
}

// ====================================================================== //
// ====================================================================== //
// Run a detached thread for animation process
// ====================================================================== //

void Skeleton::animate() {
  auto timestepFn = [&]() {
    if (!m_currMotion) return 0.5f / speed;

    if (m_currMotion->linked) {
      return glm::lerp(m_currMotion->timeStep,
                       m_currMotion->linked->timeStep,
                       m_linkedAlpha);
    } else {
      return m_currMotion->timeStep / speed;
    }
  };

  auto animationFn = [&]() {
    if (!this->play or !m_currMotion) return;
    hierarchy();
    frameCounter();
    userFn();
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

void Skeleton::draw() const {
  if (!m_currMotion) return;
  auto joints = m_currMotion->joints;

  // Draw joints
  for (auto idx = 0u; idx < joints.size(); ++idx) {
    auto J = joints.at(idx);
    if (!J->parent) continue;

    if (J->name == "Head" and J->endsite) {
      auto headRE = Renderable::getByName("Monkey");

      auto aux = J->endsite->transformAsMatrix;
      aux      = glm::translate(aux, glm::vec3{0.f, 1.f, 0.f});
      aux      = glm::scale(aux, glm::vec3{3.f});

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

  auto t = (m_currFrame >= lastFrame()) ? m_motionMap.at(key).at(lastFrame())
                                        : m_motionMap.at(key).at(m_currFrame);
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
                     _IO_FUNC repeat) const {
  IO::keyboardAddAction(key, IO::kbState::press, press);
  IO::keyboardAddAction(key, IO::kbState::release, release);
  IO::keyboardAddAction(key, IO::kbState::repeat, repeat);
}

} // namespace brave
