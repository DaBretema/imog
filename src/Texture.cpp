#include "Texture.hpp"

#include "helpers/GLAssert.hpp"
#include "Settings.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace brave {

#define tex2DParam(id, mode) \
  GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, id, mode));

#define tex2DImg(w, h, img) \
  GL_ASSERT(glTexImage2D(   \
      GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img));


// ====================================================================== //
// ====================================================================== //
// Global pool and indexmap for textures
// ====================================================================== //

std::unordered_map<std::string, std::shared_ptr<Texture>> Texture::pool;


// ====================================================================== //
// ====================================================================== //
// Init variables and load img to OpenGL context
// ====================================================================== //

Texture::Texture(const std::string& path)
    : m_glID(0), m_path(path), m_bytes(0), m_width(0), m_height(0) {

  static bool __once_stbflip = []() {
    stbi_set_flip_vertically_on_load(1);
    return true;
  }();

  GL_ASSERT(glGenTextures(1, &m_glID));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_glID));

  tex2DParam(GL_TEXTURE_WRAP_S, GL_REPEAT);
  tex2DParam(GL_TEXTURE_WRAP_T, GL_REPEAT);
  tex2DParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  tex2DParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  if (auto img = stbi_load(m_path.c_str(), &m_width, &m_height, &m_bytes, 4)) {
    tex2DImg(m_width, m_height, img);           // This before mipmap gen
    GL_ASSERT(glGenerateMipmap(GL_TEXTURE_2D)); // This after glTexImg2D
    stbi_image_free(img);
  } else {
    if (!Settings::quiet) dErr("Failed loading texture \"{}\"", path);
  }
}


// ====================================================================== //
// ====================================================================== //
// Get a shared ptr to the texture from the global pool
// ====================================================================== //

std::shared_ptr<Texture> Texture::get(const std::string& path) {
  if (pool.count(path) > 0) { return pool[path]; }
  return nullptr;
}

// ====================================================================== //
// ====================================================================== //
// Create a new texture if it isn't on the gloabl pool
// ====================================================================== //

std::shared_ptr<Texture> Texture::create(const std::string& path) {
  if (path.empty()) { return nullptr; }
  if (auto T = get(path)) { return T; }
  pool[path] = std::make_shared<Texture>(path);
  return pool[path];
}

// ====================================================================== //
// ====================================================================== //
// Just remove texture from OpenGL context
// ====================================================================== //

Texture::~Texture() {
  if (!Settings::quiet) dInfo("Destroyed @ {}", m_path);
  GL_ASSERT(glDeleteTextures(1, &m_glID));
}

// ====================================================================== //
// ====================================================================== //
// Active this texture binding it to its OpenGL slot
// ====================================================================== //

void Texture::bind() const {
  GL_ASSERT(glActiveTexture(GL_TEXTURE0 + m_glID));
  GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_glID));
}

void Texture::bind(const std::shared_ptr<Shader>& s) const {
  bind();
  s->uInt1("u_texture", m_glID);
}

// ====================================================================== //
// ====================================================================== //
// Disable this texture unbinding from its OpenGL slot
// ====================================================================== //

void Texture::unbind() const { GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0)); }


// ====================================================================== //
// ====================================================================== //
// Getter for path
// ====================================================================== //

std::string Texture::path() const { return m_path; }

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

} // namespace brave
