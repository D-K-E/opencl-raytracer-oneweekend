#ifndef SHADER_HPP
#define SHADER_HPP

// includes
#include <custom/external.hpp>
#include <stdexcept>
namespace fs = std::filesystem;

void checkShaderCompilation(GLuint shader, const char *shaderType,
                            const char *shaderFilePath) {
  // check the shader compilation
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == 0) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::" << shaderType << "::" << shaderFilePath
              << "::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
}

void checkShaderProgramCompilation(GLuint program, const char *shaderName) {
  // check the shader compilation
  int success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == 0) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::" << shaderName << "::PROGRAM"
              << "::LINK_FAILED\n"
              << infoLog << std::endl;
  }
}

void checkUniformLocation(int locVal, std::string uniName) {
  if (locVal == -1) {
    std::cout << "Shader program can not find the uniform location for "
              << uniName << std::endl;
  }
}

class Shader {
public:
  // program id
  GLuint programId;
  std::string shaderName = "MyShader";

  // constructor takes the path of the shaders and builts them
  Shader(const GLchar *vertexPath, const GLchar *fragmentPath);
  Shader(const GLchar *computePath);
  Shader(const GLchar *vertexPath, const GLchar *fragmentPath,
         const GLchar *computePath);
  Shader(std::vector<fs::path> shaderPaths,
         std::vector<std::string> shaderTypes);
  Shader(const GLchar *vertexSource, const GLchar *fragmentSource,
         bool fromSource);

  void useProgram();

  // utility functions for setting uniforms
  void setBoolUni(const std::string &name, bool value) const {
    // set boolean value to given uniform name
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform1i(uniLocation, (int)value);
  };
  void setIntUni(const std::string &name, int value) const {
    // set boolean value to given uniform name
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform1i(uniLocation, value);
  };
  void setFloatUni(const std::string &name, float value) const {
    // set boolean value to given uniform name
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform1f(uniLocation, value);
  };
  void setVec2Uni(const std::string &name, const glm::vec2 &value) const {
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform2fv(uniLocation, 1, glm::value_ptr(value));
  }
  void setVec2Uni(const std::string &name, float x, float y) const {
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform2f(uniLocation, x, y);
  }
  void setVec3Uni(const std::string &name, const glm::vec3 &value) const {
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform3fv(uniLocation, 1, glm::value_ptr(value));
  }
  void setVec3Uni(const std::string &name, float x, float y, float z) const {
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform3f(uniLocation, x, y, z);
  }
  void setVec4Uni(const std::string &name, const glm::vec4 &value) const {
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform4fv(uniLocation, 1, glm::value_ptr(value));
  }
  void setVec4Uni(const std::string &name, float x, float y, float z,
                  float w) const {
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform4f(uniLocation, x, y, z, w);
  }
  void setMat2Uni(const std::string &name, glm::mat2 &value) const {
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniformMatrix2fv(uniLocation, 1, GL_FALSE, glm::value_ptr(value));
  }
  void setMat3Uni(const std::string &name, glm::mat3 &value) const {
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniformMatrix3fv(uniLocation, 1, GL_FALSE, glm::value_ptr(value));
  }
  void setMat4Uni(const std::string &name, glm::mat4 &value) const {
    int uniLocation = glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniformMatrix4fv(uniLocation, 1, GL_FALSE, glm::value_ptr(value));
  }
  // load shader from file path
  GLuint loadShader(const GLchar *shaderFpath, const char *shdrType);
};

GLuint Shader::loadShader(const GLchar *shaderFilePath,
                          const GLchar *shaderType) {
  // load shader file from system
  GLuint shader;
  std::string stype(shaderType);
  if (stype == "FRAGMENT") {
    shader = glCreateShader(GL_FRAGMENT_SHADER);
  } else if (stype == "VERTEX") {
    shader = glCreateShader(GL_VERTEX_SHADER);
  } else if (stype == "COMPUTE") {
    shader = glCreateShader(GL_COMPUTE_SHADER);
  } else if (stype == "GEOMETRY") {
    shader = glCreateShader(GL_GEOMETRY_SHADER);
  } else {
    std::cout << "Unknown shader type:\n" << shaderType << std::endl;
  }
  std::ifstream shdrFileStream;
  shdrFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  std::string shaderCodeStr;
  try {
    shdrFileStream.open(shaderFilePath);
    std::stringstream shaderSStream;
    shaderSStream << shdrFileStream.rdbuf();
    shdrFileStream.close();
    shaderCodeStr = shaderSStream.str();
  } catch (std::ifstream::failure e) {
    //
    std::cout << "shader file path: " << shaderFilePath << std::endl;
    std::cout << "shader type: " << shaderType << std::endl;
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }
  const char *shaderCode = shaderCodeStr.c_str();

  // lets source the shader
  glShaderSource(shader, 1, &shaderCode, NULL);
  glCompileShader(shader);

  // a sanity check for unsuccessful compilations
  checkShaderCompilation(shader, shaderType, shaderFilePath);
  return shader;
}

// first constructor
Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
  // loading shaders
  this->programId = glCreateProgram();
  GLuint vshader = this->loadShader(vertexPath, "VERTEX");
  GLuint fshader = this->loadShader(fragmentPath, "FRAGMENT");
  glAttachShader(this->programId, vshader);
  glAttachShader(this->programId, fshader);
  glLinkProgram(this->programId);
  checkShaderProgramCompilation(this->programId, shaderName.c_str());
  glDeleteShader(vshader);
  glDeleteShader(fshader);
}
// third constructor
Shader::Shader(const GLchar *computePath) {
  // loading shaders
  this->programId = glCreateProgram();
  GLuint cshader = this->loadShader(computePath, "COMPUTE");
  glAttachShader(this->programId, cshader);
  glLinkProgram(this->programId);
  checkShaderProgramCompilation(this->programId, shaderName.c_str());
  glDeleteShader(cshader);
}
// second constructor
Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath,
               const GLchar *computePath) {
  // loading shaders
  this->programId = glCreateProgram();
  GLuint vshader = this->loadShader(vertexPath, "VERTEX");
  GLuint fshader = this->loadShader(fragmentPath, "FRAGMENT");
  GLuint cshader = this->loadShader(computePath, "COMPUTE");
  glAttachShader(this->programId, vshader);
  glAttachShader(this->programId, fshader);
  glAttachShader(this->programId, cshader);
  glLinkProgram(this->programId);
  checkShaderProgramCompilation(this->programId, shaderName.c_str());
  glDeleteShader(vshader);
  glDeleteShader(fshader);
  glDeleteShader(cshader);
}

Shader::Shader(const GLchar *vertexSource, const GLchar *fragmentSource,
               bool fromSource) {
  if (!fromSource) {
    std::invalid_argument("if passed source to args, check must be true");
  }
  this->programId = glCreateProgram();
  GLuint vshader;
  glShaderSource(vshader, 1, &vertexSource, NULL);
  glCompileShader(vshader);

  // a sanity check for unsuccessful compilations
  checkShaderCompilation(vshader, "VERTEX", "from source");
  glAttachShader(this->programId, vshader);
  GLuint fshader;
  glShaderSource(fshader, 1, &fragmentSource, NULL);
  glCompileShader(fshader);

  // a sanity check for unsuccessful compilations
  checkShaderCompilation(fshader, "FRAGMENT", "from source");
  glAttachShader(this->programId, fshader);
  glLinkProgram(this->programId);
  checkShaderProgramCompilation(this->programId, shaderName.c_str());
  glDeleteShader(vshader);
  glDeleteShader(fshader);
}

// fourth more general constructor
Shader::Shader(std::vector<fs::path> shaderPaths,
               std::vector<std::string> shaderTypes) {
  if (shaderPaths.size() != shaderTypes.size()) {
    throw std::invalid_argument(
        "shaderPath vector has different size than shaderTypes vector");
  }
  this->programId = glCreateProgram();
  std::vector<GLuint> shaderIds;
  for (unsigned int i = 0; i < shaderTypes.size(); i++) {
    //
    GLuint shdrId =
        this->loadShader(shaderPaths[i].c_str(), shaderTypes[i].c_str());
    glAttachShader(this->programId, shdrId);
    shaderIds.push_back(shdrId);
  }
  glLinkProgram(this->programId);
  checkShaderProgramCompilation(this->programId, shaderName.c_str());
  for (unsigned int i = 0; i < shaderTypes.size(); i++) {
    //
    glDeleteShader(shaderIds[i]);
  }
}

void Shader::useProgram() { glUseProgram(this->programId); }

#endif
