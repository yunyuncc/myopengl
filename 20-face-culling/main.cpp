#include "camera.hpp"
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

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

vector<float> cubeVertices = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left  	
};

vector<float> planeVertices{
    // positions          // texture Coords (note we set these higher than 1
    // (together with GL_REPEAT as texture wrapping mode). this will cause the
    // floor texture to repeat)
    5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, 5.0f,
    0.0f, 0.0f,  -5.0f, -0.5f, -5.0f, 0.0f,  2.0f,

    5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, -5.0f,
    0.0f, 2.0f,  5.0f,  -0.5f, -5.0f, 2.0f,  2.0f};

vector<float> transparentVertices{
    // positions         // texture Coords (swapped y coordinates because
    // texture is flipped upside down)
    0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -0.5f, 0.0f,
    0.0f, 1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

    0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 1.0f, -0.5f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f};
vector<glm::vec3> vegetation{
    glm::vec3(-1.5f, 0.0f, -0.48f), glm::vec3(1.5f, 0.0f, 0.51f),
    glm::vec3(0.0f, 0.0f, 0.7f), glm::vec3(-0.3f, 0.0f, -2.3f),
    glm::vec3(0.5f, 0.0f, -0.6f)};

vector<unsigned int> setup_buffer() {
  // cube VAO
  unsigned int cubeVAO, cubeVBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cubeVertices.size(),
               cubeVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);
  // plane VAO
  unsigned int planeVAO, planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planeVertices.size(),
               planeVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);

  // transparent VAO
  unsigned int transparentVAO, transparentVBO;
  glGenVertexArrays(1, &transparentVAO);
  glGenBuffers(1, &transparentVBO);
  glBindVertexArray(transparentVAO);
  glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * transparentVertices.size(),
               transparentVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);
  return {cubeVAO, planeVAO, transparentVAO};
}

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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  //glCullFace(GL_FRONT);
  // init user view
  glViewport(0, 0, screen_width, screen_height);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  return window;
}

void set_floor_model_and_draw(const myopengl::shader &light_shader_) {
  light_shader_.set_uniform("model", glm::mat4(1.0f));
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void set_grass_model_and_draw(const myopengl::shader &light_shader_) {
  std::map<float, glm::vec3, std::greater<float>> windows;
  for (const auto &win : vegetation) {
    float distance = glm::length(get_camera().get_pos() - win);
    windows[distance] = win;
  }
  for (const auto &win : windows) {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, win.second);
    light_shader_.set_uniform("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}

void set_cube_model_and_draw(const myopengl::shader &cube_shader_,
                             float scale) {
  static vector<glm::vec3> cubePositions{glm::vec3(-1.0f, 0.0f, -1.0f),
                                         glm::vec3(2.0f, 0.0f, 0.0f)};
  for (size_t i = 0; i < cubePositions.size(); i++) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, cubePositions[i]);
    if (scale != 1.0f) {
      model = glm::scale(model, glm::vec3(scale, scale, scale));
    }
    cube_shader_.set_uniform("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
}

unsigned int load_texture(const std::string &img_path, GLenum texture_unit) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glActiveTexture(
      texture_unit); /*active texture unit , and load img1 to that unit*/
  glBindTexture(GL_TEXTURE_2D, textureID);

  cv::Mat img = cv::imread(img_path, cv::IMREAD_UNCHANGED);
  if (img.channels() == 3) {
    cv::Mat img_rgb;
    cv::cvtColor(img, img_rgb, CV_BGR2RGB);
    auto width = img.cols;
    auto height = img.rows;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, img_rgb.data);
  } else if (img.channels() == 4) {
    auto width = img.cols;
    auto height = img.rows;
    cv::cvtColor(img, img, CV_BGRA2RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, img.data);

  } else {
    std::runtime_error(string("img channel is " + to_string(img.channels())));
  }
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  if (img.channels() == 4) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  return textureID;
}

int main(/*int argc, char **argv*/) {
  auto window = init();
  auto VAOs = setup_buffer();
  myopengl::shader shader_("../20-face-culling/depth_testing.vs",
                           "../20-face-culling/depth_testing.fs");
  // render loop

  auto cubeTexture = load_texture("../img/marble.jpg", GL_TEXTURE0);
  auto floorTexture = load_texture("../img/metal.png", GL_TEXTURE0);
  // auto grassTexture = load_texture("../img/grass.png", GL_TEXTURE0);
  auto windowTexture = load_texture("../img/window.png", GL_TEXTURE0);
  shader_.use();
  shader_.set_uniform("texture1", 0 /*use texture unit 0*/);
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    shader_.use();
    update_camera_view(shader_);
    update_projection(shader_);

    // do render
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    // draw floar
    shader_.use();
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glBindVertexArray(VAOs[1]);
    set_floor_model_and_draw(shader_);

    glEnable(GL_CULL_FACE);
    // draw cube
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glBindVertexArray(VAOs[0]);
    set_cube_model_and_draw(shader_, 1.0f);

    glDisable(GL_CULL_FACE);
    // draw grass
    glBindTexture(GL_TEXTURE_2D, windowTexture);
    glBindVertexArray(VAOs[2]);
    set_grass_model_and_draw(shader_);

    // double buffer
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}
