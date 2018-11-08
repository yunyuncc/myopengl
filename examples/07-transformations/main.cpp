#include "shader.hpp"
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
// resize window callback
void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  // update user view
  glViewport(0, 0, width, height);
}
// deal esc
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}
// three vertices of a triangle in Normalized Device Coordinates

vector<float> vertices = {
    //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // 右上
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 右下
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // 左上
};
vector<unsigned int> indices = {
    // 注意索引从0开始!
    0, 1, 2, // 第一个三角形
    0, 2, 3};
vector<float> texCoords = {
    0.0f, 0.0f, // 左下角
    1.0f, 0.0f, // 右下角
    0.5f, 1.0f  // 上中
};
void load_img_to_cur_texture(const std::string &img_path) {
  cv::Mat img = cv::imread(img_path);
  cv::Mat img_rgb;
  cv::cvtColor(img, img_rgb, CV_BGR2RGB);
  cv::flip(img_rgb, img_rgb, 0);
  auto width = img.cols;
  auto height = img.rows;
  if (img.channels() != 3) {
    throw std::runtime_error("img should be rgb channels 3");
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, img_rgb.data);
  glGenerateMipmap(GL_TEXTURE_2D);
}
void setup_texture(const std::string &img_path, const std::string &img_path2) {
  unsigned int texture1, texture2;
  glGenTextures(1, &texture1);
  glActiveTexture(
      GL_TEXTURE0); /*active texture unit 0, and load img1 to unit 0*/
  glBindTexture(GL_TEXTURE_2D, texture1);
  load_img_to_cur_texture(img_path);

  glGenTextures(1, &texture2);
  glActiveTexture(
      GL_TEXTURE1); /*active texture unit 1, and load img2 to unit 1*/
  glBindTexture(GL_TEXTURE_2D, texture2);
  load_img_to_cur_texture(img_path2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void setup_transform(const myopengl::shader &shader_) {
  glm::mat4 trans(1.0f);
  // scale first , rotate second
  trans = glm::rotate(trans, glm::radians(90.0f),
                      glm::vec3(0.0, 0.0, 1.0) /*z axis*/);
  trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

  unsigned int transformLoc = glGetUniformLocation(shader_.id(), "transform");
  glUniformMatrix4fv(transformLoc, 1 /*num of matrix*/,
                     GL_FALSE /*do not transpose*/, glm::value_ptr(trans));
}

void do_transform(const myopengl::shader &shader_) {
  glm::mat4 trans(1.0f);
  trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
  trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
  unsigned int transformLoc = glGetUniformLocation(shader_.id(), "transform");
  glUniformMatrix4fv(transformLoc, 1 /*num of matrix*/,
                     GL_FALSE /*do not transpose*/, glm::value_ptr(trans));
}
vector<unsigned int> setup_buffer() {

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  cout << "vao:" << VAO << endl;

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

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  return {EBO};
}

int main(int argc, char **argv) {
  if (argc != 5) {
    cout << "usage:" << argv[0] << " vertex.GLSL fragment.GLSL img1 img2"
         << endl;
    return 0;
  }
  std::string vertex_path(argv[1]);
  std::string fragment_path(argv[2]);
  std::string texture_path(argv[3]);
  std::string texture_path2(argv[4]);
  // init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create window
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // init glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // init user view
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  int nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes
            << std::endl;

  myopengl::shader shader_(vertex_path, fragment_path);
  shader_.use();
  shader_.set_uniform("texture1", 0);
  shader_.set_uniform("texture2", 1);
  auto EBOs = setup_buffer();

  setup_texture(texture_path, texture_path2);

  setup_transform(shader_);
  // render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    // do render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    do_transform(shader_);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // double buffer
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();

  return 0;
}
