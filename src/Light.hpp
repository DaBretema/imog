#pragma once

#include "Math.hpp"

namespace BRAVE {

class Light {

private:
  glm::vec3 m_pos;
  glm::vec3 m_color;

public:
  // Param constructor
  Light(const glm::vec3& pos, const glm::vec3& color);

  // G/Setter for pos
  glm::vec3 pos() const;
  void      pos(const glm::vec3& newPos);

  // G/Setter for color
  glm::vec3 color() const;
  void      color(const glm::vec3& newColor);
};

} // namespace BRAVE
