#pragma once

#include <string>
#include <memory>
#include <unordered_map>


namespace brave {

class Texture {

public:
  // Global pool for textures
  static std::unordered_map<std::string, std::shared_ptr<Texture>> pool;


private:
  unsigned int m_glID;
  std::string  m_path;
  int          m_bytes;
  int          m_width;
  int          m_height;

public:
  // Init variables and load img to OpenGL context
  Texture(const std::string& path);

  // Get a shared ptr to the texture from the global pool
  static std::shared_ptr<Texture> getByPath(const std::string& path);

  // Create a new texture if it isn't on the gloabl pool
  static std::shared_ptr<Texture> create(const std::string& path);

  // Just remove texture from OpenGL context
  ~Texture();

  // Active this texture binding it to its OpenGL slot
  unsigned int bind() const;

  // Disable this texture unbinding from its OpenGL slot
  void unbind() const;


  // Getter for path
  std::string path() const;

  // Getter for width
  int width() const;

  // Getter for height
  int height() const;
};

} // namespace brave
