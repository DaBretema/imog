#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/component_wise.hpp>

#include <vector>
#include <string>
#include <stdexcept>


namespace brave {

class Math {
public:
  // Unit vectors
  static glm::vec3 unitVecX;
  static glm::vec3 unitVecY;
  static glm::vec3 unitVecZ;

  // Wrap mat4 translation
  static glm::mat4 translate(glm::mat4& mat, const glm::vec3& T);
  static glm::mat4 translate(glm::mat4& mat, float x, float y, float z);

  // Wrap mat4 rotation
  static glm::mat4 rotate(glm::mat4& mat, float angle, const glm::vec3& axes);
  static glm::mat4 rotateXYZ(glm::mat4& mat, const glm::vec3& angles);
  static glm::mat4 rotateXYZ(glm::mat4& mat, float x, float y, float z);

  // Wrap mat4 scale
  static glm::mat4 scale(glm::mat4& mat, const glm::vec3& S);
  static glm::mat4 scale(glm::mat4& mat, float x, float y, float z);

  // Numeric string vector of size 3(-1) to glm::vec3
  static glm::vec2 glmVec2FromStr(const std::vector<std::string>& strVec);
  // Numeric string vector of size 4(-1) to glm::vec3
  static glm::vec3 glmVec3FromStr(const std::vector<std::string>& strVec);
  // Numeric string vector of size 5(-1) to glm::vec3
  static glm::vec4 glmVec4FromStr(const std::vector<std::string>& strVec);
};

} // namespace brave
