
#include <vector>

#include <glad/glad.h>

#include <dac/Logger.hpp>
#include <dac/Strings.hpp>

#include "../incl/Shader.hpp"


namespace BRAVE {

// ====================================================================== //
// ====================================================================== //
// Return the OpenGL state machine ID for a filePath
// shader, if source compilation fails returns 0
// ====================================================================== //
unsigned int Shader::loadShader(const std::string& filePath,
                                unsigned int       type) {
  // Data
  std::string sourceStr  = dac::Strings::fromFile(filePath);
  const char* sourceChar = sourceStr.c_str();
  int         sourceLen  = sourceStr.length();

  // Request shader to opengl
  unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &sourceChar, &sourceLen);
  glCompileShader(shader);

  // Check errors
  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    int len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    std::vector<char> msg(len);
    glGetShaderInfoLog(shader, len, &len, &msg[0]);
    dlog::err("Shader {} \n{}", filePath, std::string(msg.data(), msg.size()));
    glDeleteShader(shader);
    return 0;
  }

  // Return the ID gived by OpenGL
  return shader;
}


// ====================================================================== //
// ====================================================================== //
// Param constructor
//
// 1. Create new program
// 2. Compile shaders:
//    - required = vertex, fragment.
//    - optional = geometry.
// 3. Apped it to a created program
// 4. Link program
// 5. Verify that its linked (if not, delete it and prompt an alert)
// ====================================================================== //

Shader::Shader(const std::string& name,
               const std::string& vertexPath,
               const std::string& geomPath,
               const std::string& fragPath)
    : m_name(name) {

  auto addShader = [&](unsigned int p, const std::string& s, unsigned int t) {
    if (!s.empty()) {
      unsigned int sID = loadShader(s, t);
      glAttachShader(p, sID);
      glDeleteShader(sID);
    }
  };

  // 1. Request program to opengl
  m_program = glCreateProgram();

  // 2&3. MUST Shaders
  addShader(m_program, vertexPath, GL_VERTEX_SHADER);
  addShader(m_program, fragPath, GL_FRAGMENT_SHADER);

  // 2&3. OPTIONAL Shaders
  addShader(m_program, geomPath, GL_GEOMETRY_SHADER);

  // 4. Link program
  glLinkProgram(m_program);

  // 5. Check errors
  int linked;
  glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
  if (!linked) {
    int len = 0;
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &len);
    std::vector<char> msg(len);
    glGetProgramInfoLog(m_program, len, &len, &msg[0]);
    dlog::err("Program {} \n{}", m_name, std::string(msg.data(), msg.size()));
    glDeleteProgram(m_program);
  }
}


// ====================================================================== //
// ====================================================================== //
// Bind set this program as active and use it to draw
// ====================================================================== //

void Shader::bind() { glUseProgram(m_program); }

// ====================================================================== //
// ====================================================================== //
// Unbind unset this program as active so won't be used to draw
// ====================================================================== //
void Shader::unbind() { glUseProgram(0); }


// ====================================================================== //
// ====================================================================== //
// Returns the ID of the uniform associated to that string
// ====================================================================== //

int Shader::getUniformID(const char* uniformName) {
  return glGetUniformLocation(m_program, uniformName);
}


// ====================================================================== //
// ====================================================================== //
// Upload a mat4 (view, proj, etc.) to the shader
// ====================================================================== //
void Shader::uMat4(const std::string& uniformName, const glm::mat4& mat) {
  int uID = this->getUniformID(uniformName.data());
  glProgramUniformMatrix4fv(m_program, uID, 1, GL_FALSE, glm::value_ptr(mat));
}


// ====================================================================== //
// ====================================================================== //
// Upload a float (height, intensity, etc.) to the shader
// ====================================================================== //

void Shader::uFloat1(const std::string& uniformName, float f) {
  int uID = this->getUniformID(uniformName.data());
  glProgramUniform1f(m_program, uID, f);
}

// ====================================================================== //
// ====================================================================== //
// Upload a vec3 (lightPos, color, etc.) to the shader
// ====================================================================== //

void Shader::uFloat3(const std::string& uniformName,
                     float              f1,
                     float              f2,
                     float              f3) {
  int uID = this->getUniformID(uniformName.data());
  glProgramUniform3f(m_program, uID, f1, f2, f3);
}

// ====================================================================== //
// ====================================================================== //
// Upload a vec3 (lightPos, color, etc.) to the shader
// ====================================================================== //

void Shader::uFloat3(const std::string& uniformName, const glm::vec3& floats) {
  int uID = this->getUniformID(uniformName.data());
  glProgramUniform3fv(m_program, uID, 1, glm::value_ptr(floats));
}

} // namespace BRAVE
