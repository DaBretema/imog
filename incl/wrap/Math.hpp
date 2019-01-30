#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>


namespace BRAVE {
namespace Math {

  const glm::vec3 VEC_X{1.f, 0.f, 0.f};
  const glm::vec3 VEC_Y{0.f, 1.f, 0.f};
  const glm::vec3 VEC_Z{0.f, 0.f, 1.f};

  // Wrap mat4 translation
  static inline void dTranslate(glm::mat4& mat, const glm::vec3& T) {
    mat = glm::translate(mat, T);
  }

  // Wrap mat4 rotation
  static inline void dRotate(glm::mat4& mat, const glm::vec3& R) {
    mat = glm::rotate(mat, glm::radians(R.z), VEC_Z);
    mat = glm::rotate(mat, glm::radians(R.y), VEC_Y);
    mat = glm::rotate(mat, glm::radians(R.x), VEC_X);
  }

  // Wrap mat4 scale
  static inline void dScale(glm::mat4& mat, const glm::vec3& S) {
    mat = glm::scale(mat, S);
  }

  // Numeric string vector of size 3 to glm::vec3
  static inline glm::vec3 str3ToVec3(const std::vector<std::string>& str) {
    auto x = std::stof(str.at(1));
    auto y = std::stof(str.at(2));
    auto z = std::stof(str.at(3));
    return glm::vec3{x, y, z};
  }

} // namespace Math
} // namespace BRAVE
