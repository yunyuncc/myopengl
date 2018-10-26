#pragma once
#include "util.hpp"
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <string>
namespace myopengl {
class shader {
public:
  shader(const std::string &vertex_path, const std::string &fragment_path) {
    try {
      if (!std::filesystem::exists(vertex_path)) {
        throw std::runtime_error(vertex_path + " not exists");
      }
      if (!std::filesystem::exists(fragment_path)) {
        throw std::runtime_error(fragment_path + " not exists");
      }
      std::ifstream vertex_file(vertex_path);
      std::ifstream fragment_file(fragment_path);
      std::stringstream ss_ver;
      std::stringstream ss_frag;
      ss_ver << vertex_file.rdbuf();
      ss_frag << fragment_file.rdbuf();
      vertex_code_ = ss_ver.str();
      fragment_code_ = ss_frag.str();
      create_shader();

    } catch (const std::exception &e) {
      throw std::runtime_error(
          std::string("got a exception when create shader:") + e.what());
    }
  }
  ~shader() {
    //  	glDeleteProgram(id_);
  }
  unsigned int id() const { return id_; }
  void use() const {
    glUseProgram(id_);
    auto error_opt = glCheckError();
    if (error_opt) {
      throw std::runtime_error(std::string("use shader fail:") +
                               error_opt.value());
    }
  }
  void set_uniform(const std::string &name, bool value) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw std::runtime_error(std::string("has not uniform name:") + name);
    }
    glUniform1i(loc, (int)value);
    glCheckError();
  }
  void set_uniform(const std::string &name, int value) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw std::runtime_error(std::string("has not uniform name:") + name);
    }
    glUniform1i(loc, value);
    glCheckError();
  }
  void set_uniform(const std::string &name, float value) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw std::runtime_error(std::string("has not uniform name:") + name);
    }
    glUniform1f(loc, value);
    glCheckError();
  }
  void set_uniform(const std::string &name, const glm::mat4 &mat) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw std::runtime_error(std::string("has not uniform name:") + name);
    }
    glUniformMatrix4fv(loc, 1 /*num of matrix*/, GL_FALSE /*do not transpose*/,
                       glm::value_ptr(mat));
    glCheckError();
  }
  void set_uniform(const std::string &name, const glm::vec3 &vec) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw std::runtime_error(std::string("has not uniform name:") + name);
    }
    glUniform3fv(loc, 1, &vec[0]);
    glCheckError();
  }
  void set_uniform(const std::string &name, float x, float y, float z) const {
    int loc = glGetUniformLocation(id_, name.c_str());
    if (loc == -1) {
      throw std::runtime_error(std::string("has not uniform name:") + name);
    }
    glUniform3f(loc, x, y, z);
    glCheckError();
  }

private:
  void create_shader() {
    int success{};
    char infoLog[512]{};
    const char *vShaderCode = vertex_code_.c_str();
    const char *fShaderCode = fragment_code_.c_str();
    // 顶点着色器
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    // 打印编译错误（如果有的话）
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
      throw std::runtime_error(std::string("compilation vertex fail:") +
                               std::string(infoLog));
    };

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
      throw std::runtime_error(std::string("compilation fragment fail:") +
                               std::string(infoLog));
    };

    // 着色器程序
    id_ = glCreateProgram();
    if (id_ == 0) {
      throw std::runtime_error("glCreateProgram fail");
    }

    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    glLinkProgram(id_);
    // 打印连接错误（如果有的话）
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(id_, 512, nullptr, infoLog);
      throw std::runtime_error(std::string("link shader fail:") +
                               std::string(infoLog));
    }

    // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }
  unsigned int id_;
  std::string vertex_code_;
  std::string fragment_code_;
};

} // namespace myopengl
