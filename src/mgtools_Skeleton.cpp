#include "mgtools_Skeleton.hpp"

#include "gltools_IO.hpp"
#include "cpptools_Strings.hpp"
#include "cpptools_Async.hpp"
#include "cpptools_Logger.hpp"
#include "gltools_Loader.hpp"
#include "Settings.hpp"
#include "gltools_Renderable.hpp"
#include "helpers/Consts.hpp"

namespace imog {

// ====================================================================== //
// ====================================================================== //
// Param constructor
// ====================================================================== //

Skeleton::Skeleton(const std::shared_ptr<imog::Camera>& camera,
                   float                                scale,
                   float                                speed)
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
      linkedSteps(10u) {}


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
  transform.pos.y   = F.translation.y;
  transform.rot.x   = F.rotations.at(0).x;
  transform.rot.z   = F.rotations.at(0).z;
  joints[0]->matrix = transform.asMatrix();
  Math::rotateXYZ(joints[0]->matrix, F.rotations.at(0).y * Math::unitVecY);
  // ======

  // === JOINTS ===
  for (auto idx = 1u; idx < joints.size(); ++idx) {
    auto joint    = joints.at(idx);
    joint->matrix = joint->parent->matrix;

    // Process current joint
    Math::translate(joint->matrix, joint->offset * m_scale);
    Math::rotateXYZ(joint->matrix, F.rotations.at(idx));

    // Process joint end-site (if exists)
    if (auto je = joint->endsite) {
      je->matrix = joint->matrix;
      Math::translate(je->matrix, je->offset * m_scale);
    }
  }
  // ======
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
  auto P1    = J->matrix[3].xyz();
  auto P2    = J->parent->matrix[3].xyz();
  auto scale = glm::distance(P1, P2) * 0.5f;
  auto bone  = Renderable::line(P1, P2, scale);

  bone->transform.scl *= glm::vec3(5.f, 1.f, 5.f);
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
// Link with the camera
// ====================================================================== //

void Skeleton::toggleCameraFollow() {
  this->camera->target = (!this->camera->target)
                             ? std::shared_ptr<Transform>(
                                   &this->transform, [](imog::Transform*) {})
                             : nullptr;
}

// ====================================================================== //
// ====================================================================== //
// Manage speed
// ====================================================================== //

void Skeleton::incSpeed() { speed = glm::clamp(speed + 0.1f, 1.f, 2.5f); }
void Skeleton::decSpeed() { speed = glm::clamp(speed - 0.1f, 1.f, 2.5f); };

// ====================================================================== //
// ====================================================================== //
// Manage linked motion alpha to lerp
// ====================================================================== //

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
// Run a detached thread for animation process
// ====================================================================== //

void Skeleton::animate() {

  // Time per frame
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

  // Actions per frame
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

      auto aux = J->endsite->matrix;
      aux      = glm::translate(aux, glm::vec3{0.f, 1.f, 0.f});
      aux      = glm::scale(aux, glm::vec3{3.f});

      headRE->transform.overrideMatrix = aux;
      headRE->draw(camera);
    }

    // auto jointRE = Renderable::getByName("Ball");
    // auto aux = J->matrix;
    // aux      = glm::scale(aux, glm::vec3{3.f});
    // jointRE->transform.overrideMatrix = aux;
    // jointRE->draw(camera);

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
  m2->name = Strings::toLower(m2->name);

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

} // namespace imog
