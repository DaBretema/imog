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
    if (joint->name == "Root") {
      *jtm = this->transform.asMatrix();
      // Math::translate(*jtm, targetFrame.translation * m_scale);
    }

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

glm::vec3 Skeleton::step3() {
  auto p1 = m_motions.at(m_currMotion)->frames.at(m_currFrame).translation;
  auto p2 = m_motions.at(m_currMotion)->frames.at(m_currFrame + 1).translation;
  return p2 - p1;
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
      m_lastFrame(-1),
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

void Skeleton::onKey(int key, _IO_FUNC press, _IO_FUNC release) {
  IO::keyboardAddAction(key, IO::kbState::release, release);
  IO::keyboardAddAction(key, IO::kbState::press, press);
}

// ====================================================================== //
// ====================================================================== //
// Add motions to skeleton motion map
// ====================================================================== //

void Skeleton::addMotion(const std::string& name,
                         const std::string& file,
                         loopMode           lm) {
  if (Motion::isMix(name)) {
    LOGE("Motion names can NOT contains '_' is reserved for motion mixeds");
    return;
  }

  m_motions.try_emplace(name, loader::BVH(file, lm));
  m_motions[name]->name = m_currMotion = name;

  // Compute mix for current motions
  for (const auto& m1 : m_motions) {
    if (m1.second->isMix()) continue;

    for (const auto& m2 : m_motions) {
      // Ignore itself and mixes
      if (m1.first == m2.first or m2.second->isMix()) continue;
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

// ====================================================================== //
// ====================================================================== //
// Modify current motion
// ====================================================================== //

void Skeleton::currMotion(const std::string& motionName,
                          unsigned int       targetFrame) {
  // Ignore if is on transition
  if (Motion::isMix(m_currMotion)) return;


  // Don't make any operation if motion name doesn't exist
  if (m_motions.count(motionName) < 1) {
    if (!Settings::quiet) LOGE("Zero motions with name {}.", motionName);
    return;
  }

  std::string key             = m_currMotion + "_" + motionName;
  bool        transitionExist = m_motions.count(key) > 0;
  // bool        transitionExist = m_motions.count(key) - 1 == 0;
  bool noEqualNames = m_currMotion != motionName;
  LOG("KEY CURR MOTION: {}", key);

  if (transitionExist && noEqualNames) {
    if (m_lastFrame > -1) {
      LOG("SecondEntry");
      m_lastFrame  = -1;
      m_currFrame  = m_motions.at(key)->frameB;
      m_currMotion = motionName;
    } else {
      LOGD("NEXT MOTION")
      m_lastFrame = m_motions.at(key)->frameA;
      LOGD("NEXT MOTION 2")
      std::lock_guard<std::mutex> guard(_modifyDestMotion);
      m_motions.at(m_currMotion)->destMotion = motionName;
      LOGD("NEXT MOTION 3")
      // LOG("Asigned output frame: {}", m_lastFrame);
    }
  } else {
    // Reset curr frame, because not every motion have the same length
    // and may incur a forbidden memory access
    m_currFrame = targetFrame;
    // Modify current motion
    m_currMotion = motionName;
  }

  // ? If reply to (?) on addmotion is false: When/How flip motion order?
  // m_currMotion = (transitionExist and noEqualNames) ? key : motionName;

  LOG("curr motion: {}", m_currMotion);
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
          auto frameLimit = (m_lastFrame < 0)
                                ? m_motions.at(m_currMotion)->frames.size() - 2
                                : m_lastFrame;

          if (m_currFrame >= frameLimit) {
            if (Motion::isMix(m_currMotion)) {
              auto nextMotion = Strings::split(m_currMotion, "_").at(1);
              currMotion(nextMotion, m_motions.at(m_currMotion)->frameB);
            } else {
              auto mo = m_motions.at(m_currMotion);
              if (auto dm = mo->destMotion; !dm.empty()) {
                LOG("HEREEEE");
                mo->destMotion = "";
                currMotion(dm);
              } else {
                m_currFrame = 0;
              }
            }
          } else {
            ++m_currFrame;
          }



          // Camera rotation sync when skeleton is executing a valid move
          // this->transform.rot.y = m_camera->pivot.rot.y;
          // if (m_validMoves.find(move) != m_validMoves.end()) {
          // }

          // Move actions
          // switch (move) {
          //   case (int)directions::F:
          //     this->transform.pos += m_camera->pivot.frontXZ() * step();
          //     break;
          //   default: break;
          // }

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
  try {
    // LOG("CURR_MOTION: {}", m_currMotion);
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
  } catch (const std::out_of_range& e) { LOGE("OOR: {}", e.what()); }
}

} // namespace brave
