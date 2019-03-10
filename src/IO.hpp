#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include <string>
#include <functional>
#include <unordered_map>
using _IO_FUNC = std::function<void()>;

#include "Camera.hpp"

namespace brave {

class IO {

  static bool                    m_pause;
  static std::shared_ptr<Camera> m_camera;

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

  static void  windowInit(std::shared_ptr<Camera> camera);
  static void  windowLoop(const _IO_FUNC& renderFn, const _IO_FUNC& updateFn);
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
  static std::unordered_map<std::string, _IO_FUNC> m_keyboardActions;

public:
#define IO_DefineKeyStates(key, onRelease, onPress, onRepeat)  \
  IO::keyboardAddAction(key, IO::kbState::release, onRelease); \
  IO::keyboardAddAction(key, IO::kbState::press, onPress);     \
  IO::keyboardAddAction(key, IO::kbState::repeat, onRepeat);

  enum struct kbState { release, press, repeat, press_and_repeat };

  static void keyboardOnPress(GLFWwindow* w,
                              int         key,
                              int         scancode,
                              int         action,
                              int         mods);

  static void keyboardAddAction(int key, kbState state, const _IO_FUNC& action);
};

} // namespace brave
