#include "Transform.hpp"

#include <dac/Logger.hpp>

namespace brave {

Transform::Transform()
    : pos(0.f), rot(0.f), scl(1.f), rotAngle(0.f), rotAxis(0.f) {}

glm::vec3 Transform::front() const { return this->model()[2]; }

glm::mat4 Transform::model() const {
  auto model = glm::translate(glm::mat4(1.f), pos);

  if (rotAxis != glm::vec3(0.f)) {
    // dInfo("Rotating over an axis for bones :D");
    model = glm::rotate(model, rotAngle, rotAxis);
  } else {
    model = glm::rotate(model, glm::radians(rot.z), Math::unitVecZ);
    model = glm::rotate(model, glm::radians(rot.x), Math::unitVecX);
    model = glm::rotate(model, glm::radians(rot.y), Math::unitVecY);
  }

  model = glm::scale(model, scl);
  return model;
}

} // namespace brave
