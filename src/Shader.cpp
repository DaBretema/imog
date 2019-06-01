#include "Shader.hpp"

#include <vector>

#include <glad/glad.h>

#include "Files.hpp"
#include "Logger.hpp"
#include "Strings.hpp"
#include "Settings.hpp"
#include "helpers/Consts.hpp"


namespace brave {

// * static

// ====================================================================== //
// ====================================================================== //
// Global pool for shaders
// ====================================================================== //

std::vector<std::shared_ptr<Shader>>          Shader::pool{};
std::unordered_map<std::string, unsigned int> Shader::poolIndices{};

// ====================================================================== //
// ====================================================================== //
// Get a shared ptr to the shader from the global pool
// by the concatenation of shaders paths
// ====================================================================== //

std::shared_ptr<Shader> Shader::getFromCache(const std::string& paths) {
  if (poolIndices.count(paths) > 0) { return pool[poolIndices[paths]]; }
  return nullptr;
}

// ====================================================================== //
// ====================================================================== //
// Get a shared ptr to the shader from the global pool by name
// ====================================================================== //

std::shared_ptr<Shader> Shader::getByName(const std::string& name) {
  auto _name = Strings::toLower(name);
  if (poolIndices.count(_name) > 0) { return pool[poolIndices[_name]]; }

  if (!Settings::quiet) LOGE("Zero entries @ shaders pool with name {}.", name);
  return nullptr;
}

// ====================================================================== //
// ====================================================================== //
// Create a new shader if it isn't on the gloabl pool
// ====================================================================== //

std::shared_ptr<Shader> Shader::create(const std::string& name,
                                       const std::string& vertexPath,
                                       const std::string& geomPath,
                                       const std::string& fragPath) {

  if (vertexPath.empty() || fragPath.empty()) {
    if (!Settings::quiet) LOGE("Undefined non-optional shaders");
    return nullptr;
  }

  std::vector<std::string> shaderfiles = {vertexPath, fragPath};
  if (!geomPath.empty()) { shaderfiles.push_back(geomPath); }
  for (const auto& fp : shaderfiles) {
    if (!Files::ok(fp, true)) { return nullptr; }
  }

  auto paths = vertexPath + geomPath + fragPath;
  if (auto S = getFromCache(paths)) { return S; }

  auto _name = Strings::toLower(name);
  pool.push_back(
      std::make_shared<Shader>(_name, vertexPath, geomPath, fragPath));

  auto idx           = pool.size() - 1;
  poolIndices[_name] = idx;
  poolIndices[paths] = idx;
  return pool.at(idx);
}

// ====================================================================== //
// ====================================================================== //
// Create a new shader by a gived name, searching it in default forlder
// ====================================================================== //

std::shared_ptr<Shader> Shader::createByName(const std::string& name,
                                             bool               hasGeometry,
                                             bool hasTesselation) {
  auto        _name = Strings::toLower(name);
  std::string sPath = Paths::shaders + _name + "/" + _name;
  std::string sV    = sPath + ".vert";
  std::string sG    = (hasGeometry) ? sPath + ".geom" : "";
  // std::string sTC   = (hasTesselation) ? sPath + ".tesc" : "";
  // std::string sTE   = (hasTesselation) ? sPath + ".tese" : "";
  std::string sF = sPath + ".frag";
  // return Shader::create(_name, sV, sG, sTC, sTE, sF);
  return Shader::create(_name, sV, sG, sF);
}

// ====================================================================== //
// ====================================================================== //
// Update all shaders of the pool
// ====================================================================== //

void Shader::poolUpdate(const std::shared_ptr<Camera>& camera) {
  for (const auto& s : pool) { s->update(camera); }
}

// * private


// ====================================================================== //
// ====================================================================== //
// Return the OpenGL state machine ID for a filePath
// shader, if source compilation fails returns 0
// ====================================================================== //

unsigned int Shader::loadShader(const std::string& filePath,
                                unsigned int       type) {
  // Data
  std::string sourceStr  = Strings::fromFile(filePath);
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
    LOGE("Shader {} \n{}", filePath, std::string(msg.data(), msg.size()));
    glDeleteShader(shader);
    return 0;
  }

  // Return the ID gived by OpenGL
  return shader;
}


// * public


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
    if (!s.empty() && Files::ok(s, true)) {
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
    LOGE("Program {} \n{}", m_name, std::string(msg.data(), msg.size()));
    glDeleteProgram(m_program);
  }
}

// ====================================================================== //
// ====================================================================== //
// Destructor
// ====================================================================== //

Shader::~Shader() {
  if (!Settings::quiet) LOGD("Destroyed @ {}.{}", m_program, m_name);
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
// Update upload to the shader camera data
// ====================================================================== //

void Shader::update(const std::shared_ptr<Camera>& camera) {

  uFloat3("u_clearColor", Settings::clearColor);

  if (camera) {
    uMat4("u_matV", camera->view());
    uMat4("u_matP", camera->proj());
    uMat4("u_matVP", camera->viewproj());
  }
}

// ====================================================================== //
// ====================================================================== //
// Returns the ID of the uniform associated to that string,
// if its cached, return from cache, else request it to OpenGL
// and store on cache.
// ====================================================================== //

int Shader::uniform(const std::string& uName) {

  if (m_uCache.count(uName) > 0) { return m_uCache.at(uName); }
  int uLoc = glGetUniformLocation(m_program, uName.c_str());

  if (uLoc > -1) {
    m_uCache.insert({uName, uLoc});
  } else if (!Settings::quiet && !m_alertCache[uName]) {
    LOGE("@{}: not found/used \"{}\"", m_name, uName);
    m_alertCache[uName] = true;
  }

  return uLoc;
}


// ====================================================================== //
// ====================================================================== //
// Upload a mat4 (view, proj, ...)
// ====================================================================== //

void Shader::uMat4(const std::string& uName, const glm::mat4& mat) {
  glProgramUniformMatrix4fv(
      m_program, uniform(uName), 1, GL_FALSE, glm::value_ptr(mat));
}

// ====================================================================== //
// ====================================================================== //
// Upload a float (height, intensity, ...)
// ====================================================================== //

void Shader::uFloat1(const std::string& uName, float f) {
  glProgramUniform1f(m_program, uniform(uName), f);
}

// ====================================================================== //
// ====================================================================== //
// Upload a vec3 (lightPos, color, ...)
// ====================================================================== //

void Shader::uFloat3(const std::string& uName, float f1, float f2, float f3) {
  glProgramUniform3f(m_program, uniform(uName), f1, f2, f3);
}

// ====================================================================== //
// ====================================================================== //
// Upload a vec3 (lightPos, color, ...)
// ====================================================================== //

void Shader::uFloat3(const std::string& uName, const glm::vec3& floats) {
  glProgramUniform3fv(m_program, uniform(uName), 1, glm::value_ptr(floats));
}

// ====================================================================== //
// ====================================================================== //
// Upload a int1 (textures, ...)
// ====================================================================== //

void Shader::uInt1(const std::string& uName, int i) {
  glProgramUniform1i(m_program, uniform(uName), i);
}

} // namespace brave
