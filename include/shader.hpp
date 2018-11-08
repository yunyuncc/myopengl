#pragma once
#include "util.hpp"
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
namespace myopengl {
class shader {
public:
  shader(const std::string &vertex_path, const std::string &fragment_path);
  ~shader() {
    //  	glDeleteProgram(id_);
  }
  unsigned int id() const { return id_; }
  void use() const;
  void set_uniform(const std::string &name, bool value) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw_exception(std::string("has not uniform name:") + name);
    }
    glUniform1i(loc, (int)value);
    glCheckError();
  }
  void set_uniform(const std::string &name, int value) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw_exception(std::string("has not uniform name:") + name);
    }
    glUniform1i(loc, value);
    glCheckError();
  }
  void set_uniform(const std::string &name, float value) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw_exception(std::string("has not uniform name:") + name);
    }
    glUniform1f(loc, value);
    glCheckError();
  }
  void set_uniform(const std::string &name, const glm::mat4 &mat) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw_exception(std::string("has not uniform name:") + name);
    }
    glUniformMatrix4fv(loc, 1 /*num of matrix*/, GL_FALSE /*do not transpose*/,
                       glm::value_ptr(mat));
    glCheckError();
  }
  void set_uniform(const std::string &name, const glm::vec3 &vec) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw_exception(std::string("has not uniform name:") + name);
    }
    glUniform3fv(loc, 1, &vec[0]);
    glCheckError();
  }
  void set_uniform(const std::string &name, float x, float y, float z) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw_exception(std::string("has not uniform name:") + name);
    }
    glUniform3f(loc, x, y, z);
    glCheckError();
  }

private:
  void create_shader();
  unsigned int id_;
  std::string vertex_code_;
  std::string fragment_code_;
};

} // namespace myopengl
