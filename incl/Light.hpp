#pragma once

#include "wrap/Math.hpp"

namespace BRAVE {

class Light {

private:
  glm::vec3 m_pos;
  glm::vec3 m_color;

public:
  // Ctor
  Light(const glm::vec3& t_pos, const glm::vec3& t_color);

  // Pos
  glm::vec3 pos() const;
  void      pos(const glm::vec3& newPos);

  // Color
  glm::vec3 color() const;
  void      color(const glm::vec3& newColor);
};

} // namespace BRAVE
