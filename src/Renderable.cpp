#include "Renderable.hpp"

#include <mutex>
#include <sstream>

#include "LoaderOBJ.hpp"
#include "helpers/GLAssert.hpp"


namespace BRAVE {

// ====================================================================== //
// ====================================================================== //
// Global Renderable objects counter
// ====================================================================== //

unsigned int Renderable::g_RenderablesLastID;

// ====================================================================== //
// ====================================================================== //
// Global pool for shaders
// ====================================================================== //

std::vector<std::shared_ptr<Renderable>> Renderable::pool;


// ====================================================================== //
// ====================================================================== //
// Param constructor w/o OBJ file
// ====================================================================== //

Renderable::Renderable(const std::shared_ptr<Shader>& shader,
                       const glm::vec3&               color)
    : m_ID(g_RenderablesLastID++),
      m_shader(shader),
      m_color(color),
      m_model(glm::mat4(1.f)),
      m_vao(0),
      m_loc(0),
      m_eboSize(0) {
  m_shader->uFloat3("Color", m_color);
  GL_ASSERT(glGenVertexArrays(1, &m_vao));

  pool.push_back(std::shared_ptr<Renderable>(this));
}

// ====================================================================== //
// ====================================================================== //
// Param constructor w/ OBJ file, call basic constructor and link
// render data of the OBJ file to EBO(indices) and
// VBOs(vertices, normals)
// ====================================================================== //

Renderable::Renderable(const std::string&             objFilePath,
                       const std::shared_ptr<Shader>& shader,
                       const glm::vec3&               color)
    : Renderable(shader, color) {
  RenderData renderData = loadOBJ(objFilePath);
  this->fillEBO(renderData.indices); // Location = 0
  this->addVBO(renderData.vertices); // Location = 1
  this->addVBO(renderData.normals);  // Location = 2
}


// ====================================================================== //
// ====================================================================== //
// Bind this Renderable VAO(m_vao) as active to auto attach VBO, EBO, ...
// ====================================================================== //

void Renderable::bind() {
  GL_ASSERT(glBindVertexArray(m_vao));
  m_shader->bind();
}

// ====================================================================== //
// ====================================================================== //
// Unbind this Renderable VAO(m_vao) as active to avoid modify VBO, EBO, ...
// ====================================================================== //

void Renderable::unbind() {
  GL_ASSERT(glBindVertexArray(0));
  m_shader->unbind();
  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


// ====================================================================== //
// ====================================================================== //
// Getter for ID
// ====================================================================== //

unsigned int Renderable::ID() const { return m_ID; }

// ====================================================================== //
// ====================================================================== //
// G/Setter for shader
// ====================================================================== //

std::shared_ptr<Shader> Renderable::shader() const { return m_shader; }
void Renderable::shader(const std::shared_ptr<Shader>& newShader) {
  m_shader = newShader;
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for color
// ====================================================================== //

glm::vec3 Renderable::color() const { return m_color; }
void      Renderable::color(glm::vec3 newColor) {
  m_color = newColor;
  m_shader->uFloat3("Color", m_color);
}

// ====================================================================== //
// ====================================================================== //
// G/Setter for model
// ====================================================================== //

glm::mat4 Renderable::model() const { return m_model; }
void      Renderable::model(glm::mat4 newModel) { m_model = newModel; }


// ====================================================================== //
// ====================================================================== //
// Add a vertex attribute to this Renderable
// ====================================================================== //

template <typename T>
void Renderable::addVBO(const std::vector<T>& data) {
  this->bind();
  {
    // Upload data to OpenGL
    unsigned int vbo;
    GL_ASSERT(glGenBuffers(1, &vbo));
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_ASSERT(glBufferData(
        GL_ARRAY_BUFFER, data.size() * sizeof(T), &data[0], GL_DYNAMIC_DRAW));
    // Link data to a vertex attribute (in order of entry)
    GL_ASSERT(glEnableVertexAttribArray(m_loc));
    GL_ASSERT(glVertexAttribPointer(
        m_loc, data[0].length(), GL_FLOAT, GL_FALSE, 0, 0));
    // Increment location index for vertex attribs
    // [!] Check that order in your shader is the order in which you add VBOs
    ++m_loc;
  }
  this->unbind();
}


// ====================================================================== //
// ====================================================================== //
// Store indices in the internal variable m_ebo
// ====================================================================== //

void Renderable::fillEBO(const std::vector<unsigned int>& indices) {
  this->bind();
  // Store indices count
  m_eboSize = indices.size();
  // Upload indices to OpenGL
  unsigned int ebo;
  GL_ASSERT(glGenBuffers(1, &ebo));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
  GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         m_eboSize * sizeof(unsigned int),
                         &indices[0],
                         GL_DYNAMIC_DRAW));
  this->unbind();
}

// ====================================================================== //
// ====================================================================== //
// Draw execute the Renderable in the viewport using its shader and vbos
// ====================================================================== //

void Renderable::draw() {
  this->bind();
  GL_ASSERT(glDrawElements(GL_TRIANGLES, m_eboSize, GL_UNSIGNED_INT, 0));
  this->unbind();
}

// ====================================================================== //
// ====================================================================== //
// Transform operations wrappers
// ====================================================================== //

void Renderable::translate(const glm::vec3& T) { Math::dTranslate(m_model, T); }
void Renderable::rotate(const glm::vec3& R) { Math::dRotate(m_model, R); }
void Renderable::scale(const glm::vec3& S) { Math::dScale(m_model, S); }

} // namespace BRAVE
