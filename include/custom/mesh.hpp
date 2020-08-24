#ifndef MESH_HPP
#define MESH_HPP
//
#include <glad/glad.h>
//
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
#include <custom/shader.hpp>
#include <filesystem>
#include <string>
#include <vector>
namespace fs = std::filesystem;

//
struct Vertex {
  // pos
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 texCoord;
  //
  glm::vec3 tangent;
  glm::vec3 bitangent;
};

enum class TextureType {
  Specular,
  Normal,
  Diffuse,
  Height,
  Displacement,
  Roughness,
  Metallic,
  Ao,
  Albedo
};

inline std::ostream &operator<<(std::ostream &out, const TextureType &t) {
  switch (t) {
  case TextureType::Diffuse:
    return out << "type diffuse";
  case TextureType::Specular:
    return out << "type specular";
  case TextureType::Normal:
    return out << "type normal";
  case TextureType::Height:
    return out << "type height";
  case TextureType::Displacement:
    return out << "type displacement";
  case TextureType::Ao:
    return out << "type ambient occlusion";
  case TextureType::Roughness:
    return out << "type roughness";
  case TextureType::Albedo:
    return out << "type albedo";
  case TextureType::Metallic:
    return out << "type metallic";
  }
}

struct Texture {
  GLuint id;
  TextureType type;
  std::string name;
  fs::path path;
};

class Mesh {
  //
public:
  std::vector<Vertex> vertices;
  std::vector<Texture> textures;
  std::vector<unsigned int> indices;
  //
  GLuint vao;
  Mesh(std::vector<Vertex> vs, std::vector<Texture> txts,
       std::vector<unsigned int> inds)
      : vertices(vs), indices(inds), textures(txts) {
    setupMesh();
  }
  void draw(Shader shdr) {
    // bind textures
    unsigned int diffuseNb = 1;
    unsigned int specularNb = 1;
    unsigned int normalNb = 1;
    unsigned int heightNb = 1;
    unsigned int dispNb = 1;
    unsigned int aoNb = 1;
    unsigned int roughNb = 1;
    unsigned int metalNb = 1;
    unsigned int albedoNb = 1;
    for (int i = 0; i < textures.size(); i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      std::string nb;
      TextureType ttype = textures[i].type;
      std::string tname = textures[i].name;
      switch (ttype) {
      case TextureType::Diffuse:
        nb = std::to_string(diffuseNb++);
        break;
      case TextureType::Specular:
        nb = std::to_string(specularNb++);
        break;
      case TextureType::Normal:
        nb = std::to_string(normalNb++);
        break;
      case TextureType::Height:
        nb = std::to_string(heightNb++);
        break;
      case TextureType::Displacement:
        nb = std::to_string(dispNb++);
        break;
      case TextureType::Ao:
        nb = std::to_string(aoNb++);
        break;
      case TextureType::Roughness:
        nb = std::to_string(roughNb++);
        break;
      case TextureType::Albedo:
        nb = std::to_string(albedoNb++);
        break;
      case TextureType::Metallic:
        nb = std::to_string(metalNb++);
        break;
      }
      shdr.setIntUni(tname + nb, i);
      glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //
    glActiveTexture(GL_TEXTURE0);
  }

private:
  GLuint ebo, vbo;

  void setupMesh() {
    //
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // bind vao
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
    //
    // vertex pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // vertex normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));
    // texcoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, texCoord));

    // tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, tangent));
    // bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, bitangent));

    //
    glBindVertexArray(0);
  }
};

#endif
