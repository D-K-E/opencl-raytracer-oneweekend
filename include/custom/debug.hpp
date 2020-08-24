// debug utilities
#ifndef DEBUG_HPP
#define DEBUG_HPP
#include <custom/external.hpp>

const bool DEBUG = true;

// debug function
GLenum gerror(const char *file, int line) {
  // from learnopengl.com
  GLenum errorCode = glGetError();
  if (errorCode != GL_NO_ERROR) {
    std::string error;
    switch (errorCode) {
    case GL_INVALID_ENUM:
      error = "INVALID_ENUM: enumeration parameter is not legal";
      break;
    case GL_INVALID_VALUE:
      error = "INVALID_VALUE: value parameter is not legal";
      break;
    case GL_INVALID_OPERATION:
      error = "INVALID_OPERATION: command not ready for given parameters";
      break;
    case GL_STACK_OVERFLOW:
      error = "STACK_OVERFLOW: stack push causes overflow";
      break;
    case GL_STACK_UNDERFLOW:
      error = "STACK_UNDERFLOW: stack pop at lowest point";
      break;
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY: cannot allocate enough memory for operation";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "INVALID_FRAMEBUFFER_OPERATION: fbo not complete for reading or "
              "writting";
      break;
    }
    std::cout << error << " | " << file << " (" << line << ")" << std::endl;
  }
  return errorCode;
}
GLenum gerrorf_(const char *file, int line) {
  GLenum errorCode = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  std::string error;
  if (errorCode != GL_FRAMEBUFFER_COMPLETE) {
    switch (errorCode) {
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      error = "INCOMPLETE_ATTACHMENT: any of the framebuffer attachment points "
              "are framebuffer incomplete";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      error = "INCOMPLETE_MISSING_ATTACHMENT: framebuffer does not have at "
              "least one image attached to it";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      error = "INCOMPLETE_DRAW_BUFFER: attachment object type is none for any"
              "of the attachment points named by draw buffers";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      error = "INCOMPLETE_READ_BUFFER: attachment object type is none for any"
              "of the attachment points named by read buffers";
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      error = "UNSUPPORTED:combination of internal formats of the attached "
              "images violates an implementation-dependent set of "
              "restrictions.";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
      error =
          "INCOMPLETE_MULTISAMPLE:if the value of GL_RENDERBUFFER_SAMPLES "
          "is not the same for all attached renderbuffers; if the value of "
          "GL_TEXTURE_SAMPLES is the not same for all attached textures; "
          "or, if the attached images are a mix of renderbuffers and "
          "textures, the value of GL_RENDERBUFFER_SAMPLES does not match "
          "the value of GL_TEXTURE_SAMPLES.if the value of "
          "GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached "
          "textures; or, if the attached images are a mix of renderbuffers and "
          "textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not "
          "GL_TRUE for all attached textures.";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
      error =
          "INCOMPLETE_LAYER_TARGETS: framebuffer attachment is layered, and "
          "any populated attachment is not layered, or if all populated "
          "color attachments are not from textures of the same target";
      break;
    case GL_INVALID_ENUM:
      error = "generated if target is not GL_DRAW_FRAMEBUFFER, "
              "GL_READ_FRAMEBUFFER or GL_FRAMEBUFFER";
    }
    std::cout << error << " | " << file << " (" << line << ")" << std::endl;
  }
  return errorCode;
}
#define gerr() gerror(__FILE__, __LINE__)
#define gerrf() gerrorf_(__FILE__, __LINE__)
void printDebug(const char *mes, float arg) {
  std::cout << mes << " " << arg << std::endl;
}
void printDebug(const char *mes, glm::vec3 arg) {
  std::cout << mes << " x: " << arg.x << "  y: " << arg.y << "  z: " << arg.z
            << std::endl;
}
void printDebug(const char *mes, glm::vec4 arg) {
  std::cout << mes << " x: " << arg.x << "  y: " << arg.y << "  z: " << arg.z
            << "  w:" << arg.w << std::endl;
}
void printDebug(const char *mes, glm::mat3 arg) {
  std::cout << mes << std::endl;
  printDebug("col1: ", arg[0]);
  printDebug("col2: ", arg[1]);
  printDebug("col3: ", arg[2]);
}
void printDebug(const char *mes, glm::mat4 arg) {
  std::cout << mes << std::endl;
  printDebug("col1: ", arg[0]);
  printDebug("col2: ", arg[1]);
  printDebug("col3: ", arg[2]);
  printDebug("col3: ", arg[3]);
}
#endif
