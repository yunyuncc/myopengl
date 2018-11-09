#pragma once
#include "camera.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "util.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace myopengl {
void scroll_callback(GLFWwindow *, double /* xoffset*/, double yoffset);
void framebuffer_size_callback(GLFWwindow *, int width, int height);
void mouse_callback(GLFWwindow *, double xpos, double ypos);

camera &get_camera();

class opengl_app {
public:
  opengl_app() { init(); }
  virtual ~opengl_app() = default;

  GLFWwindow *init();

  void update_projection(const myopengl::shader &shader_);
  void update_camera_view(const myopengl::shader &shader_);

  virtual void before_render() = 0;
  virtual void render() = 0;
  void run();

  const size_t screen_width = 1280;
  const size_t screen_height = 720;

private:
  void process_input(GLFWwindow *window);

private:
  float delta_time_ = 0.0f;      // 当前帧与上一帧的时间差
  float last_frame_time_ = 0.0f; // 上一帧的时间
  GLFWwindow *window_ = nullptr;
};

} // namespace myopengl
