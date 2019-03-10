#include "Skeleton.hpp"

#include "IO.hpp"
#include "Loader.hpp"
#include "Settings.hpp"
#include "Renderable.hpp"

#include "helpers/Consts.hpp"

namespace brave {

// ====================================================================== //
// ====================================================================== //
// Param constructor
// ====================================================================== //

Skeleton::Skeleton(const std::shared_ptr<brave::Camera>& camera, float scale)
    : m_animThread(true),
      m_readyToDraw(false),
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
  if (!Settings::quiet) dInfo("Skeleton destroyed!");
  this->play   = false;
  m_animThread = false;
}

// ====================================================================== //
// ====================================================================== //
// Compute displacement to apply on next user input
// ====================================================================== //
float Skeleton::step() {
  auto currFramePos = moCurrFrame().translation;
  auto nextFramePos = moNextFrame().translation;
  auto dist         = glm::distance(nextFramePos, currFramePos);
  return dist * dist * dist;
}


// ====================================================================== //
// ====================================================================== //
// Compute hierarchy of the skeleton based on current frame and motion
// ====================================================================== //

void Skeleton::hierarchy() {
  int rotIdx = 0;
  for (const auto& joint : moJoints()) {
    auto jtm = &joint->transformAsMatrix;
    // On root joint
    if (joint->name == "Root") { *jtm = this->transform.asMatrix(); }
    // On all joints
    if (joint->parent) { *jtm = joint->parent->transformAsMatrix; }
    Math::translate(*jtm, joint->offset * m_scale);
    Math::rotateXYZ(*jtm, moCurrFrame().rotations.at(rotIdx++));
    // On end-sites
    if (auto je = joint->endsite) {
      je->transformAsMatrix = *jtm;
      Math::translate(je->transformAsMatrix, je->offset * m_scale);
    }
  }

  // Frame counter
  (m_currFrame >= moFrames().size() - 2) ? m_currFrame = 0 : ++m_currFrame;
  m_readyToDraw                                        = true;
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


// ====================================================================== //
// ====================================================================== //
// Add motions to skeleton motion map
// ====================================================================== //

void Skeleton::addMotion(const std::string& name, const std::string& file) {
  //todo: before to emplace, CLEAN the motion
  m_motions.try_emplace(name, loader::BVH(file));
  m_motions[name]->name = m_currMotion = name;
}

// ====================================================================== //
// ====================================================================== //
// Modify current motion
// ====================================================================== //

void Skeleton::currMotion(const std::string& motionName) {

  // Don't make any operation if motion name doesn't exist
  if (m_motions.count(motionName) < 1) {
    if (!Settings::quiet) dErr("Zero motions with name {}.", motionName);
    return;
  }

  // if (motionName == "Idle") { m_move = -1; }

  m_readyToDraw = false;

  // Reset curr frame, because not every motion have the same length
  // and may incur a forbidden memory access
  m_currFrame = 0;

  // Modify current motion
  m_currMotion = motionName;
}


// ====================================================================== //
// ====================================================================== //
// Move forward
// ====================================================================== //

void Skeleton::moveFront(bool active) {
  // this->transform.rot.y = m_camera->pivot.rot.y;
  // this->transform.pos += m_camera->pivot.frontXZ() * step();

  move += ((active) ? 1 : -1) * (int)directions::front;
}

// ====================================================================== //
// ====================================================================== //
// Move to the right
// ====================================================================== //

void Skeleton::moveRight(bool active) {
  move += ((active) ? 1 : -1) * (int)directions::right;
}

// ====================================================================== //
// ====================================================================== //
// Move to the left
// ====================================================================== //

void Skeleton::moveLeft(bool active) {
  move += ((active) ? 1 : -1) * (int)directions::left;
}

// ====================================================================== //
// ====================================================================== //
// Move backward
// ====================================================================== //

void Skeleton::moveBack(bool active) {
  move += ((active) ? 1 : -1) * (int)directions::back;
}


// ====================================================================== //
// ====================================================================== //
// Run a detached thread for animation process
// ====================================================================== //

void Skeleton::animation() {
  std::call_once(animationOnceFlag, [&]() {
    //      ToDo: change periodic to allow change moTimeStep() on motion change
    dac::Async::periodic(moTimeStep(), &m_animThread, [&]() {
      if (!this->play) return;

      hierarchy();

      if (!(move == 0 or move == 3 or move == 12)) {
        this->transform.rot.y = m_camera->pivot.rot.y;
      }

      switch ((directions)move) {
        case directions::front:
          this->transform.pos += m_camera->pivot.frontXZ() * step();
          break;

        default: break;
      }

      if (!Settings::pollEvents) { glfwPostEmptyEvent(); }
    });
  });
}


// ====================================================================== //
// ====================================================================== //
// Compute joints transform matrices and draw its renderable bone
// ====================================================================== //

void Skeleton::draw() {

  //! Also, not the solution :(
  if (!m_readyToDraw) return;

  for (auto idx = 0u; idx < moJoints().size() - 2; ++idx) {
    auto J = moJoints().at(idx);
    if (!J->parent) continue;

    if (J->name == "Head" and J->endsite) {
      auto head                      = Renderable::getByName("MonkeyHead");
      head->transform.overrideMatrix = J->endsite->transformAsMatrix;
      head->draw(m_camera);
    }

    drawBone(J);
  }
}

} // namespace brave
