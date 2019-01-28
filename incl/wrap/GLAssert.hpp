
#include <glad/glad.h>
#include <Dac/Logger.hpp>

#define GL_ASSERT(funcToCheck) \
  glErrClear();                \
  funcToCheck;                 \
  glAssert(__FILE__, __LINE__);


/// GL_ERRORS :: Avoid fake error
static inline void glErrClear() {
  while (glGetError() != GL_NO_ERROR)
    ;
}

/// GL_ERRORS :: Parse real errors after gl* func is executed
static inline void glAssert(const char* file, int line) {
  if (auto glError = glGetError()) {
    const char* errStr;
    switch (glError) {
      case GL_INVALID_ENUM: errStr = "Invalid enum"; break;
      case GL_INVALID_VALUE: errStr = "Invalid value"; break;
      case GL_INVALID_OPERATION: errStr = "Invalid operation"; break;
      default: errStr = "Unrecognised GLenum"; break;
    }
    DacLog_ERR("{} ({}): {} ({})", file, line, errStr, glError);
  }
}
