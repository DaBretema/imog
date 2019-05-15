#include "Transform.hpp"



namespace brave {

// ====================================================================== //
// ====================================================================== //
// Constructor
// ====================================================================== //

Transform::Transform()
    : pos(0.f), scl(1.f), rot(0.f), rotAngle(0.f), rotAxis(0.f) {}

// ====================================================================== //
// ====================================================================== //
// Get the front of the transform, computed from transform as matrix
// ====================================================================== //

glm::vec3 Transform::up() const {
  return glm::normalize(glm::cross(this->front(), this->right()));
}
glm::vec3 Transform::right() const {
  return glm::normalize(glm::cross(this->front(), Math::unitVecY));
}
glm::vec3 Transform::front() const {
  return glm::normalize(this->asMatrix()[2]);
}

// ====================================================================== //
// ====================================================================== //
// Generate matrix with transform values or return override matrix
// if is defined
// ====================================================================== //

glm::mat4 Transform::asMatrix() const {
  if (overrideMatrix != glm::mat4(0.f)) { return overrideMatrix; }

  glm::mat4 aux(1.f);
  Math::translate(aux, allowedTrans * pos);

  (rotAxis != glm::vec3(0.f))
      ? Math::rotate(aux, rotAngle, allowedRots * rotAxis)
      : Math::rotateXYZ(aux, allowedRots * rot);

  Math::scale(aux, scl);
  return aux;
}

} // namespace brave
