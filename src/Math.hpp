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
#include <stdexcept>


namespace BRAVE {

class Math {
public:
  static glm::vec3 unitVecX;
  static glm::vec3 unitVecY;
  static glm::vec3 unitVecZ;

  // Wrap mat4 translation
  static void translate(glm::mat4& mat, const glm::vec3& T);
  static void translate(glm::mat4& mat, float x, float y, float z);

  // Wrap mat4 rotation
  static void rotate(glm::mat4& mat, const glm::vec3& R);
  static void rotate(glm::mat4& mat, float x, float y, float z);

  // Wrap mat4 scale
  static void scale(glm::mat4& mat, const glm::vec3& S);
  static void scale(glm::mat4& mat, float x, float y, float z);

  // Numeric string vector of size 3(-1) to glm::vec3
  static glm::vec2 glmVec2FromStr(const std::vector<std::string>& strVec);
  // Numeric string vector of size 4(-1) to glm::vec3
  static glm::vec3 glmVec3FromStr(const std::vector<std::string>& strVec);
  // Numeric string vector of size 5(-1) to glm::vec3
  static glm::vec4 glmVec4FromStr(const std::vector<std::string>& strVec);
};

} // namespace BRAVE
