
#include <Dac/Logger.hpp>

#include "../incl/IO.hpp"
#include "../incl/Core.hpp"
#include "../incl/Settings.hpp"


namespace BRAVE {

std::string IO::m_windowTitle;
int         IO::m_windowWidth;
int         IO::m_windowHeight;

bool   IO::m_mouseClicL;
bool   IO::m_mouseClicM;
bool   IO::m_mouseClicR;
double IO::m_mouseLastX;
double IO::m_mouseLastY;

std::unordered_map<int, _FUNC> IO::m_keyboardActions;


// --- GFLW ---------------------------------------------------------------- //
// ------------------------------------------------------------------------- //


// ------------ Window ----------------------------------------------------- //

void IO::windowOnScaleChange(GLFWwindow* w, int width, int height) {
  glViewport(0, 0, width, height);
  m_windowWidth  = width;
  m_windowHeight = height;
}
void IO::windowOnClose(GLFWwindow* w) {
  Core::close();
  glfwSetWindowShouldClose(w, GL_TRUE);
}


// ------------ Mouse ------------------------------------------------------ //

void IO::mouseOnScroll(GLFWwindow* w, double xOffset, double yOffset) {
  Core::camera->zoom(static_cast<float>(yOffset));
}
void IO::mouseOnMove(GLFWwindow* w, double mouseCurrX, double mouseCurrY) {
  if (m_mouseClicL) {
    Core::camera->rotateX(static_cast<float>((mouseCurrX - m_mouseLastX) *
                                             Settings::mouseSensitivity));
    Core::camera->rotateY(static_cast<float>((mouseCurrY - m_mouseLastY) *
                                             Settings::mouseSensitivity));
  }
  m_mouseLastX = mouseCurrX;
  m_mouseLastY = mouseCurrY;
}
void IO::mouseOnClick(GLFWwindow* w, int button, int action, int mods) {
  switch (button) {

    case GLFW_MOUSE_BUTTON_LEFT:
      if (action == GLFW_PRESS) m_mouseClicL = true;
      if (action == GLFW_RELEASE) m_mouseClicL = false;
      break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
      if (action == GLFW_PRESS) m_mouseClicM = true;
      if (action == GLFW_RELEASE) m_mouseClicM = false;
      break;

    case GLFW_MOUSE_BUTTON_RIGHT:
      if (action == GLFW_PRESS) m_mouseClicR = true;
      if (action == GLFW_RELEASE) m_mouseClicR = false;
      break;

    default: break;
  }
}


// ------------ Keyboard --------------------------------------------------- //

void IO::keyboardOnPress(GLFWwindow* w,
                         int         key,
                         int         scancode,
                         int         action,
                         int         mods) {}



// --- HELPERS ------------------------------------------------------------- //
// ------------------------------------------------------------------------- //

#define __wiERR(cond, errMsg) \
  {                           \
    if (!cond) {              \
      DacLog_ERR(errMsg);     \
      glfwTerminate();        \
    }                         \
  }


// ------------ Window ----------------------------------------------------- //

void IO::windowInit() {
  GLFWwindow* o_WINDOW;

  m_windowTitle  = Settings::windowTitle;
  m_windowHeight = Settings::windowHeight;
  m_windowWidth  = Settings::windowWidth;

  // Creation
  __wiERR(glfwInit(), "Initializing GLFW");
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Settings::openglMajorV);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Settings::openglMinorV);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  o_WINDOW = glfwCreateWindow(
      m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);
  __wiERR(o_WINDOW, "Creating GLFW window");

  // Set as main
  glfwMakeContextCurrent(o_WINDOW);

  // Callbacks
  glfwSetKeyCallback(o_WINDOW, keyboardOnPress);
  glfwSetCursorPosCallback(o_WINDOW, mouseOnMove);
  glfwSetScrollCallback(o_WINDOW, mouseOnScroll);
  glfwSetMouseButtonCallback(o_WINDOW, mouseOnClick);
  glfwSetWindowSizeCallback(o_WINDOW, windowOnScaleChange);

  // Gl-Extensions
  auto glwfProc = (GLADloadproc)glfwGetProcAddress;
  __wiERR(gladLoadGLLoader(glwfProc), "Initializing OpenGL with GLAD");

  // Gl-Settings
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  auto cc = Settings::clearColor;
  glClearColor(cc.r, cc.g, cc.b, 1.0f);

  // Store window ptr
  m_windowPtr = o_WINDOW;
}



void IO::windowLoop(const _FUNC& renderFn) {

  int    frame = 0;
  double iTime = glfwGetTime();

  auto FPS = [&]() {
    ++frame;
    double eTime = glfwGetTime();

    if (eTime - iTime >= 1.0) {
      auto title = m_windowTitle + " :: " + std::to_string(frame) + "fps";
      iTime      = eTime;
      frame      = 0;

      glfwSetWindowTitle(m_windowPtr, title.c_str());
    }
  };

  while (!glfwWindowShouldClose(m_windowPtr)) {
    // Events
    FPS();
    glfwWaitEvents();
    if (Core::pause) { continue; }
    // Render
    renderFn();
    glfwSwapBuffers(m_windowPtr);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}


float IO::windowAspectRatio() {
  float fWidth  = static_cast<float>(m_windowWidth);
  float fHeight = static_cast<float>(m_windowHeight);
  return fWidth / fHeight;
}



// ------------ Keyboard --------------------------------------------------- //

void IO::keyboardAddAction(int key, const _FUNC& action) {
  if (m_keyboardActions.count(key) < 1) {
    m_keyboardActions.insert({key, action});
  } else {
    DacLog_INFO("The key is already defined");
  }
}

} // namespace BRAVE
