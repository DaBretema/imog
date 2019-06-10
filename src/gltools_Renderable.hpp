#pragma once

#include <memory>
#include <vector>

#include "gltools_Math.hpp"
#include "helpers/Colors.hpp"

#include "gltools_Transform.hpp"
#include "gltools_Texture.hpp"

#include "gltools_Camera.hpp"
#include "gltools_Shader.hpp"


namespace imog {


class Renderable {

private:
  static unsigned int g_RenderablesLastID;

public:
  // Default data struct to compose a Renderable
  struct data {
    std::vector<glm::vec3>    vertices;
    std::vector<glm::vec3>    normals;
    std::vector<glm::vec2>    uvs;
    std::vector<unsigned int> indices;
  };

  // Global pool for renderables
  static std::vector<std::shared_ptr<Renderable>>      pool;
  static std::unordered_map<std::string, unsigned int> poolIndices;

  // Get a shared ptr to Renderable obj from global pool by name
  static std::shared_ptr<Renderable> getByName(const std::string& name);

  // Create a new Renderable if it isn't on the gloabl pool
  static std::shared_ptr<Renderable>
      create(bool                           allowGlobalDraw = true,
             const std::string&             name            = "",
             const std::string&             objFilePath     = "",
             const std::string&             texturePath     = "",
             const glm::vec3&               color           = Colors::magenta,
             const std::shared_ptr<Shader>& shader          = nullptr,
             bool                           culling         = true);

  // Draw all renderables of the pool
  static void poolDraw(const std::shared_ptr<Camera>& camera);

private:
  unsigned int m_ID;
  std::string  m_name;
  std::string  m_meshPath;

  std::shared_ptr<Shader>  m_shader;
  std::shared_ptr<Texture> m_texture;

  bool m_culling;

  glm::vec3 m_color;

  unsigned int m_vao;
  unsigned int m_loc;
  unsigned int m_eboSize;


public:
  Transform transform;
  bool      globalDraw;

  // Param constructor w/o OBJ file. //! DO NOT CALL THIS DIRECTLY, use Create.
  Renderable(bool                           allowGlobalDraw = true,
             const std::string&             name            = "",
             const std::string&             objFilePath     = "",
             const std::string&             texturePath     = "",
             const glm::vec3&               color           = Colors::magenta,
             const std::shared_ptr<Shader>& shader          = nullptr,
             bool                           culling         = true);

  // Destructor
  ~Renderable();


  // Bind this Renderable VAO(m_vao) as active to auto attach VBO, EBO, ...
  void bind();

  // Unbind this Renderable VAO(m_vao) as active to avoid modify VBO, EBO, ...
  void unbind();


  // Getter for ID
  unsigned int ID() const;

  // Getter for name
  std::string name() const;

  // G/Setter for shader
  std::shared_ptr<Shader> shader() const;
  void                    shader(const std::shared_ptr<Shader>& newShader);

  // G/Setter for color
  glm::vec3 color() const;
  void      color(const glm::vec3& newColor);

  // Add a vertex attribute to this Renderable
  template <typename T>
  void addVBO(const std::vector<T>& data);

  // Store indices in the internal variable m_ebo
  void fillEBO(const std::vector<unsigned int>& indices);

  // Draw execute the Renderable in the viewport using its shader and vbos
  void draw(const std::shared_ptr<Camera>& camera);

  // Draw cyl between 2points
  static std::shared_ptr<Renderable>
      line(const glm::vec3& P1, const glm::vec3& P2, float scale = 10.f);
};

} // namespace imog
