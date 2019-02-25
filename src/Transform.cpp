#include "Transform.hpp"

namespace brave {

Transform::Transform() : pos(0.f), rot(0.f), scl(1.f) {}


glm::mat4 Transform::model() const {
  glm::mat4 model(1.f);

  Math::translate(model, pos);
  Math::rotateXYZ(model, rot);
  Math::scale(model, scl);

  return model;
}


glm::vec3 Transform::front() const {
  auto auxFront = this->model()[2];
  return -1.f * auxFront; // Taking into account Z-Negative.
}

} // namespace brave
