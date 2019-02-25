#pragma once

#include "Math.hpp"

namespace brave {

struct Transform {
  Transform();

  glm::vec3 pos;
  glm::vec3 rot;
  glm::vec3 scl;

  float     rotAngle;
  glm::vec3 rotAxis;

  glm::mat4 model() const;
  glm::vec3 front() const;
};

} // namespace brave
