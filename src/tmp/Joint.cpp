#include "Joint.hpp"

#include <dac/Logger.hpp>

#include "Settings.hpp"
#include "helpers/Consts.hpp"

namespace brave {

// ====================================================================== //
// ====================================================================== //
// Constructor
// ====================================================================== //

Joint::Joint(const std::string& name)
    : m_name(name), m_model(1.f), m_offset(0.f), m_parent(nullptr) {}


// ====================================================================== //
// ====================================================================== //
// Get if joint is not root and prompt an alert if is not
// ====================================================================== //

bool Joint::isNotRoot() const {
  bool isRoot = (m_name == "Root");
  if (!isRoot) { dErr("NON Root joints shouldn't call that method"); }
  return !isRoot;
}

// ====================================================================== //
// ====================================================================== //
// Getter for name
// ====================================================================== //

std::string Joint::name() const { return m_name; }

// ====================================================================== //
// ====================================================================== //
// Getter for current position
// ====================================================================== //

glm::vec3 Joint::currPos() const { return m_model[3].xyz(); }

// ====================================================================== //
// ====================================================================== //
// Getter for number of frames
// ====================================================================== //

int Joint::frames() const {
  if (auto rotFrames = rots().size(); rotFrames != trans().size()) {

    if (!Settings::quiet)
      dErr("The number of frames differ between Rotations and Translations");
    return -1;

  } else {
    return rotFrames;
  }
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for rotations
// ====================================================================== //

std::vector<glm::vec3> Joint::rots() const { return m_rots; }
glm::vec3 Joint::rots(unsigned int idx) const { return m_rots.at(idx); }
void      Joint::rots(const std::vector<glm::vec3>& Rs) { m_rots = Rs; }
void      Joint::addRot(const glm::vec3& R) { m_rots.push_back(R); }

void Joint::updateRot(unsigned int frame, float scale) {
  // 1.- Compute hierarchy
  if (m_parent) { m_model = m_parent->model(); }
  // 2.- Apply local offset
  Math::translate(m_model, m_offset * scale);
  // 3.- Apply local rotation
  if (m_name != "EndSite") { Math::rotateXYZ(m_model, rots(frame)); }
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for translations
// ====================================================================== //

std::vector<glm::vec3> Joint::trans() const { return m_trans; }
glm::vec3 Joint::trans(unsigned int idx) const { return m_trans.at(idx); }
void      Joint::trans(const std::vector<glm::vec3>& Ts) { m_trans = Ts; }
void      Joint::addTrans(const glm::vec3& T) { m_trans.push_back(T); }

void Joint::updateTrans(unsigned int frame, const glm::vec3& despl) {
  if (isNotRoot()) return;

  auto nextPos = trans(frame) + despl;
  nextPos.y    = 0.f;

  m_model = glm::mat4(1.f);
  Math::translate(m_model, nextPos);
}

// ====================================================================== //
// ====================================================================== //
// Getter for model
// ====================================================================== //

glm::mat4 Joint::model() const { return m_model; }
void      Joint::model(const glm::mat4 newModel) { m_model = newModel; }

// ====================================================================== //
// ====================================================================== //
// G/Setter for offset
// ====================================================================== //

glm::vec3 Joint::offset() const { return m_offset; }
void      Joint::offset(const glm::vec3& newOffset) { m_offset = newOffset; }

// ====================================================================== //
// ====================================================================== //
// G/Setter for channels
// ====================================================================== //
std::vector<unsigned int> Joint::channels() const { return m_channels; }
unsigned int              Joint::channels(unsigned int idx) const {
  return m_channels.at(idx);
}
void Joint::addChannel(const unsigned int channel) {
  m_channels.push_back(channel);
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for parent
// ====================================================================== //

std::shared_ptr<Joint> Joint::parent() const { return m_parent; }
void Joint::parent(std::shared_ptr<Joint> newParent) { m_parent = newParent; }

// ====================================================================== //
// ====================================================================== //
// Define draw process
// ====================================================================== //

void Joint::draw() {

  auto bone = [&]() {
    glm::mat4 boneModel(1.f);
    // T
    auto bonePos = (this->currPos() + m_parent->currPos()) * 0.5f;
    Math::translate(boneModel, bonePos);
    // R
    auto vJ = glm::normalize(this->currPos() - m_parent->currPos());
    auto vB = glm::normalize(bonePos + glm::vec3(0, .5f, 0) - bonePos); // !
    Math::rotate(boneModel, glm::angle(vB, vJ), glm::cross(vB, vJ));
    // S
    auto distJ = glm::distance(this->currPos(), m_parent->currPos()) * 0.5f;
    Math::scale(boneModel, Math::unitVecY * distJ);
    // Draw
    Renderable::getByName("Bone")->model(boneModel);
    Renderable::getByName("Bone")->draw();
  };

  if (m_parent) {
    if (m_parent->name() == "Head") {
      auto head = Renderable::getByName("Monkey");
      head->model(m_model);
      head->draw();
    }
    bone();
  }
}

} // namespace brave
