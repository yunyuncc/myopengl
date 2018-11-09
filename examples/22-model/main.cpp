#include "camera.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "util.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
const size_t screen_width = 1280;
const size_t screen_height = 720;
// resize window callback
void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  // update user view
  glViewport(0, 0, width, height);
}

myopengl::camera &get_camera() {
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
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float lastX = 400, lastY = 300;

bool firstMouse = true;
void mouse_callback(GLFWwindow *, double xpos, double ypos) {

  if (firstMouse) // 这个bool变量初始时是设定为true的
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      -1 *
      (ypos - lastY); // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
  lastX = xpos;
  lastY = ypos;
  get_camera().deal_mouse_move(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *, double /* xoffset*/, double yoffset) {
  get_camera().deal_mouse_scroll(yoffset);
}

// deal esc
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    get_camera().deal_keyboard(myopengl::camera::move::forward, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    get_camera().deal_keyboard(myopengl::camera::move::back, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    get_camera().deal_keyboard(myopengl::camera::move::left, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    get_camera().deal_keyboard(myopengl::camera::move::right, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    cout << "pos:" << myopengl::to_string(get_camera().get_pos())
         << " yaw:" << get_camera().get_yaw()
         << " pitch:" << get_camera().get_pitch()
         << "  front:" << myopengl::to_string(get_camera().get_front()) << endl;
}
// three vertices of a triangle in Normalized Device Coordinates

void update_projection(const myopengl::shader &shader_) {
  glm::mat4 projection(1.0f);
  projection = glm::perspective(
      glm::radians(get_camera().get_zoom()),
      screen_width / static_cast<float>(screen_height), 0.1f, 100.0f);

  shader_.set_uniform("projection", projection);
}
void update_camera_view(const myopengl::shader &shader_) {
  glm::mat4 view = get_camera().get_view();
  shader_.set_uniform("view", view);
}

GLFWwindow *init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // create window
  GLFWwindow *window = glfwCreateWindow(screen_width, screen_height,
                                        "LearnOpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }
  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  // init glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
  }
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_FRONT);
  // init user view
  glViewport(0, 0, screen_width, screen_height);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  return window;
}

int main(/*int argc, char **argv*/) {
  auto window = init();
  myopengl::shader shader_("../examples/22-model/model_loading.vs",
                           "../examples/22-model/model_loading.fs");
  myopengl::model m("../res/nanosuit/nanosuit.obj");
  // render loop

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_.use();
    update_camera_view(shader_);
    update_projection(shader_);

    glm::mat4 model_mat(1.f);
    model_mat = glm::translate(model_mat, glm::vec3(0.0f, -1.75f, 0.0f));
    model_mat = glm::scale(model_mat, glm::vec3(0.2f, 0.2f, 0.2f));
    shader_.set_uniform("model", model_mat);
    m.draw(shader_);

    // double buffer
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}
