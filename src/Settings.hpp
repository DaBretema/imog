#pragma once

#include <json.hpp>
using json = nlohmann::json;

#include "gltools_Math.hpp"
#include "cpptools_FileWatcher.hpp"

namespace imog {

class Settings {
private:
  static bool           m_corrupted;
  static std::string    m_path;
  static nlohmann::json m_json;
  static FileWatcher    m_filewatcher;

public:
  static bool initialized;

  static bool        quiet;
  static bool        showTimes;
  static int         openglMajorV;
  static int         openglMinorV;
  static int         windowWidth;
  static int         windowHeight;
  static std::string windowTitle;
  static glm::vec3   clearColor;
  static float       mouseSensitivity;
  static bool        pollEvents;
  static glm::vec3   mainCameraPos;
  static glm::vec2   mainCameraRot;
  static float       mainCameraSpeed;
  static float       mainCameraFov;
  static std::string plotDir;
  static float       floorSize;

  // Initializer
  static void init(const std::string& filePath);

  // Print object values
  static void dump();

  // Are settings corrupted?
  // If it's corrupted the simulation will be paused
  static bool corrupted();
};

} // namespace imog
