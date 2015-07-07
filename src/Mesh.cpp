
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"

namespace vv
{
  Mesh::Mesh(const std::vector<Vertex> &vertices)
  {
    vertices_ = vertices;
    draw_elements_ = false;
    model_mat_ = glm::mat4(1.0);
    init();
  }

  Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
  {
    vertices_ = vertices;
    indices_ = indices;
    draw_elements_ = true;
    model_mat_ = glm::mat4(1.0);
    init();
  }

  Mesh::~Mesh()
  {
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);

    if (draw_elements_)
      glDeleteBuffers(1, &EBO_);
  }

  void Mesh::init()
  {
    VAO_ = 0;
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_); // from here on, all configurations pertain to this VAO.

    // think of this like a data structure on the device, we're generating a new storage space for our data
    // that we want draw, and then shipping the data from the host to the device.
    VBO_ = 0;

    // generate a buffer on the device
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);

    // fill the created buffer with our data
    glBufferData(GL_ARRAY_BUFFER,
                 this->vertices_.size() * sizeof(Vertex),
                 &this->vertices_[0],
                 GL_STATIC_DRAW);

    if (draw_elements_)
    {
      EBO_ = 0;
      glGenBuffers(1, &EBO_);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   this->indices_.size() * sizeof(GLuint),
                   &this->indices_[0],
                   GL_STATIC_DRAW);
    }

    // inform the GPU how to understand and process the incoming data
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

    // reset the currently active device buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // reset the currently active device buffer container
    glBindVertexArray(0);
  }

  void Mesh::translate(glm::vec3 translation)
  {
    model_mat_ = glm::translate(model_mat_, translation);
  }

  void Mesh::rotate(GLfloat angle, glm::vec3 axis)
  {
    model_mat_ = glm::rotate(model_mat_, angle, axis);
  }

  void Mesh::bindMatrices(Shader *shader)
  {
    GLint model_location = glGetUniformLocation(shader->getProgramId(), "model");
    glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model_mat_));
  }

  void Mesh::render()
  {
    glBindVertexArray(VAO_);

    if (draw_elements_)
      glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(this->indices_.size()), GL_UNSIGNED_INT, 0);
    else
      glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(this->vertices_.size()));

    glBindVertexArray(0);
  }
} // namespace vv