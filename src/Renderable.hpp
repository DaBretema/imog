#pragma once

#include <memory>
#include <vector>

#include "helpers/Math.hpp"
#include "helpers/Colors.hpp"

#include "Shader.hpp"


namespace BRAVE {

class Renderable {

  static unsigned int g_RenderablesLastID;

public:
  // Global pool for shaders
  static std::vector<std::shared_ptr<Renderable>> pool;


private:
  unsigned int            m_ID;
  std::shared_ptr<Shader> m_shader;
  glm::vec3               m_color;
  glm::mat4               m_model;
  unsigned int            m_vao;
  unsigned int            m_loc;
  unsigned int            m_eboSize;

public:
  // Param constructor w/o OBJ file
  Renderable(const std::shared_ptr<Shader>& shaderPath,
             const glm::vec3&               color = Colors::Magenta);

  // Param constructor w/ OBJ file, call basic constructor and link
  // render data of the OBJ file to EBO(indices) and
  // VBOs(vertices, normals)
  Renderable(const std::string&             objFilePath,
             const std::shared_ptr<Shader>& shaderPath,
             const glm::vec3&               color = Colors::Magenta);


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
  void rotate(const glm::vec3& rot);
  void scale(const glm::vec3& scl);
};

} // namespace BRAVE
