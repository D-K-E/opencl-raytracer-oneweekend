// license: see, LICENSE
// render objects
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <custom/debug.hpp>
#include <custom/singulars.hpp>

glm::vec3 getTangent(glm::vec2 deltaUV2, glm::vec2 deltaUV1, glm::vec3 edge1,
                     glm::vec3 edge2) {
  // Compute tangent
  GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
  glm::vec3 tangent;
  tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
  tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
  tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
  tangent = glm::normalize(tangent);
  return tangent;
}
glm::vec3 getBiTangent(glm::vec2 deltaUV2, glm::vec2 deltaUV1, glm::vec3 edge1,
                       glm::vec3 edge2) {
  GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
  glm::vec3 bitangent;
  bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
  bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
  bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
  bitangent = glm::normalize(bitangent);
  return bitangent;
}

struct VertexAttrib {
  GLuint loc;
  GLuint size;
};
bool compareAttribs(const VertexAttrib &v1, const VertexAttrib &v2) {
  return v1.loc < v2.loc;
}

/**
  Set vertex attributes of a shader
 */
void setVertexAttrib(std::vector<VertexAttrib> vs) {

  // deal with size
  size_t fsize = 0 * sizeof(float);
  std::sort(vs.begin(), vs.end(), compareAttribs); // 1,2,3,4
  for (const auto &v : vs) {
    fsize += v.size * sizeof(float);
  }
  // now to locations
  size_t voffset = 0 * sizeof(float); // attribute offset
  for (const auto &v : vs) {
    glEnableVertexAttribArray(v.loc); // location
    glVertexAttribPointer(v.loc,      // location ==  aPos
                          v.size,     // vec3
                          GL_FLOAT, GL_FALSE, fsize, (void *)voffset);
    voffset += v.size * sizeof(float);
  }
}

void renderTriangleInTangentSpace(float vert[15], float normal[3],
        std::vector<VertexAttrib> vs) {
  GLuint triVBO, triVAO;
  glGenBuffers(1, &triVBO);
  glGenVertexArrays(1, &triVAO);

  // triangle points
  glm::vec3 p1(vert[0], vert[1], vert[2]);
  glm::vec2 tex1(vert[3], vert[4]);
  glm::vec3 p2(vert[5], vert[6], vert[7]);
  glm::vec2 tex2(vert[8], vert[9]);
  glm::vec3 p3(vert[10], vert[11], vert[12]);
  glm::vec2 tex3(vert[13], vert[14]);

  // there are two triangles in a square
  glm::vec3 snormal(normal[0], normal[1], normal[2]);

  // first triangle
  glm::vec3 edge1 = p2 - p1;
  glm::vec3 edge2 = p3 - p1;
  glm::vec2 deltaUV1 = tex2 - tex1;
  glm::vec2 deltaUV2 = tex3 - tex1;

  glm::vec3 tan = getTangent(deltaUV2, deltaUV1, edge1, edge2);
  glm::vec3 bitan = getBiTangent(deltaUV2, deltaUV1, edge1, edge2);

  float trivert[] = {
      p1.x,   p1.y,  p1.z,  snormal.x, snormal.y, snormal.z, tex1.x,
      tex1.y, tan.x, tan.y, tan.z,     bitan.x,   bitan.y,   bitan.z,
      p2.x,   p2.y,  p2.z,  snormal.x, snormal.y, snormal.z, tex2.x,
      tex2.y, tan.x, tan.y, tan.z,     bitan.x,   bitan.y,   bitan.z,
      p3.x,   p3.y,  p3.z,  snormal.x, snormal.y, snormal.z, tex3.x,
      tex3.y, tan.x, tan.y, tan.z,     bitan.x,   bitan.y,   bitan.z,
  };
  glBindVertexArray(triVAO);
  glBindBuffer(GL_ARRAY_BUFFER, triVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(trivert), &trivert, GL_STATIC_DRAW);
  // specify attributes
  setVertexAttrib(vs);
  gerr();
  glBindVertexArray(triVAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &triVAO);
  glDeleteBuffers(1, &triVBO);
}

void renderTriangleInTangentSpace(float vert[15], float normal[3]) {
  std::vector<VertexAttrib> vs{
      {0, 3}, // aPos
      {1, 3}, // aNormal
      {2, 2}, // aTexCoord
      {3, 3}, // aTan
      {4, 3}, // aBitan
  };
  renderTriangleInTangentSpace(vert, normal, vs);
}

void renderTriangle(float vert[15], float normal[3],
                    std::vector<VertexAttrib> vs) {
  GLuint triVBO, triVAO;
  glGenBuffers(1, &triVBO);
  glGenVertexArrays(1, &triVAO);

  // there are two triangles in a square
  float trivert[] = {
      vert[0],   vert[1],   vert[2],   normal[0], normal[1], normal[2],
      vert[3],   vert[4],   vert[5],   vert[6],   vert[7],   normal[0],
      normal[1], normal[2], vert[8],   vert[9],   vert[10],  vert[11],
      vert[12],  normal[0], normal[1], normal[2], vert[13],  vert[14],
  };
  glBindVertexArray(triVAO);
  glBindBuffer(GL_ARRAY_BUFFER, triVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(trivert), &trivert, GL_STATIC_DRAW);
  // specify attributes
  setVertexAttrib(vs);
  glBindVertexArray(triVAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &triVAO);
  glDeleteBuffers(1, &triVBO);
}
void renderTriangle(float vert[15], float normal[3]) {
  std::vector<VertexAttrib> vs{
      {0, 3},
      {1, 3},
      {2, 2},
  };
  renderTriangle(vert, normal, vs);
}
void renderLamp() {
  GLuint vbo, lightVao;
  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &lightVao); // separate object to isolate lamp from
  float vert[] = {-0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f};

  glBindVertexArray(lightVao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vert), &vert, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glBindVertexArray(lightVao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &lightVao);
  glDeleteBuffers(1, &vbo);
}
void renderCubeInTangentSpace(std::vector<VertexAttrib> vs) {
  /*
     Draw cube
   */
  float s1n[] = {0.0f, 0.0f, -1.0f};
  float s2n[] = {0.0f, 0.0f, 1.0f};
  float s3n[] = {-1.0f, 0.0f, 0.0f};
  float s4n[] = {1.0f, 0.0f, 0.0f};
  float s5n[] = {0.0f, -1.0f, 0.0f};
  float s6n[] = {0.0f, 1.0f, 0.0f};

  // positions        // texture coords
  float t1[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.5f, -0.5f, -0.5f,
      1.0f,  0.0f,  0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
  };
  renderTriangleInTangentSpace(t1, s1n, vs);
  float tt1[] = {
      0.5f, 0.5f, -0.5f, 1.0f,  1.0f,  -0.5f, 0.5f, -0.5f,
      0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,
  };
  renderTriangleInTangentSpace(tt1, s1n, vs);

  float t2[] = {
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f,
      1.0f,  0.0f,  0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
  };

  renderTriangleInTangentSpace(t2, s2n, vs);
  float tt2[] = {
      0.5f, 0.5f, 0.5f,  1.0f,  1.0f, -0.5f, 0.5f, 0.5f,
      0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f,  0.0f,
  };

  renderTriangleInTangentSpace(tt2, s2n, vs);

  float t3[] = {
      -0.5f, 0.5f, 0.5f,  1.0f,  0.0f,  -0.5f, 0.5f, -0.5f,
      1.0f,  1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,
  };
  renderTriangleInTangentSpace(t3, s3n, vs);

  float tt3[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,
      0.0f,  0.0f,  -0.5f, 0.5f, 0.5f, 1.0f,  0.0f,
  };
  renderTriangleInTangentSpace(tt3, s3n, vs);

  float t4[] = {
      0.5f, 0.5f, 0.5f, 1.0f,  0.0f,  0.5f, 0.5f, -0.5f,
      1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
  };

  renderTriangleInTangentSpace(t4, s4n, vs);
  float tt4[] = {
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f,
      0.0f, 0.0f,  0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangleInTangentSpace(tt4, s4n, vs);

  float t5[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  1.0f, 0.5f, -0.5f, -0.5f,
      1.0f,  1.0f,  0.5f,  -0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangleInTangentSpace(t5, s5n, vs);

  float tt5[] = {
      0.5f, -0.5f, 0.5f,  1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,
  };

  renderTriangleInTangentSpace(tt5, s5n, vs);

  float t6[] = {
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f,
      1.0f,  1.0f, 0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangleInTangentSpace(t6, s6n, vs);

  float tt6[] = {0.5f, 0.5f, 0.5f,  1.0f, 0.0f,  -0.5f, 0.5f, 0.5f,
                 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f,  1.0f};

  renderTriangleInTangentSpace(tt6, s6n, vs);
}
void renderCubeInTangentSpace() {
  /*
     Draw cube
   */
  std::vector<VertexAttrib> vs{
      {0, 3}, // aPos
      {1, 3}, // aNormal
      {2, 2}, // aTexCoord
      {3, 3}, // aTan
      {4, 3}, // aBitan
  };
  renderCubeInTangentSpace(vs);
}
void renderCube(std::vector<VertexAttrib> vs) {
  /*
     Draw cube
   */
  float s1n[] = {0.0f, 0.0f, -1.0f};
  float s2n[] = {0.0f, 0.0f, 1.0f};
  float s3n[] = {-1.0f, 0.0f, 0.0f};
  float s4n[] = {1.0f, 0.0f, 0.0f};
  float s5n[] = {0.0f, -1.0f, 0.0f};
  float s6n[] = {0.0f, 1.0f, 0.0f};

  // positions        // texture coords
  float t1[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.5f, -0.5f, -0.5f,
      1.0f,  0.0f,  0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
  };
  renderTriangle(t1, s1n, vs);
  float tt1[] = {
      0.5f, 0.5f, -0.5f, 1.0f,  1.0f,  -0.5f, 0.5f, -0.5f,
      0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,
  };
  renderTriangle(tt1, s1n, vs);

  float t2[] = {
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f,
      1.0f,  0.0f,  0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
  };

  renderTriangle(t2, s2n, vs);
  float tt2[] = {
      0.5f, 0.5f, 0.5f,  1.0f,  1.0f, -0.5f, 0.5f, 0.5f,
      0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f,  0.0f,
  };

  renderTriangle(tt2, s2n, vs);

  float t3[] = {
      -0.5f, 0.5f, 0.5f,  1.0f,  0.0f,  -0.5f, 0.5f, -0.5f,
      1.0f,  1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,
  };
  renderTriangle(t3, s3n, vs);

  float tt3[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,
      0.0f,  0.0f,  -0.5f, 0.5f, 0.5f, 1.0f,  0.0f,
  };
  renderTriangle(tt3, s3n, vs);

  float t4[] = {
      0.5f, 0.5f, 0.5f, 1.0f,  0.0f,  0.5f, 0.5f, -0.5f,
      1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
  };

  renderTriangle(t4, s4n, vs);
  float tt4[] = {
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f,
      0.0f, 0.0f,  0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangle(tt4, s4n, vs);

  float t5[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  1.0f, 0.5f, -0.5f, -0.5f,
      1.0f,  1.0f,  0.5f,  -0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangle(t5, s5n, vs);

  float tt5[] = {
      0.5f, -0.5f, 0.5f,  1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,
  };

  renderTriangle(tt5, s5n, vs);

  float t6[] = {
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f,
      1.0f,  1.0f, 0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangle(t6, s6n, vs);

  float tt6[] = {0.5f, 0.5f, 0.5f,  1.0f, 0.0f,  -0.5f, 0.5f, 0.5f,
                 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f,  1.0f};

  renderTriangle(tt6, s6n, vs);
}
void renderCube() {
  /*
     Draw cube
   */
  std::vector<VertexAttrib> vs{
      {0, 3},
      {1, 3},
      {2, 2},
  };
  renderCube(vs);
}
void renderCubeD(std::vector<VertexAttrib> vs) {
  //
  float vertices[] = {
      //  position         // normal         // texture coord
      -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
      1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
      1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
      1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
      -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
      -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
      // front face
      -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
      1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
      1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
      1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
      -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
      -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
      // left face
      -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
      -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
      -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
      -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                          // right face
      1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
      1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
      1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
      1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
      1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
      1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
      // bottom face
      -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
      1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
      1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
      1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
      -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
      -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
      // top face
      -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
      1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
      1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
      1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
      -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
      -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
  };
  GLuint cubeVAO;
  GLuint cubeVBO;

  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  //
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
  glBindVertexArray(cubeVAO);
  // ----------------- shader specific -----------------
  setVertexAttrib(vs);
  //
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // render
  glBindVertexArray(cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteBuffers(1, &cubeVBO);
}

void renderCubeD() {
  //
  std::vector<VertexAttrib> vs{{0, 3}, {1, 3}, {2, 2}};
  renderCubeD(vs);
}
void renderQuad(std::vector<VertexAttrib> vs) {
  GLuint quadVAO;
  GLuint quadVBO;

  float screen[] = {//
                    // positions    texCoords //
                    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
                    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //

                    1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
                    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f}; //
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screen), &screen, GL_STATIC_DRAW);
  // ------------- shader specific -------------
  setVertexAttrib(vs);
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);
}
void renderQuad() {
  std::vector<VertexAttrib> vs{
      {0, 3}, // aPos
      {1, 2}  // aTexCoord
  };
  renderQuad(vs);
}
// renders (and builds at first invocation) a sphere
// -------------------------------------------------
void renderSphere(std::vector<VertexAttrib> vs) {
  unsigned int sphereVAO;
  glGenVertexArrays(1, &sphereVAO);
  unsigned int indexCount;

  unsigned int vbo, ebo;
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> uv;
  std::vector<glm::vec3> normals;
  std::vector<unsigned int> indices;

  const unsigned int X_SEGMENTS = 64;
  const unsigned int Y_SEGMENTS = 64;
  const float PI = 3.14159265359;
  for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
      float xSegment = (float)x / (float)X_SEGMENTS;
      float ySegment = (float)y / (float)Y_SEGMENTS;
      float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
      float yPos = std::cos(ySegment * PI);
      float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

      positions.push_back(glm::vec3(xPos, yPos, zPos));
      uv.push_back(glm::vec2(xSegment, ySegment));
      normals.push_back(glm::vec3(xPos, yPos, zPos));
    }
  }

  bool oddRow = false;
  for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
    if (!oddRow) // even rows: y == 0, y == 2; and so on
    {
      for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
        indices.push_back(y * (X_SEGMENTS + 1) + x);
        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
      }
    } else {
      for (int x = X_SEGMENTS; x >= 0; --x) {
        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        indices.push_back(y * (X_SEGMENTS + 1) + x);
      }
    }
    oddRow = !oddRow;
  }
  indexCount = indices.size();

  std::vector<float> data;
  for (unsigned int i = 0; i < positions.size(); ++i) {
    data.push_back(positions[i].x);
    data.push_back(positions[i].y);
    data.push_back(positions[i].z);
    if (uv.size() > 0) {
      data.push_back(uv[i].x);
      data.push_back(uv[i].y);
    }
    if (normals.size() > 0) {
      data.push_back(normals[i].x);
      data.push_back(normals[i].y);
      data.push_back(normals[i].z);
    }
  }
  glBindVertexArray(sphereVAO);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0],
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);
  setVertexAttrib(vs);

  glBindVertexArray(sphereVAO);
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &sphereVAO);
  glDeleteBuffers(1, &vbo);
}

void renderSphere() {
  std::vector<VertexAttrib> vs{
      {0, 3}, // aPos
      {1, 2}, // aPos
      {2, 3}, // aPos
  };
  renderSphere(vs);
}
#endif
