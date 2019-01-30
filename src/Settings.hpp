#pragma once

#include <json.hpp>
using json = nlohmann::json;

#include <dac/FileWatcher.hpp>

#include "helpers/Math.hpp"


namespace BRAVE {

class Settings {
private:
  static bool             m_corrupted;
  static std::string      m_path;
  static nlohmann::json   m_json;
  static dac::FileWatcher m_filewatcher;

public:
  static int         openglMajorV;
  static int         openglMinorV;
  static int         windowWidth;
  static int         windowHeight;
  static std::string windowTitle;
  static glm::vec3   clearColor;
  static float       mouseSensitivity;
  static bool        pollEvents;
  static glm::vec3   mainCameraPos;
  static float       mainCameraSpeed;
  static glm::vec3   mainLightPos;
  static glm::vec3   mainLightColor;
  static bool        promptUniformErrors;

  // Initializer
  static void init(const std::string& filePath);

  // Print object values
  static void dump();

  // Are settings corrupted?
  // If it's corrupted the simulation will be paused
  static bool corrupted();
};

} // namespace BRAVE
