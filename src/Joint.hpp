#pragma once

#include <memory>

#include "Math.hpp"
#include "Renderable.hpp"

namespace brave {

class Joint {

private:
  std::string               m_name;
  std::vector<glm::vec3>    m_rots;
  std::vector<glm::vec3>    m_trans;
  glm::mat4                 m_model;
  glm::vec3                 m_offset;
  std::vector<unsigned int> m_channels;
  std::shared_ptr<Joint>    m_parent;

public:
  // Constructor
  Joint(const std::string& name);


private:
  // Get if joint is not root and prompt an alert if is not
  bool isNotRoot() const;

public:
  // Getter for name
  std::string name() const;

  // Getter for current position
  glm::vec3 currPos() const;

  // Getter for number of frames
  int frames() const;

  // G/Setter for rotations
  std::vector<glm::vec3> rots() const;
  glm::vec3              rots(unsigned int idx) const;
  void                   rots(const std::vector<glm::vec3>& Rs);
  void                   addRot(const glm::vec3& R);
  void                   updateRot(unsigned int frame);

  // G/Setter for translations
  std::vector<glm::vec3> trans() const;
  glm::vec3              trans(unsigned int idx) const;
  void                   trans(const std::vector<glm::vec3>& TS);
  void                   addTrans(const glm::vec3& T);
  void updateTrans(unsigned int frame, const glm::vec3& despl);

  // Getter for model
  glm::mat4 model() const;
  // void      model(const glm::mat4 newModel);

  // G/Setter for offset
  glm::vec3 offset() const;
  void      offset(const glm::vec3& newOffset);

  // G/Setter for channels
  std::vector<unsigned int> channels() const;
  unsigned int              channels(unsigned int idx) const;
  void                      addChannel(const unsigned int channel);

  // G/Setter for parent
  std::shared_ptr<Joint> parent() const;
  void                   parent(std::shared_ptr<Joint> parentJoint);

  // Define draw process
  void draw();
};

} // namespace brave
