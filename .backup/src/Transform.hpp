#pragma once

#include <memory>
#include "Math.hpp"

namespace brave {

struct Transform {
  Transform();

  // Get the front of the transform, computed from transform as matrix
  glm::vec3 front() const;

  // Get the front of the transform with Y == 0.0f
  glm::vec3 frontXZ() const;

  // Generate matrix with transform values or return override matrix
  // if is defined
  glm::mat4 asMatrix() const;

  // 3D position vector
  glm::vec3 pos;

  // 3D scale vector
  glm::vec3 scl;

  // If rotAxis is zero Rotate...
  // Angle X arround Axis (1,0,0)
  // Angle Y arround Axis (0,1,0)
  // Angle Z arround Axis (0,0,1)
  glm::vec3 rot;

  // -----------------------


  // If rotAxis is NOT zero Rotate rotAngle arround it
  float rotAngle;

  // If rotAxis is NOT zero Rotate rotAngle arround it
  glm::vec3 rotAxis;

  // If its defined, ignore other values of transform
  // and use this as matrix when transform is requested as matrix
  glm::mat4 overrideMatrix{0.f}; //! MUST be initialize to {0.f}


  // -----------------------
};

} // namespace brave
