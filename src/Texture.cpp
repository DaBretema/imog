#include "Texture.hpp"

#include "helpers/GLAssert.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace BRAVE {

// ====================================================================== //
// ====================================================================== //
// Global counter of generated textures
// ====================================================================== //

unsigned int Texture::g_TexturesLastID;


// ====================================================================== //
// ====================================================================== //
// Init variables and load img to OpenGL context
// ====================================================================== //

Texture::Texture(const std::string& path)
    : m_ID(g_TexturesLastID++),
      m_glID(0),
      m_path(path),
      m_bits(0),
      m_width(0),
      m_height(0) {

  stbi_set_flip_vertically_on_load(1);
  unsigned char* imgBuffer =
      stbi_load(m_path.c_str(), &m_width, &m_height, &m_bits, 4);

  GL_ASSERT(glGenTextures(1, &m_glID));
  bind();

  GL_ASSERT(glGenerateMipmap(GL_TEXTURE_2D));
  GL_ASSERT(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
  GL_ASSERT(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));
  GL_ASSERT(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GL_ASSERT(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

  GL_ASSERT(glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA8,
                         m_width,
                         m_height,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         imgBuffer));

  if (imgBuffer) { stbi_image_free(imgBuffer); }
}

// ====================================================================== //
// ====================================================================== //
// Just remove texture from OpenGL context
// ====================================================================== //

Texture::~Texture() { GL_ASSERT(glDeleteTextures(1, &m_glID)); }

// ====================================================================== //
// ====================================================================== //
// Active this texture binding it to its OpenGL slot
// ====================================================================== //

void Texture::bind() const {
  GL_ASSERT(glActiveTexture(GL_TEXTURE0 + m_ID));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_glID));
}

// ====================================================================== //
// ====================================================================== //
// Disable this texture unbinding from its OpenGL slot
// ====================================================================== //

void Texture::unbind() const { GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0)); }

// ====================================================================== //
// ====================================================================== //
// Getter for width
// ====================================================================== //

int Texture::width() const { return m_width; }

// ====================================================================== //
// ====================================================================== //
// Getter for height
// ====================================================================== //
int Texture::height() const { return m_height; }

} // namespace BRAVE
