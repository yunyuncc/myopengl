#pragma once
#include "common/error.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
namespace myopengl {
inline std::string to_string(const glm::vec3 &vec) {
  std::stringstream ss;
  ss << "[" << vec.x << "," << vec.y << "," << vec.z << "]";
  return ss.str();
}

inline std::string to_string(const glm::mat4 &mat) {
  std::stringstream ss;
  for (size_t i = 0; i < 4; i++) {
    char buf[500]{};
    snprintf(buf, sizeof(buf), "|%.4f,%.4f,%.4f,%.4f|\n", mat[i][0], mat[i][1],
             mat[i][2], mat[i][3]);
    ss << buf;
  }
  return ss.str();
}


inline std::optional<std::string> glCheckError_(const char *file, int line) {
  GLenum errorCode;
  std::string error;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    switch (errorCode) {
    case GL_INVALID_ENUM:
      error = "INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error = "INVALID_OPERATION";
      break;
    case GL_STACK_OVERFLOW:
      error = "STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      error = "STACK_UNDERFLOW";
      break;
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "INVALID_FRAMEBUFFER_OPERATION";
      break;
    }
    std::cerr << "openGL error:" << error << " | " << file << " (" << line
              << ")" << std::endl;
  }
  if (error.empty()) {
    return {};
  }
  return error;
}

#define glCheckError() ::myopengl::glCheckError_(__FILE__, __LINE__)
} // namespace myopengl
