#pragma once

#include <string>

namespace BRAVE {

class Texture {

  // Global counter of generated textures
  static unsigned int g_TexturesLastID;

private:
  unsigned int m_ID;
  unsigned int m_glID;
  std::string  m_path;
  int          m_bits;
  int          m_width;
  int          m_height;

public:
  // Init variables and load img to OpenGL context
  Texture(const std::string& path);

  // Just remove texture from OpenGL context
  ~Texture();

  // Active this texture binding it to its OpenGL slot
  void bind() const;
  // Disable this texture unbinding from its OpenGL slot
  void unbind() const;

  // Getter for width
  int width() const;
  // Getter for height
  int height() const;
};

} // namespace BRAVE
