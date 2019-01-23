#pragma once

#include <string>

#include "wrap/Glad.hpp"

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include "wrap/Global.hpp"


namespace BRAVE {

class IO {

private:
  static GLFWwindow* m_windowPtr;

  static std::string m_windowTitle;
  static int         m_windowWidth;
  static int         m_windowHeight;

  static bool   m_mouseClicL;
  static bool   m_mouseClicM;
  static bool   m_mouseClicR;
  static double m_mouseLastX;
  static double m_mouseLastY;

  static std::unordered_map<int, _FUNC> m_keyboardActions;

public:
  static GLFWwindow* window();

  // --- GFLW -------------------------------------------------------------- //

  static void windowOnScaleChange(GLFWwindow* w, int width, int height);
  static void windowOnClose(GLFWwindow* w);

  static void mouseOnScroll(GLFWwindow* w, double xOffset, double yOffset);
  static void mouseOnMove(GLFWwindow* w, double currX, double currY);
  static void mouseOnClick(GLFWwindow* w, int button, int action, int mods);

  static void keyboardOnPress(GLFWwindow* w,
                              int         key,
                              int         scancode,
                              int         action,
                              int         mods);


  // --- HELPERS ----------------------------------------------------------- //

  static void  windowInit();
  static void  windowLoop(const _FUNC& renderFn);
  static float windowAspectRatio();

  static void keyboardAddAction(int key, const _FUNC& action);
};

} // namespace BRAVE
