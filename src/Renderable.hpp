#pragma once

#include <memory>
#include <vector>

#include "Math.hpp"
#include "helpers/Colors.hpp"

#include "Shader.hpp"
#include "Texture.hpp"


namespace BRAVE {

class Renderable {

  static unsigned int g_RenderablesLastID;

public:
  // Global pool for renderables
  static std::vector<std::shared_ptr<Renderable>>      pool;
  static std::unordered_map<std::string, unsigned int> poolIndices;


private:
  unsigned int             m_ID;
  std::string              m_meshPath;
  std::shared_ptr<Shader>  m_shader;
  std::shared_ptr<Texture> m_texture;
  // std::string  m_texturePath;
  glm::vec3    m_color;
  glm::mat4    m_model;
  unsigned int m_vao;
  unsigned int m_loc;
  unsigned int m_eboSize;


public:
  // Param constructor w/o OBJ file
  Renderable(const std::string&             objFilePath = "",
             const std::string&             texturePath = "",
             const glm::vec3&               color       = Colors::Magenta,
             const std::shared_ptr<Shader>& shader      = nullptr);

  // Get a shared ptr to Renderable obj from global pool
  // by mixed data of Renderable, like id or objFilepath
  static std::shared_ptr<Renderable> get(const std::string dataMix);

  // Create a new Renderable if it isn't on the gloabl pool
  static std::shared_ptr<Renderable>
      create(const std::string&             objFilePath = "",
             const std::string&             texturePath = "",
             const glm::vec3&               color       = Colors::Magenta,
             const std::shared_ptr<Shader>& shader      = nullptr);

  // Destructor
  ~Renderable();

  // Bind this Renderable VAO(m_vao) as active to auto attach VBO, EBO, ...
  void bind();

  // Unbind this Renderable VAO(m_vao) as active to avoid modify VBO, EBO, ...
  void unbind();


  // Getter for ID
  unsigned int ID() const;

  // G/Setter for shader
  std::shared_ptr<Shader> shader() const;
  void                    shader(const std::shared_ptr<Shader>& newShader);

  // G/Setter for color
  glm::vec3 color() const;
  void      color(glm::vec3 newColor);

  // G/Setter for model
  glm::mat4 model() const;
  void      model(glm::mat4 newModel);


  // Add a vertex attribute to this Renderable
  template <typename T>
  void addVBO(const std::vector<T>& data);

  // Store indices in the internal variable m_ebo
  void fillEBO(const std::vector<unsigned int>& indices);

  // Draw execute the Renderable in the viewport using its shader and vbos
  void draw();

  // Transform operations wrappers
  void translate(const glm::vec3& trans);
  void translate(float x, float y, float z);
  void rotate(const glm::vec3& rot);
  void rotate(float x, float y, float z);
  void scale(const glm::vec3& scl);
  void scale(float x, float y, float z);
};

} // namespace BRAVE
