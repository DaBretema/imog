
#include <dac/Logger.hpp>

#include "../incl/IO.hpp"
#include "../incl/Core.hpp"
#include "../incl/Settings.hpp"


namespace BRAVE {

// ====================================================================== //
// ====================================================================== //
// Private variables definition
// ====================================================================== //

GLFWwindow* IO::m_windowPtr{nullptr};

std::string IO::m_windowTitle{""};
int         IO::m_windowWidth{0};
int         IO::m_windowHeight{0};

bool   IO::m_mouseClicL{false};
bool   IO::m_mouseClicM{false};
bool   IO::m_mouseClicR{false};
double IO::m_mouseLastX{.0};
double IO::m_mouseLastY{.0};

std::unordered_map<int, _IO_FUNC> IO::m_keyboardActions{};



// * --- WINDOW --- * //

// ====================================================================== //
// ====================================================================== //
// WINDOW reply when is "initialized"
// ====================================================================== //

void IO::windowInit() {

  auto lam_WinERR = [&](auto cond, const std::string& err) {
    if (!cond) {
      dlog::err(err);
      glfwTerminate();
    }
  };

  m_windowTitle  = Settings::windowTitle;
  m_windowHeight = Settings::windowHeight;
  m_windowWidth  = Settings::windowWidth;

  // Creation
  lam_WinERR(glfwInit(), "Initializing GLFW");
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Settings::openglMajorV);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Settings::openglMinorV);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

  GLFWwindow* o_WINDOW = glfwCreateWindow(
      m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);
  lam_WinERR(o_WINDOW, "Creating GLFW window");

  // Set as main
  glfwMakeContextCurrent(o_WINDOW);

  // Callbacks
  glfwSetKeyCallback(o_WINDOW, keyboardOnPress);
  glfwSetCursorPosCallback(o_WINDOW, mouseOnMove);
  glfwSetScrollCallback(o_WINDOW, mouseOnScroll);
  glfwSetMouseButtonCallback(o_WINDOW, mouseOnClick);
  glfwSetWindowSizeCallback(o_WINDOW, windowOnScaleChange);

  // Gl-Extensions
  GLADloadproc glwfProc = (GLADloadproc)glfwGetProcAddress;
  lam_WinERR(gladLoadGLLoader(glwfProc), "Initializing OpenGL with GLAD");

  // Gl-Settings
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  glm::vec3 cc = Settings::clearColor;
  glClearColor(cc.r, cc.g, cc.b, 1.0f);

  // Store window ptr
  m_windowPtr = o_WINDOW;
}

// ====================================================================== //
// ====================================================================== //
// WINDOW loop definition
// ====================================================================== //

void IO::windowLoop(const _IO_FUNC& renderFn, const _IO_FUNC& updateFn) {
  int    frame = 0;
  double iTime = glfwGetTime();

  auto lam_FPS = [&]() {
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
    lam_FPS();
    (Settings::pollEvents) ? glfwPollEvents() : glfwWaitEvents();
    if (Settings::corrupted()) { Core::pause = true; }
    if (Core::pause) { continue; }
    updateFn();

    // Render
    renderFn();
    glfwSwapBuffers(m_windowPtr);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}

// ====================================================================== //
// ====================================================================== //
// WINDOW compute "aspect ratio"
// ====================================================================== //

float IO::windowAspectRatio() {
  float fWidth  = static_cast<float>(m_windowWidth);
  float fHeight = static_cast<float>(m_windowHeight);
  return fWidth / fHeight;
}

// ====================================================================== //
// ====================================================================== //
// WINDOW reply when is "resized"
// ====================================================================== //

void IO::windowOnScaleChange(GLFWwindow* w, int width, int height) {
  glViewport(0, 0, width, height);
  m_windowWidth  = width;
  m_windowHeight = height;
}

// ====================================================================== //
// ====================================================================== //
// WINDOW reply when is "closed"
// ====================================================================== //

void IO::windowOnClose(GLFWwindow* w) {
  Core::close();
  glfwSetWindowShouldClose(w, GL_TRUE);
}



// * --- MOUSE --- * //

// ====================================================================== //
// ====================================================================== //
// MOUSE reply when is "scrolled"
// ====================================================================== //

void IO::mouseOnScroll(GLFWwindow* w, double xOffset, double yOffset) {
  Core::camera->zoom(static_cast<float>(yOffset));
}

// ====================================================================== //
// ====================================================================== //
// MOUSE reply when is "moved"
// ====================================================================== //

void IO::mouseOnMove(GLFWwindow* w, double mouseCurrX, double mouseCurrY) {
  if (m_mouseClicL) {
    Core::camera->rotateY(static_cast<float>((mouseCurrX - m_mouseLastX) *
                                             Settings::mouseSensitivity));
    Core::camera->rotateX(static_cast<float>((mouseCurrY - m_mouseLastY) *
                                             Settings::mouseSensitivity));
  }
  m_mouseLastX = mouseCurrX;
  m_mouseLastY = mouseCurrY;
}

// ====================================================================== //
// ====================================================================== //
// MOUSE reply when is "clicked"
// ====================================================================== //

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



// * --- KEYBOARD --- * //

// ====================================================================== //
// ====================================================================== //
// KEYBOARD reply when is "pressed"
// ====================================================================== //

void IO::keyboardOnPress(GLFWwindow* w,
                         int         key,
                         int         scancode,
                         int         action,
                         int         mods) {
  bool keyIsOnMap  = (m_keyboardActions.count(key) > 0);
  bool keyIsActive = (action == GLFW_PRESS || action == GLFW_REPEAT);

  if (keyIsActive && keyIsOnMap) { m_keyboardActions.at(key)(); }
}

// ====================================================================== //
// ====================================================================== //
// KEYBOARD reply when an "action" is added
// ====================================================================== //

void IO::keyboardAddAction(int key, const _IO_FUNC& action) {
  if (m_keyboardActions.count(key) < 1) {
    m_keyboardActions.insert({key, action});
  } else {
    dlog::info("The key is already defined");
  }
}

} // namespace BRAVE
