#include "shader.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <iostream>
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
    // 位置              // 颜色
    0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 右下
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 左下
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // 顶部
};
vector<unsigned int> indices = {
    // 注意索引从0开始!
    0, 1, 2, // 第一个三角形
};
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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  return {EBO};
}

int main(int argc, char **argv) {
  if (argc != 3) {
    cout << "usage:" << argv[0] << " vertex.GLSL fragment.GLSL" << endl;
    return 0;
  }
  std::string vertex_path(argv[1]);
  std::string fragment_path(argv[2]);
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
  auto EBOs = setup_buffer();
  // render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    // do render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader_.use();
    // glBindVertexArray(VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // double buffer
    glfwSwapBuffers(window);

    glfwPollEvents();
  }
  glfwTerminate();

  return 0;
}
