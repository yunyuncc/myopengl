#include <vector>
#include <cmath>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;
// resize window callback
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
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
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};
vector<unsigned int> indices = { // 注意索引从0开始! 
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
  glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(),
               GL_STATIC_DRAW); // GL_DYNAMIC_DRAW, GL_STREAM_DRAW

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(float), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3*sizeof(float)));
  glEnableVertexAttribArray(1);
  return {EBO};
}

const char *vertexShaderSource =
    "#version 330 core\n "
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;"
    "void main()\n"
    "{"
    "gl_Position = vec4(aPos, 1.0);"
    "ourColor = aColor;"
    "}";
const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;"
    "void main()"
    "{"
    "	FragColor = vec4(ourColor, 1.0);"
    "}";

unsigned int create_vertex_shader() {
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
    throw std::runtime_error("shader fail");
  } else {
    cout << "create vertex shader success" << endl;
  }
  return vertexShader;
}

unsigned int create_fragment_shader() {
  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  int success;
  char infoLog[512];
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
    throw std::runtime_error("shader fail");
  } else {
    cout << "create fragment shader success" << endl;
  }
  return fragmentShader;
}
unsigned int create_shader_program() {

  auto vertexShader = create_vertex_shader();
  auto fragmentShader = create_fragment_shader();
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  int success;
  char infoLog[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    throw std::runtime_error(std::string(infoLog));
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  cout << "create shader program success" << endl;


  return shaderProgram;
}
int main() {
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
  std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

  auto t1 = std::chrono::high_resolution_clock::now();
  auto shaderProgram = create_shader_program();
  auto EBOs = setup_buffer();
  // render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    // do render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    //glBindVertexArray(VAO);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // double buffer
    glfwSwapBuffers(window);

    glfwPollEvents();
  }
  glfwTerminate();

  return 0;
}
