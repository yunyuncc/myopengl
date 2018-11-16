#include "opengl_app.hpp"
#include <iostream>
using namespace std;
namespace {

float g_last_x = 400;
float g_last_y = 300;
bool g_first_mouse = true;

} // namespace
namespace myopengl {
void opengl_app::update_projection(const myopengl::shader &shader_) {
  glm::mat4 projection(1.0f);
  projection = glm::perspective(
      glm::radians(get_camera().get_zoom()),
      screen_width / static_cast<float>(screen_height), 0.1f, 100.0f);

  shader_.set_uniform("projection", projection);
}
void opengl_app::update_camera_view(const myopengl::shader &shader_) {
  glm::mat4 view = get_camera().get_view();
  shader_.set_uniform("view", view);
}
void opengl_app::process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    get_camera().deal_keyboard(myopengl::camera::move::forward, delta_time_);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    get_camera().deal_keyboard(myopengl::camera::move::back, delta_time_);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    get_camera().deal_keyboard(myopengl::camera::move::left, delta_time_);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    get_camera().deal_keyboard(myopengl::camera::move::right, delta_time_);
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
   cout << "pos:" << myopengl::to_string(get_camera().get_pos())
        << " yaw:" << get_camera().get_yaw()
        << " pitch:" << get_camera().get_pitch()
        << "  front:" << myopengl::to_string(get_camera().get_front()) <<
        endl;
}

void opengl_app::run() {
  before_render();
  while (!glfwWindowShouldClose(window_)) {
    float current_frame_time = glfwGetTime();
    delta_time_ = current_frame_time - last_frame_time_;
    last_frame_time_ = current_frame_time;
    process_input(window_);
    render();
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
  glfwTerminate();
}

GLFWwindow *opengl_app::init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // create window
  GLFWwindow *window = glfwCreateWindow(screen_width, screen_height,
                                        "LearnOpenGL", nullptr, nullptr);
  if (window == nullptr) {
    throw_exception("fail to create GLFW window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // init glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw_exception("fail to init glad");
  }
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  // init user view
  glViewport(0, 0, screen_width, screen_height);
  // draw model
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  window_ = window;
  return window;
}

camera &get_camera() {
  static glm::vec3 camera_pos(0.f, 0.f, 3.f);
  static glm::vec3 camera_up(0.f, 1.f, 0.f);
  static myopengl::camera::config cfg{.yaw = -90.f,
                                      .pitch = 0.f,
                                      .speed = 2.5f,
                                      .sensitivity = 0.1f,
                                      .zoom = 45.f};
  static myopengl::camera c(camera_pos, camera_up, cfg);
  return c;
}

void scroll_callback(GLFWwindow *, double /* xoffset*/, double yoffset) {
  get_camera().deal_mouse_scroll(yoffset);
}
void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  // update user view
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *, double xpos, double ypos) {
  if (g_first_mouse) // 这个bool变量初始时是设定为true的
  {
    g_last_x = xpos;
    g_last_y = ypos;
    g_first_mouse = false;
  }

  float xoffset = xpos - g_last_x;
  float yoffset =
      -1 *
      (ypos - g_last_y); // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
  g_last_x = xpos;
  g_last_y = ypos;
  get_camera().deal_mouse_move(xoffset, yoffset);
}

} // namespace myopengl
