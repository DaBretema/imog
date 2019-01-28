#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <string>
#include <functional>
#include <unordered_map>
using _IO_FUNC = std::function<void()>;


namespace BRAVE {
class IO {

  // ====================================================================== //
  // ====================================================================== //
  // WINDOW
  // ====================================================================== //

private:
  static GLFWwindow* m_windowPtr;
  static std::string m_windowTitle;
  static int         m_windowWidth;
  static int         m_windowHeight;

public:
  static GLFWwindow* window();

  static void  windowInit();
  static void  windowLoop(const _IO_FUNC& renderFn);
  static float windowAspectRatio();
  static void  windowOnScaleChange(GLFWwindow* w, int width, int height);
  static void  windowOnClose(GLFWwindow* w);



  // ====================================================================== //
  // ====================================================================== //
  // MOUSE
  // ====================================================================== //

private:
  static bool   m_mouseClicL;
  static bool   m_mouseClicM;
  static bool   m_mouseClicR;
  static double m_mouseLastX;
  static double m_mouseLastY;

public:
  static void mouseOnScroll(GLFWwindow* w, double xOffset, double yOffset);
  static void mouseOnMove(GLFWwindow* w, double currX, double currY);
  static void mouseOnClick(GLFWwindow* w, int button, int action, int mods);



  // ====================================================================== //
  // ====================================================================== //
  // KEYBOARD
  // ====================================================================== //

private:
  static std::unordered_map<int, _IO_FUNC> m_keyboardActions;

public:
  static void keyboardOnPress(GLFWwindow* w,
                              int         key,
                              int         scancode,
                              int         action,
                              int         mods);
  static void keyboardAddAction(int key, const _IO_FUNC& action);
};

} // namespace BRAVE
