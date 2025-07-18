#include "mesh.h"
#include "glad/glad.h"

void setupMesh(Mesh &mesh);

Mesh createMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures) {
  Mesh mesh;
  mesh.vertices = vertices;
  mesh.indices = indices;
  mesh.textures = textures;

  setupMesh(mesh);
  return mesh;
}

void drawMesh(const Mesh &mesh, GLuint shader) {
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  setInt(shader, "tiling", 1);

  for (size_t i = 0; i < mesh.textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;

    std::string name = mesh.textures[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(diffuseNr++);
    else if (name == "texture_specular")
      number = std::to_string(specularNr++);

    setInt(shader, ("material." + name + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(mesh.VAO);
  glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void setupMesh(Mesh &mesh) {
  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glGenBuffers(1, &mesh.EBO);

  glBindVertexArray(mesh.VAO);

  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int),
               &mesh.indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texCoords));

  glBindVertexArray(0);
}
