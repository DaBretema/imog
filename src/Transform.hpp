#pragma once

#include <memory>
#include "Math.hpp"

namespace brave {

struct Transform {


  Transform();
  glm::vec3 front() const;
  glm::mat4 asMatrix() const;

  // If its defined, ignore other values of transform
  // and use this as matrix when transform is requested as matrix
  glm::mat4 overrideMatrix{0.f}; //! MUST be initialize to {0.f}


  glm::vec3 pos;
  glm::vec3 scl;

  // If rotAxis is zero Rotate...
  // Angle X arround Axis (1,0,0)
  // Angle Y arround Axis (0,1,0)
  // Angle Z arround Axis (0,0,1)
  glm::vec3 rot;

  // If rotAxis is NOT zero Rotate rotAngle arround it
  glm::vec3 rotAxis;
  float     rotAngle;
};

} // namespace brave
