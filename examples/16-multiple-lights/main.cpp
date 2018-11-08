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
#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
const size_t screen_width = 800;
const size_t screen_height = 600;
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

vector<float> vertices = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
    -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
    0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
    1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
    0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
    0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
    -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
    0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
    0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
    -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
    0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
    0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
    -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
    -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
    1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
    -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
    1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

// positions of the point lights
vector<glm::vec3> pointLightPositions{
    glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};
// color of the point lights
vector<glm::vec3> pointLightColors{
    glm::vec3(1.f, 0.0f, 0.0f), glm::vec3(0.f, 1.f, 0.f),
    glm::vec3(0.f, 0.0f, 1.0f), glm::vec3(1.f, 1.0f, 0.0f)};

vector<unsigned int> setup_buffer() {
  // create a Vertex Buffer Object VBO
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  cout << "vbo:" << VBO << endl;
  // bind VBO to gl state machine
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // copy vertices data to VBO
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(),
               GL_STATIC_DRAW); // GL_DYNAMIC_DRAW, GL_STREAM_DRAW

  unsigned int cubeVAO;
  glGenVertexArrays(1, &cubeVAO);
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  unsigned int lightVAO;
  // bind VBO to gl state machine
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // copy vertices data to VBO
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(),
               GL_STATIC_DRAW); // GL_DYNAMIC_DRAW, GL_STREAM_DRAW
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  return {cubeVAO, lightVAO};
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
  // init user view
  glViewport(0, 0, screen_width, screen_height);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  return window;
}
void set_light_model_and_draw(const myopengl::shader &light_shader_) {
  for (size_t i = 0; i < pointLightPositions.size();
       i++) { // const auto& pos : pointLightPositions){
    const auto &pos = pointLightPositions.at(i);
    light_shader_.set_uniform("lightColor", pointLightColors.at(i));
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, glm::vec3(0.2f));
    light_shader_.set_uniform("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
}
void set_cube_model_and_draw(const myopengl::shader &cube_shader_) {
  static glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};
  for (unsigned int i = 0; i < 10; i++) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, cubePositions[i]);
    float angle = 20.0f * i;
    model =
        glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
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

  cv::Mat img = cv::imread(img_path);
  if (img.channels() != 3) {
    throw std::runtime_error("img should be rgb channels 3");
  }
  cv::Mat img_rgb;
  cv::cvtColor(img, img_rgb, CV_BGR2RGB);
  // cv::flip(img_rgb, img_rgb, 0);
  auto width = img.cols;
  auto height = img.rows;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, img_rgb.data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  return textureID;
}

int main(/*int argc, char **argv*/) {
  auto window = init();
  auto VAOs = setup_buffer();
  myopengl::shader lightingShader("../16-multiple-lights/basic_lighting.vs",
                                  "../16-multiple-lights/basic_lighting.fs");
  myopengl::shader lampShader("../16-multiple-lights/lamp.vs",
                              "../16-multiple-lights/lamp.fs");
  // render loop
  load_texture("../img/container2.png", GL_TEXTURE0);
  load_texture("../img/container2_specular.png", GL_TEXTURE1);
  lightingShader.use();
  lightingShader.set_uniform("material.diffuse", 0 /*use texture unit 0*/);
  lightingShader.set_uniform("material.specular", 1 /*use texture unit 1*/);
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);
    // do render
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw cube
    lightingShader.use();
    // directional light
    lightingShader.set_uniform("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.set_uniform("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.set_uniform("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    lightingShader.set_uniform("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    lightingShader.set_uniform("pointLights[0].position",
                               pointLightPositions[0]);
    lightingShader.set_uniform(
        "pointLights[0].ambient", pointLightColors[0].r * 0.05f,
        pointLightColors[0].g * 0.05f, pointLightColors[0].b * 0.05f);
    lightingShader.set_uniform(
        "pointLights[0].diffuse", pointLightColors[0].r * 0.8f,
        pointLightColors[0].g * 0.8f, pointLightColors[0].b * 0.8f);
    lightingShader.set_uniform("pointLights[0].specular", pointLightColors[0]);
    lightingShader.set_uniform("pointLights[0].constant", 1.0f);
    lightingShader.set_uniform("pointLights[0].linear", 0.09f);
    lightingShader.set_uniform("pointLights[0].quadratic", 0.032f);
    // point light 2
    lightingShader.set_uniform("pointLights[1].position",
                               pointLightPositions[1]);
    lightingShader.set_uniform(
        "pointLights[1].ambient", pointLightColors[1].r * 0.05f,
        pointLightColors[1].g * 0.05f, pointLightColors[1].b * 0.05f);
    lightingShader.set_uniform(
        "pointLights[1].diffuse", pointLightColors[1].r * 0.8f,
        pointLightColors[1].g * 0.8f, pointLightColors[1].b * 0.8f);
    lightingShader.set_uniform("pointLights[1].specular", pointLightColors[1]);
    lightingShader.set_uniform("pointLights[1].constant", 1.0f);
    lightingShader.set_uniform("pointLights[1].linear", 0.09f);
    lightingShader.set_uniform("pointLights[1].quadratic", 0.032f);
    // point light 3
    lightingShader.set_uniform("pointLights[2].position",
                               pointLightPositions[2]);
    lightingShader.set_uniform(
        "pointLights[2].ambient", pointLightColors[2].r * 0.05f,
        pointLightColors[2].g * 0.05f, pointLightColors[2].b * 0.05f);
    lightingShader.set_uniform(
        "pointLights[2].diffuse", pointLightColors[2].r * 0.8f,
        pointLightColors[2].g * 0.8f, pointLightColors[2].b * 0.8f);
    lightingShader.set_uniform("pointLights[2].specular", pointLightColors[2]);
    lightingShader.set_uniform("pointLights[2].constant", 1.0f);
    lightingShader.set_uniform("pointLights[2].linear", 0.09f);
    lightingShader.set_uniform("pointLights[2].quadratic", 0.032f);
    // point light 4
    lightingShader.set_uniform("pointLights[3].position",
                               pointLightPositions[3]);
    lightingShader.set_uniform(
        "pointLights[3].ambient", pointLightColors[3].r * 0.05f,
        pointLightColors[3].g * 0.05f, pointLightColors[3].b * 0.05f);
    lightingShader.set_uniform(
        "pointLights[3].diffuse", pointLightColors[3].r * 0.8f,
        pointLightColors[3].g * 0.8f, pointLightColors[3].b * 0.8f);
    lightingShader.set_uniform("pointLights[3].specular", pointLightColors[3]);
    lightingShader.set_uniform("pointLights[3].constant", 1.0f);
    lightingShader.set_uniform("pointLights[3].linear", 0.09f);
    lightingShader.set_uniform("pointLights[3].quadratic", 0.032f);
    // spotLight
    lightingShader.set_uniform("spotLight.position", get_camera().get_pos());
    lightingShader.set_uniform("spotLight.direction", get_camera().get_front());
    lightingShader.set_uniform("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    auto spot_color = glm::vec3(1.f, 0.f, 1.f);
    lightingShader.set_uniform("spotLight.diffuse", spot_color);
    lightingShader.set_uniform("spotLight.specular", spot_color);
    lightingShader.set_uniform("spotLight.constant", 1.0f);
    lightingShader.set_uniform("spotLight.linear", 0.09f);
    lightingShader.set_uniform("spotLight.quadratic", 0.032f);
    lightingShader.set_uniform("spotLight.cutOff",
                               glm::cos(glm::radians(12.5f)));
    // lightingShader.set_uniform("spotLight.outerCutOff",
    // glm::cos(glm::radians(15.0f)));

    lightingShader.set_uniform("material.shininess", 64.0f);
    lightingShader.set_uniform("viewPos", get_camera().get_pos());
    update_camera_view(lightingShader);
    update_projection(lightingShader);
    glBindVertexArray(VAOs[0]);
    set_cube_model_and_draw(lightingShader);
    // draw lighting
    lampShader.use();
    update_camera_view(lampShader);
    update_projection(lampShader);
    glBindVertexArray(VAOs[1]);
    set_light_model_and_draw(lampShader);
    // double buffer
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}
