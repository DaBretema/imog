#include <mutex>
#include <sstream>

#include "../incl/Renderable.hpp"
#include "../incl/wrap/Global.hpp"
#include "../incl/wrap/Glad.hpp"
#include "../incl/LoaderOBJ.hpp"


namespace BRAVE {


/// Empty ctor
Renderable::Renderable(const std::shared_ptr<Shader>& shader,
                       const glm::vec3&               color)
    : m_color(color) {
  m_ID     = ID_Counter_RenderableObjects++;
  m_shader = shader;
  m_shader->uFloat3("Color", m_color);
  GL_ASSERT(glGenVertexArrays(1, &m_vao));
}


/// OBJfile based ctor : Call to simpler constructor before load from obj
Renderable::Renderable(const std::string&             objFilePath,
                       const std::shared_ptr<Shader>& shader,
                       const glm::vec3&               color)
    : Renderable(shader, color) {
  RenderData renderData = loadOBJ(objFilePath);
  this->fillEBO(renderData.indices);
  this->addVBO(renderData.vertices);
  this->addVBO(renderData.normals);
}



// ID
uint64_t Renderable::ID() const { return m_ID; }


// Shader
std::shared_ptr<Shader> Renderable::shader() const { return m_shader; }
void Renderable::shader(const std::shared_ptr<Shader>& newShader) {
  m_shader = newShader;
}
// Color
glm::vec3 Renderable::color() const { return m_color; }
void      Renderable::color(glm::vec3 newColor) {
  m_color = newColor;
  m_shader->uFloat3("Color", m_color);
}
// Model
glm::mat4 Renderable::model() const { return m_model; }
void      Renderable::model(glm::mat4 newModel) { m_model = newModel; }



void Renderable::bind() {
  GL_ASSERT(glBindVertexArray(m_vao));
  m_shader->bind();
}

void Renderable::unbind() {
  GL_ASSERT(glBindVertexArray(0));
  m_shader->unbind();
  GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


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


void Renderable::draw() {
  this->bind();
  GL_ASSERT(glDrawElements(GL_TRIANGLES, m_eboSize, GL_UNSIGNED_INT, 0));
  this->unbind();
}


void Renderable::translate(const glm::vec3& T) { translateMat4(m_model, T); }
void Renderable::rotate(const glm::vec3& R) { rotateMat4(m_model, R); }
void Renderable::scale(const glm::vec3& S) { scaleMat4(m_model, S); }

} // namespace BRAVE
