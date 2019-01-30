
#include "../incl/Settings.hpp"

#include <dac/Logger.hpp>

#include <fstream>
#include <functional>


namespace BRAVE {


// ========================================================================= //
// ========================================================================= //
// JSON Boilerplate
// ========================================================================= //

// --- C++ Types --- //
#define stdParse(var, defVal) \
  try {                       \
    var = m_json.at(#var);    \
  } catch (json::exception&) { var = defVal; }

// --- GLM Types --- //
#define glmParse(var, defVal)                                         \
  try {                                                               \
    auto v = m_json.at(#var);                                         \
    if (!v.is_array()) {                                              \
      throw json::other_error::create(501, "An array was expected");  \
    }                                                                 \
    for (auto i = 0u; i < v.size(); ++i) { var[i] = v.at(i); } \
  } catch (json::exception&) { var = defVal; }


// ========================================================================= //
// ========================================================================= //
// Private variables definition
// ========================================================================= //

bool             Settings::m_corrupted;
std::string      Settings::m_path;
nlohmann::json   Settings::m_json;
dac::FileWatcher Settings::m_filewatcher;


// ====================================================================== //
// ====================================================================== //
// Public variables definition
// ====================================================================== //

int         Settings::openglMajorV;
int         Settings::openglMinorV;
int         Settings::windowWidth;
int         Settings::windowHeight;
std::string Settings::windowTitle;
glm::vec3   Settings::clearColor;
float       Settings::mouseSensitivity;
bool        Settings::pollEvents;
glm::vec3   Settings::mainCameraPos;
float       Settings::mainCameraSpeed;
glm::vec3   Settings::mainLightPos;
glm::vec3   Settings::mainLightColor;


// ====================================================================== //
// ====================================================================== //
// Parse process
// ====================================================================== //

void Settings::init(const std::string& filePath) {
  if (!std::fstream(filePath).good()) {
    dlog::err("Settings file not found @ '{}' path", filePath);
    return;
  }

  m_path = filePath;
  m_filewatcher.path(m_path);
  m_filewatcher.callback([&](std::fstream f) {
    try {
      m_json = json::parse(f);
      // -----------------------//

      stdParse(openglMajorV, 4);
      stdParse(openglMinorV, 5);
      stdParse(windowWidth, 800);
      stdParse(windowHeight, 600);
      stdParse(windowTitle, "Brave engine");
      glmParse(clearColor, glm::vec3(0.2, 0.3, 0.3));
      stdParse(mouseSensitivity, 1.0);
      stdParse(pollEvents, false);
      glmParse(mainCameraPos, glm::vec3(0.f));
      stdParse(mainCameraSpeed, 0.1f);
      glmParse(mainLightPos, glm::vec3(0, 10, 0));
      glmParse(mainLightColor, glm::vec3(0.5, 0.5, 0.25));

      // -----------------------//
      m_corrupted = false;
    } catch (json::exception& e) {
      m_corrupted = true;
      dlog::err("'{}' Bad parsing:\n{}", m_path, e.what());
    }
  });
  m_filewatcher.launch();
}


// ====================================================================== //
// ====================================================================== //
// Print object values
// ====================================================================== //
#define stdPrint(var) dlog::print("{} => {}", 33, var);
#define glmPrint(var) dlog::print("{}: ({},{},{})", 33, var.x, var.y, var.z);

void Settings::dump() {

  dlog::print("{2} - [{1}]", 33, "hola");

  dlog::print("\nSETTINGS ({}) Corruption={}\n  ---", m_path, m_corrupted);
  stdPrint(openglMajorV);
  stdPrint(openglMinorV);
  stdPrint(windowWidth);
  stdPrint(windowHeight);
  stdPrint(windowTitle);
  glmPrint(clearColor);
  stdPrint(mouseSensitivity);
  stdPrint(pollEvents);
  glmPrint(mainCameraPos);
  stdPrint(mainCameraSpeed);
  glmPrint(mainLightPos);
  glmPrint(mainLightColor);
  dlog::print("");
}


// ========================================================================= //
// ========================================================================= //
// Are settings corrupted?
// ========================================================================= //

bool Settings::corrupted() { return m_corrupted; }

} // namespace BRAVE
