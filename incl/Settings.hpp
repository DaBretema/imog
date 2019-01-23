#pragma once

#include <string>

#include "wrap/Math.hpp"

namespace BRAVE { namespace Settings {

  int openglMajorV;
  int openglMinorV;

  int         windowWidth;
  int         windowHeight;
  std::string windowTitle;

  glm::vec3 clearColor;

  float mouseSensitivity;


}} // namespace BRAVE::Settings
