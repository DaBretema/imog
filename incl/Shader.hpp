#pragma once
#include "wrap/Math.hpp"

#include <string>

namespace BRAVE {

class Shader {


private:
  std::string  m_name;
  unsigned int m_program;


public:
  Shader(const std::string& name,
         const std::string& vertexPath,
         const std::string& geomPath,
         const std::string& fragPath);


  /// loadShader return the OpenGL state machine ID for a filePath shader, if
  /// source compilation fails returns 0
  unsigned int loadShader(const std::string& filePath, unsigned int type);

  /// getUniformID returns the ID of the uniform associated to that string
  int getUniformID(const char* uniformName);

  /// bind set this program as active and use it to draw
  void bind();

  /// unbind unset this program as active so won't be used to draw
  void unbind();

  /// uMat4 upload a mat4 (view, proj, etc.) to the shader
  void uMat4(const std::string& uniformName, const glm::mat4& mat);

  /// uFloat1 upload a float (height, intensity, etc.) to the shader
  void uFloat1(const std::string& uniformName, float f);

  /// uFloat3 upload a vec3 (lightPos, color, etc.) to the shader
  void uFloat3(const std::string& uniformName, float f1, float f2, float f3);

  /// uFloat3 upload a vec3 (lightPos, color, etc.) to the shader
  void uFloat3(const std::string& uniformName, const glm::vec3& floats);
};

} // namespace BRAVE
