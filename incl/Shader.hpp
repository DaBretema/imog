#pragma once

#include <string>

#include "wrap/Math.hpp"


namespace BRAVE {

class Shader {

private:
  std::string  m_name;
  unsigned int m_program;

  // Return the OpenGL state machine ID for a filePath
  // shader, if source compilation fails returns 0
  unsigned int loadShader(const std::string& filePath, unsigned int type);


public:
  // Param constructor
  //
  // 1. Create new program
  // 2. Compile shaders:
  //    - required = vertex, fragment.
  //    - optional = geometry.
  // 3. Apped it to a created program
  // 4. Link program
  // 5. Verify that its linked (if not, delete it and prompt an alert)
  Shader(const std::string& name,
         const std::string& vertexPath,
         const std::string& geomPath,
         const std::string& fragPath);


  // Bind set this program as active and use it to draw
  void bind();

  // Unbind unset this program as active so won't be used to draw
  void unbind();


  // Returns the ID of the uniform associated to that string
  int getUniformID(const char* uniformName);

  // Upload a mat4 (view, proj, etc.) to the shader
  void uMat4(const std::string& uniformName, const glm::mat4& mat);

  // Upload a float (height, intensity, etc.) to the shader
  void uFloat1(const std::string& uniformName, float f);

  // Upload a vec3 (lightPos, color, etc.) to the shader
  void uFloat3(const std::string& uniformName, float f1, float f2, float f3);

  // Upload a vec3 (lightPos, color, etc.) to the shader
  void uFloat3(const std::string& uniformName, const glm::vec3& floats);
};

} // namespace BRAVE
