#include "IO.hpp"

#include <dac/Logger.hpp>

#include "Settings.hpp"

#include "Shader.hpp"
#include "Renderable.hpp"

#include "helpers/Consts.hpp"


namespace brave {

// ====================================================================== //
// ====================================================================== //
// Private variables definition
// ====================================================================== //

bool                    IO::m_pause{false};
std::shared_ptr<Camera> IO::m_camera{nullptr};

GLFWwindow* IO::m_windowPtr{nullptr};

std::string IO::m_windowTitle{""};
int         IO::m_windowWidth{0};
int         IO::m_windowHeight{0};

bool   IO::m_mouseClicL{false};
bool   IO::m_mouseClicM{false};
bool   IO::m_mouseClicR{false};
double IO::m_mouseLastX{.0};
double IO::m_mouseLastY{.0};

std::unordered_map<std::string, _IO_FUNC> IO::m_keyboardActions{};



// * --- WINDOW --- * //

GLFWwindow* IO::window() { return m_windowPtr; }

// ====================================================================== //
// ====================================================================== //
// WINDOW reply when is "initialized"
// ====================================================================== //

void IO::windowInit(std::shared_ptr<Camera> camera) {

  // ---------------------------------------------------------
  // --- Validations -----------------------------------------

  if (!Settings::initialized) { dInfo("Engine gonna use default values."); }

  if (!camera) {
    dInfo("Camera should be defined. ABORT.");
    std::exit(2);
  }

  // --------------------------------------- / Validations ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Init vars -------------------------------------------

  m_camera = camera;

  m_windowTitle  = Settings::windowTitle;
  m_windowHeight = Settings::windowHeight;
  m_windowWidth  = Settings::windowWidth;

  // ----------------------------------------- / Init vars ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Window creation -------------------------------------

  if (!glfwInit()) {
    dErr("Couldn't initialize GLFW");
    glfwTerminate();
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Settings::openglMajorV);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Settings::openglMinorV);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

  GLFWwindow* o_WINDOW = glfwCreateWindow(
      m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);

  if (!o_WINDOW) {
    dErr("Couldn't create GLFW window");
    glfwTerminate();
  }

  glfwMakeContextCurrent(o_WINDOW); // Set as active window

  // ----------------------------------- / Window creation ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Callbacks -------------------------------------------

  // - Keyboard

  glfwSetKeyCallback(o_WINDOW, keyboardOnPress);

  // - Mouse

  glfwSetScrollCallback(o_WINDOW, mouseOnScroll);
  glfwSetCursorPosCallback(o_WINDOW, mouseOnMove);
  glfwSetMouseButtonCallback(o_WINDOW, mouseOnClick);

  // - Window

  glfwSetWindowCloseCallback(o_WINDOW, windowOnClose);
  glfwSetWindowSizeCallback(o_WINDOW, windowOnScaleChange);

  // ----------------------------------------- / Callbacks ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- GLAD ------------------------------------------------

  GLADloadproc glwfProc = (GLADloadproc)glfwGetProcAddress;

  if (!gladLoadGLLoader(glwfProc)) {
    dErr("Couldn't initialize GLAD");
    glfwTerminate();
  }

  // ---------------------------------------------- / GLAD ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Defaults --------------------------------------------

  // GL Settings
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glm::vec3 cc = Settings::clearColor;
  glClearColor(cc.r, cc.g, cc.b, 1.0f);

  // Create default Shaders
  Shader::createByName("base", true);
  Shader::createByName("light");

  // Create default Renderables
  Renderable::create(false, "Bone", Figures::cylinder, "", Colors::orange);
  Renderable::create(false, "MonkeyHead", Figures::monkey, "", Colors::orange);

  // ------------------------------------------ / Defaults ---
  // ---------------------------------------------------------

  m_windowPtr = o_WINDOW; // Store window ptr
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
    if (Settings::corrupted()) { m_pause = true; }
    if (m_pause) { continue; }

    // Update
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
  if (!Settings::quiet) dInfo("Closing GLFW window.");
  glfwSetWindowShouldClose(w, GL_TRUE);
}



// * --- MOUSE --- * //

// ====================================================================== //
// ====================================================================== //
// MOUSE reply when is "scrolled"
// ====================================================================== //

void IO::mouseOnScroll(GLFWwindow* w, double xOffset, double yOffset) {
  m_camera->zoom(static_cast<float>(yOffset));
}

// ====================================================================== //
// ====================================================================== //
// MOUSE reply when is "moved"
// ====================================================================== //

void IO::mouseOnMove(GLFWwindow* w, double mouseCurrX, double mouseCurrY) {
  if (m_mouseClicL) {
    float yRot = (mouseCurrX - m_mouseLastX) * Settings::mouseSensitivity;
    float xRot = (mouseCurrY - m_mouseLastY) * Settings::mouseSensitivity;

    m_camera->pivot.rot += glm::vec3(xRot, yRot, 0.f);
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

  // ----------------------------------
  // --- User defined actions ---------
  // ----------------------------------

  auto mapKey     = std::to_string(key) + "_" + std::to_string(action);
  bool keyIsOnMap = (m_keyboardActions.count(mapKey) > 0);
  if (keyIsOnMap) { m_keyboardActions.at(mapKey)(); }

  // ----------------------------------
  // --- Default actions --------------
  // ----------------------------------

  if (action == GLFW_PRESS) {
    switch (mods) {

      // Key pressed holding SHIFT
      case GLFW_MOD_SHIFT:
        switch (key) {
          case GLFW_KEY_ESCAPE: windowOnClose(m_windowPtr); break;
          default: break;
        }
        break;

      // Key pressed without mod
      default:
        switch (key) {
          case GLFW_KEY_ESCAPE: m_pause = !m_pause; break;
          default: break;
        }
        break;
    }
  }
}

// ====================================================================== //
// ====================================================================== //
// KEYBOARD reply when an "action" is added
// ====================================================================== //

void IO::keyboardAddAction(int key, kbState state, const _IO_FUNC& action) {
  auto mapKey = std::to_string(key) + "_";

  if (state == kbState::press_and_repeat) {
    auto keyPress = mapKey + std::to_string((unsigned int)kbState::press);
    m_keyboardActions.insert({keyPress, action});
    auto keyRepeat = mapKey + std::to_string((unsigned int)kbState::repeat);
    m_keyboardActions.insert({keyRepeat, action});
  }

  else {
    auto keyBase = mapKey + std::to_string((unsigned int)state);
    m_keyboardActions.insert({keyBase, action});
  }
}

} // namespace brave
