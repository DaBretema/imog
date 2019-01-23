
#include <vector>

#include <Dac/Logger.hpp>
#include <Dac/Strings.hpp>

#include "../incl/Shader.hpp"
#include "../incl/wrap/Glad.hpp"


// ==========================================================================

#define addShaderToProgram(program, path, type) \
  {                                             \
    unsigned int s = loadShader(path, type);    \
    glAttachShader(program, s);                 \
    glDeleteShader(s);                          \
  }

// ==========================================================================

namespace BRAVE {


Shader::Shader(const std::string& name,
               const std::string& vertexPath,
               const std::string& geomPath,
               const std::string& fragPath)
    : m_name(name) {

  // Request program to opengl
  m_program = glCreateProgram();
  {
    // MUST Shaders
    addShaderToProgram(m_program, vertexPath, GL_VERTEX_SHADER);
    addShaderToProgram(m_program, fragPath, GL_FRAGMENT_SHADER);

    // OPTIONAL Shaders
    if (!geomPath.empty()) {
      addShaderToProgram(m_program, geomPath, GL_GEOMETRY_SHADER);
    }
  }

  // Link program
  glLinkProgram(m_program);

  // Check errors
  int linked;
  glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
  if (!linked) {
    int len = 0;
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &len);
    std::vector<char> msg(len);
    glGetProgramInfoLog(m_program, len, &len, &msg[0]);
    DacLog_ERR("Program {} \n{}", m_name, std::string(msg.data(), msg.size()));
    glDeleteProgram(m_program);
  }
}


unsigned int Shader::loadShader(const std::string& filePath,
                                unsigned int       type) {

  // Data
  std::string sourceStr  = DAC::Strings::fromFile(filePath);
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
    DacLog_ERR("Shader {} \n{}", filePath, std::string(msg.data(), msg.size()));
    glDeleteShader(shader);
    return 0;
  }

  // Return the ID gived by OpenGL
  return shader;
}

int Shader::getUniformID(const char* uniformName) {
  return glGetUniformLocation(m_program, uniformName);
}

void Shader::bind() { glUseProgram(m_program); }
void Shader::unbind() { glUseProgram(0); }

void Shader::uMat4(const std::string& uniformName, const glm::mat4& mat) {
  int uID = this->getUniformID(uniformName.data());
  glProgramUniformMatrix4fv(m_program, uID, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::uFloat1(const std::string& uniformName, float f) {
  int uID = this->getUniformID(uniformName.data());
  glProgramUniform1f(m_program, uID, f);
}

void Shader::uFloat3(const std::string& uniformName,
                     float              f1,
                     float              f2,
                     float              f3) {
  int uID = this->getUniformID(uniformName.data());
  glProgramUniform3f(m_program, uID, f1, f2, f3);
}

void Shader::uFloat3(const std::string& uniformName, const glm::vec3& floats) {
  int uID = this->getUniformID(uniformName.data());
  glProgramUniform3fv(m_program, uID, 1, glm::value_ptr(floats));
}

} // namespace BRAVE
