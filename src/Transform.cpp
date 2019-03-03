#include "Transform.hpp"

#include <dac/Logger.hpp>

namespace brave {

Transform::Transform()
    : pos(0.f), rot(0.f), scl(1.f), rotAngle(0.f), rotAxis(0.f) {}

glm::vec3 Transform::front() const { return this->asMatrix()[2]; }

glm::mat4 Transform::asMatrix() const {
  if (overrideMatrix != glm::mat4(0.f)) { return overrideMatrix; }

  glm::mat4 aux(1.f);
  Math::translate(aux, pos);
  (rotAxis != glm::vec3(0.f)) ? Math::rotate(aux, rotAngle, rotAxis)
                              : Math::rotateXYZ(aux, rot);
  Math::scale(aux, scl);
  return aux;
}

} // namespace brave
