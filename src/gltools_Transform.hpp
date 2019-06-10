#pragma once

#include <memory>
#include "gltools_Math.hpp"

namespace imog {

struct Transform {
  Transform();

  glm::vec3 pos;
  glm::vec3 scl;
  glm::vec3 rot;
  float     rotAngle; // arround rotAxis
  glm::vec3 rotAxis;  // if is defined ignore 'rot'

  // If its defined, ignore other values of transform
  // and use this as matrix when transform is requested as matrix
  glm::mat4 overrideMatrix{0.f}; //! MUST be initialize to {0.f}

  // Get transform direction vectors
  glm::vec3 up() const;
  glm::vec3 right() const;
  glm::vec3 front() const;

  // Generate matrix with transform values or return override matrix
  // if is defined
  glm::mat4 asMatrix() const;



  // -----------------------
};

} // namespace imog
