#ifndef UTILS_HPP
#define UTILS_HPP

#include <custom/debug.hpp>
#include <custom/renderer.hpp>
#include <custom/singulars.hpp>

// light related
void renderLamp();

// function declarations

/**
  glfw outputs errors

  @param desc: error parameter
  */
static void glfwErrorCallBack(int id, const char *desc);
void initializeGLFWMajorMinor(unsigned int maj, unsigned int min);
void framebuffer_size_callback(GLFWwindow *window, int newWidth, int newHeight);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow *window, double xpos, double ypos);
void saveImage(const char *fpath, GLFWwindow *window);
void processInput_proc(GLFWwindow *window);
void cubeShaderInit_proc(Shader myShader);
GLuint loadCubeMap(std::vector<fs::path> cubeFaces);

void framebuffer_size_callback(GLFWwindow *window, int newWidth,
                               int newHeight) {
  glViewport(0, 0, newWidth, newHeight);
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xoffset;
  lastY = yoffset;

  camera.processMouseMovement(xoffset, yoffset);
}
void mouse_scroll_callback(GLFWwindow *window, double xpos, double ypos) {
  camera.processMouseScroll(ypos);
}
void moveCamera(GLFWwindow *window) {
  //
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.processKeyboard(FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.processKeyboard(LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.processKeyboard(BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.processKeyboard(RIGHT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
    camera.processKeyBoardRotate(LEFT, 0.7f);
  }
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
    camera.processKeyBoardRotate(RIGHT, 0.7f);
  }
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
    camera.processKeyBoardRotate(FORWARD, 0.7f);
  }
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    camera.processKeyBoardRotate(BACKWARD, 0.7f);
  }
}

void moveLight(GLFWwindow *window) {
  // move light
  if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
    lightPos.y += deltaTime;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    lightPos.y -= deltaTime;
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    lightPos.x += deltaTime;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    lightPos.x -= deltaTime;
  }
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    lightPos.z -= deltaTime; // the axis are inverse
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    lightPos.z += deltaTime;
  }
}
void rotateScaleLight(GLFWwindow *window) {
  //
}
void moveObject(GLFWwindow *window) {
  //
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    transVec.x = deltaTime;
    transVec.y = 0.0f;
    transVec.z = 0.0f;
    modelMat = glm::translate(modelMat, transVec);
  }
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    transVec.x = -deltaTime;
    transVec.y = 0.0f;
    transVec.z = 0.0f;

    modelMat = glm::translate(modelMat, transVec);
  }
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    transVec.y = deltaTime;
    transVec.x = 0.0f;
    transVec.z = 0.0f;

    modelMat = glm::translate(modelMat, transVec);
  }
  if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
    transVec.y = -deltaTime;
    transVec.x = 0.0f;
    transVec.z = 0.0f;

    modelMat = glm::translate(modelMat, transVec);
  }
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
    transVec.z = deltaTime;
    transVec.y = 0.0f;
    transVec.x = 0.0f;

    modelMat = glm::translate(modelMat, transVec);
  }
  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
    transVec.z = -0.05f;
    transVec.y = 0.0f;
    transVec.x = 0.0f;

    modelMat = glm::translate(modelMat, transVec);
  }
}
void rotateScaleObject(GLFWwindow *window) {
  //
  int axis = 0;
  if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
    scale += glm::vec3(0.01f);
    modelMat = glm::scale(modelMat, scale);
  }
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    scale -= glm::vec3(-0.01f);
    modelMat = glm::scale(modelMat, scale);
  }
  if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
    angle -= 0.005f;
  }
  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
    angle += 0.005f;
  }
  angle = angle < -180 || angle > 180 ? 0 : angle;
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
    axis += 1;
    axis = axis <= 2 ? axis : 0;
  }
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
    //
    if (axis == 0) {
      modelMat = glm::rotate(modelMat, glm::radians(angle), xaxis);
    } else if (axis == 1) {
      modelMat = glm::rotate(modelMat, glm::radians(angle), yaxis);
    } else if (axis == 2) {
      modelMat = glm::rotate(modelMat, glm::radians(angle), zaxis);
    }
  }
}
void captureScreen(GLFWwindow *window) {
  // screen capture
  std::string fname_prefix("capture");
  std::string captCount = std::to_string(captCounter++);
  std::string ext(".png");
  std::string fname = fname_prefix + captCount + ext;
  fs::path captureImagePath = capturePath / fname.c_str();
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    saveImage(captureImagePath.c_str(), window);
  }
}

void processInput_proc(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  moveCamera(window);
  moveLight(window);
  rotateScaleLight(window);
  moveObject(window);
  rotateScaleObject(window);
  captureScreen(window);
}
void showKeys() {
  // show keys
  std::cout << "Move Rotate Camera Keys:" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "W/Z move forward" << std::endl;
  std::cout << "A/Q move left" << std::endl;
  std::cout << "S move backward" << std::endl;
  std::cout << "D move right" << std::endl;
  std::cout << "H rotate left" << std::endl;
  std::cout << "J rotate right" << std::endl;
  std::cout << "K rotate downward" << std::endl;
  std::cout << "L rotate upward" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "Move Light Keys:" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "Minus/) move upward" << std::endl;
  std::cout << "space move downward" << std::endl;
  std::cout << "Right Arrow move right" << std::endl;
  std::cout << "Left Arrow move left" << std::endl;
  std::cout << "Up Arrow move forward" << std::endl;
  std::cout << "Down Arrow move backward" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "Move Object Keys:" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "E move left" << std::endl;
  std::cout << "R move right" << std::endl;
  std::cout << "T move up" << std::endl;
  std::cout << "Y move down" << std::endl;
  std::cout << "I move forward" << std::endl;
  std::cout << "O move backward" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "Rotate Scale Object Keys:" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "X scale increase" << std::endl;
  std::cout << "C scale decrease" << std::endl;
  std::cout << "V angle decrease" << std::endl;
  std::cout << "B angle increase" << std::endl;
  std::cout << "N change rotation axis" << std::endl;
  std::cout << "M rotate on axis" << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "C Capture Screen" << std::endl;
}

static void glfwErrorCallBack(int id, const char *desc) {
  std::cout << desc << std::endl;
}
void initializeGLFWMajorMinor(unsigned int maj, unsigned int min) {
  // initialize glfw version with correct profiling etc
  glfwSetErrorCallback(glfwErrorCallBack);
  if (glfwInit() == 0) {
    std::cout << "glfw not initialized correctly" << std::endl;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, maj);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, min);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLuint loadTexture2d(const char *texturePath, GLuint tex,
                     bool isGamma = false) {
  // create and load, bind texture to gl

  int width, height, nbChannels;
  unsigned char *data = stbi_load(texturePath, &width, &height, &nbChannels, 0);
  if (data) {
    GLenum format;
    GLenum informat;
    if (nbChannels == 1) {
      informat = format = GL_RED;
    } else if (nbChannels == 3) {
      format = GL_RGB;
      informat = isGamma ? GL_SRGB : GL_RGB;

    } else if (nbChannels == 4) {
      format = GL_RGBA;
      informat = isGamma ? GL_SRGB_ALPHA : GL_RGBA;
    }
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, informat, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // use nearest neighbor interpolation when zooming out
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // use nearest neighbor interpolation when zooming out
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  } else {
    std::cout << "Failed to load texture from path:" << std::endl;
    std::cout << texturePath << std::endl;
  }
  stbi_image_free(data);
  return tex;
}

GLuint loadTexture2d(const char *fpath, bool igamma = false) {
  //
  GLuint tex;
  glGenTextures(1, &tex);
  tex = loadTexture2d(fpath, tex, igamma);

  return tex;
}
GLuint loadTexture2d(const char *parent, const char *fpath,
                     bool igamma = false) {
  //
  GLuint tex;
  glGenTextures(1, &tex);
  fs::path tpath = textureDirPath / parent / fpath;
  tex = loadTexture2d(tpath.c_str(), tex, igamma);

  return tex;
}

void loadHdrTexture(const char *parent, const char *tpath, GLuint &hdrTexture) {
  // load hdr environment map
  stbi_set_flip_vertically_on_load(true);

  fs::path fpath = textureDirPath / parent / tpath;
  int width, height, nrComponents;
  float *data = stbi_loadf(fpath.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT,
        data); // note how we specify the texture's data value to be float

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Failed to load HDR image." << std::endl;
  }
}
GLuint loadHdrTexture(const char *parent, const char *tpath) {
  // load hdr environment map
  GLuint tex;
  loadHdrTexture(parent, tpath, tex);
  return tex;
}
GLuint loadHdrTexture(int w, int h) {
  // load hdr environment map
  GLuint hdrTexture;
  glGenTextures(1, &hdrTexture);
  glBindTexture(GL_TEXTURE_2D, hdrTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RG, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return hdrTexture;
}

GLuint loadHdrTexture(int w, int h, GLenum informat, GLenum outformat) {
  // load hdr environment map
  GLuint hdrTexture;
  glGenTextures(1, &hdrTexture);
  glBindTexture(GL_TEXTURE_2D, hdrTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, informat, w, h, 0, outformat, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return hdrTexture;
}

GLuint loadTexture2d(const char *texturePath, GLuint tex, unsigned int i,
                     bool isGamma = false) {
  //
  glActiveTexture(GL_TEXTURE0 + i);
  return loadTexture2d(texturePath, tex, isGamma);
}
void saveImage(const char *fpath, GLFWwindow *window) {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  GLsizei nbComponents = 3;
  //
  GLsizei stride = nbComponents * w;
  stride += (stride % 4) ? (4 - stride % 4) : 0;
  GLsizei bsize = stride * h;
  std::vector<char> buffer(bsize);
  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glReadBuffer(GL_FRONT);
  glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
  stbi_flip_vertically_on_write(true);
  stbi_write_png(fpath, w, h, nbComponents, buffer.data(), stride);
}
GLuint loadCubeMap(std::vector<fs::path> paths) {
  GLuint texId;
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
  int w, h, nbChannels;
  for (unsigned int i = 0; i < paths.size(); i++) {
    //
    unsigned char *data = stbi_load(paths[i].c_str(), &w, &h, &nbChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0,
                   GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
      std::cout << "Image not loaded cube side " << i << std::endl;
    }
    stbi_image_free(data);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  return texId;
}

void cubeShaderInit_proc(Shader myShader) {
  myShader.useProgram();
  float ambientCoeff = 0.1f;
  float shininess = 200.0f;
  glm::vec3 attc(1.0f, 0.0f, 0.0f);
  myShader.setFloatUni("ambientCoeff", ambientCoeff);
  myShader.setFloatUni("shininess", shininess);
  myShader.setVec3Uni("attC", attc);
  myShader.setIntUni("diffuseMap1", 0);
  myShader.setIntUni("specularMap1", 1);
  myShader.setIntUni("normalMap1", 2);
}

/**
  A convenience function for loading a 2D texture
 */
GLuint loadA2DTexture() {
  //
  fs::path stonePath = textureDirPath / "Stone_001_Diffuse.png";
  GLuint stone_texture = loadTexture2d(stonePath.c_str());
  return stone_texture;
}

glm::vec4 getNdcFromFrag(glm::vec4 frag, glm::vec4 viewport,
                         glm::vec2 nearFar) {
  //
  glm::vec4 ndc;
  glm::vec2 viewxy = glm::vec2(viewport.x, viewport.y);
  glm::vec2 viewzw = glm::vec2(viewport.z, viewport.w);
  glm::vec2 fragxy = glm::vec2(frag.x, frag.y);
  fragxy *= 2.0;
  viewxy *= 2.0;
  fragxy -= viewxy;
  fragxy /= viewzw;
  fragxy -= 1.0;
  glm::vec2 nd2 = fragxy;
  ndc.x = nd2.x;
  ndc.y = nd2.y;
  ndc.z = (2.0 * frag.z - nearFar.x - nearFar.y) / (nearFar.y - nearFar.x);
  ndc.w = 1.0;
  return ndc;
}

glm::vec4 getClipFromFrag(glm::vec4 frag, glm::vec4 viewport,
                          glm::vec2 nearFar) {
  glm::vec4 ndc = getNdcFromFrag(frag, viewport, nearFar);
  return ndc / frag.w;
}

#endif
