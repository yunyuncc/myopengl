#include "shader.hpp"

namespace myopengl {

void shader::create_shader() {
  int success{};
  char infoLog[512]{};
  const char *info = infoLog;
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
    std::cerr << "[" << infoLog << "]" << std::endl;
    throw_exception(std::string("compilation vertex fail:") +
                    std::string(info));
  };

  unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, nullptr);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
    std::cerr << "[" << infoLog << "]" << std::endl;
    throw_exception(std::string("compilation fragment fail:") +
                    std::string(infoLog));
  };

  // 着色器程序
  id_ = glCreateProgram();
  if (id_ == 0) {
    throw_exception("glCreateProgram fail");
  }

  glAttachShader(id_, vertex);
  glAttachShader(id_, fragment);
  glLinkProgram(id_);
  // 打印连接错误（如果有的话）
  glGetProgramiv(id_, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id_, 512, nullptr, infoLog);
    std::cerr << "[" << infoLog << "]" << std::endl;
    throw_exception(std::string("link shader fail:") + std::string(infoLog));
  }

  // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void shader::use() const {
  glUseProgram(id_);
  auto error_opt = glCheckError();
  if (error_opt) {
    throw_exception(std::string("use shader fail:") + error_opt.value());
  }
}

shader::shader(const std::string &vertex_path,
               const std::string &fragment_path) {
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
    throw_exception(std::string("got a exception when create shader:") +
                    e.what());
  }
}

} // namespace myopengl
