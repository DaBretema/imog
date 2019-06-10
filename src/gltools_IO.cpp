#include "gltools_IO.hpp"

#include "cpptools_Logger.hpp"
#include "Settings.hpp"

#include "gltools_Shader.hpp"
#include "gltools_Renderable.hpp"

#include "helpers/Consts.hpp"
#include "helpers/GLAssert.hpp"

#include <stb_image.h>


namespace imog {

// ====================================================================== //
// ====================================================================== //
// Private variables definition
// ====================================================================== //

bool                    IO::m_pause;
std::shared_ptr<Camera> IO::m_camera;

GLFWwindow* IO::m_windowPtr{nullptr};

std::string IO::m_windowTitle{""};
int         IO::m_windowWidth{0};
int         IO::m_windowHeight{0};

bool   IO::m_mouseClicL{false};
bool   IO::m_mouseClicM{false};
bool   IO::m_mouseClicR{false};
double IO::m_mouseLastX{.0};
double IO::m_mouseLastY{.0};

std::unordered_multimap<std::string, _IO_FUNC> IO::m_keyboardActions{};



// * --- WINDOW --- * //

GLFWwindow* IO::window() { return m_windowPtr; }

// ====================================================================== //
// ====================================================================== //
// WINDOW reply when is "initialized"
// ====================================================================== //

void IO::windowInit(const std::shared_ptr<Camera>& camera) {

  // ---------------------------------------------------------
  // --- Validations -----------------------------------------

  if (!Settings::initialized) { LOGD("Engine gonna use default values."); }

  if (!camera) {
    LOGD("Camera should be defined. ABORT.");
    std::exit(2);
  }

  // --------------------------------------- / Validations ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Init vars -------------------------------------------

  m_camera = camera;

  m_windowTitle  = Settings::windowTitle;
  m_windowWidth  = Settings::windowWidth;
  m_windowHeight = Settings::windowHeight;

  // ----------------------------------------- / Init vars ---
  // ---------------------------------------------------------


  // ---------------------------------------------------------
  // --- Window creation -------------------------------------

  if (!glfwInit()) {
    LOGE("Couldn't initialize GLFW");
    glfwTerminate();
  }

  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Settings::openglMajorV);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Settings::openglMinorV);

  GLFWwindow* o_WINDOW = glfwCreateWindow(
      m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);

  if (!o_WINDOW) {
    LOGE("Couldn't create GLFW window");
    glfwTerminate();
  }

  // Set as active window
  glfwMakeContextCurrent(o_WINDOW);

  // Set icon
  int       icoW, icoH;
  auto      ico = stbi_load("./assets/icon/icon.png", &icoW, &icoH, 0, 4);
  GLFWimage icons[1];
  icons[0].height = icoH;
  icons[0].width  = icoW;
  icons[0].pixels = ico;
  glfwSetWindowIcon(o_WINDOW, 1, icons);

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
    LOGE("Couldn't initialize GLAD");
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
  glDepthFunc(GL_LEQUAL);
  glm::vec3 cc = Settings::clearColor;
  glClearColor(cc.r, cc.g, cc.b, 1.0f);

  // Create default Shaders
  Shader::createByName("base", true);
  auto sks   = Shader::createByName("sk", true);
  auto bones = Shader::createByName("bone", true);

  // Create default Renderables
  Renderable::create(false, "Cube", Figures::cube, "", Colors::orange, sks);
  Renderable::create(false, "Ball", Figures::sphere, "", Colors::orange, sks);
  Renderable::create(false, "Stick", Figures::stick, "", Colors::orange, bones);
  Renderable::create(false, "Monkey", Figures::monkey, "", Colors::orange, sks);

  // Create floor
  auto floor = Renderable::create(
      true, "Floor", Figures::plane, Textures::chess, Colors::teal);
  floor->transform.pos -= glm::vec3(0, 10.f, 0);
  floor->transform.scl =
      glm::vec3(Settings::floorSize, 1.f, Settings::floorSize);

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
      auto subTitle = (m_pause) ? "PAUSED" : std::to_string(frame) + "fps";
      auto title    = m_windowTitle + " :: " + subTitle;
      iTime         = eTime;
      frame         = 0;
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
  if (!Settings::quiet) LOGD("Closing GLFW window.");
  glfwSetWindowShouldClose(w, GL_TRUE);
}

void IO::windowVisibility(bool value) {
  (value) ? glfwShowWindow(m_windowPtr) : glfwHideWindow(m_windowPtr);
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

  if (keyIsOnMap) {
    auto itRange = m_keyboardActions.equal_range(mapKey);
    for (auto it = itRange.first; it != itRange.second; ++it) { it->second(); }
  }

  // ----------------------------------
  // --- Default actions --------------
  // ----------------------------------

  if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
    windowOnClose(m_windowPtr);
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

} // namespace imog
