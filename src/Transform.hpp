#pragma once

#include <memory>
#include "Math.hpp"

namespace brave {

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

  // Get a vector as result of (v.x*RIGHT + v.y*UP + v.z*FRONT)
  glm::vec3 RUF(const glm::vec3 value);

  // Go methods implies a movement and rotation of the transform
  // !!! SHIT: This will NOT work (so easy as we think) because we need the camera front vector.
  void goForward();
  void goBackward();
  void goRight();
  void goLeft();
  void goUp();

  // Get the front of the transform with Y == 0.0f
  // glm::vec3 frontXZ() const;

  // Generate matrix with transform values or return override matrix
  // if is defined
  glm::mat4 asMatrix() const;



  // -----------------------
};

} // namespace brave
