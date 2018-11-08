#include <vector>
#include <cmath>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
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
    0.5f, 0.5f, 0.0f,   // 右上角
    0.5f, -0.5f, 0.0f,  // 右下角
    -0.5f, -0.5f, 0.0f, // 左下角
    -0.5f, 0.5f, 0.0f   // 左上角
};
vector<unsigned int> indices = { // 注意索引从0开始! 
    0, 1, 3, // 第一个三角形
    1, 2, 3  // 第二个三角形
};
vector<unsigned int> indices2 = { // 注意索引从0开始! 
    0, 1, 3 // 第一个三角形
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

  unsigned int EBO2;
  glGenBuffers(1, &EBO2);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(float), indices2.data(), GL_STATIC_DRAW);


  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  return {EBO, EBO2};
}

const char *vertexShaderSource =
    "#version 330 core\n "
    "layout (location = 0) in vec3 aPos;\n"
    "out vec4 vertexColor;"
    "void main()\n"
    "{"
    "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
    "vertexColor = vec4(0.5,0,0,1.0);"
    "}";
const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;"
    "void main()"
    "{"
    "	FragColor = ourColor;"
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
void change_color(unsigned int shaderProgram){
  float timeValue = glfwGetTime();
  float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
  int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
  if(vertexColorLocation == -1){
  	throw std::runtime_error("can not find uniform ourColor");
  }
  glUseProgram(shaderProgram);
  glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
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
    change_color(shaderProgram); 
    if(dur % 3 == 0){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    }else if(dur % 3 == 1){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    }else{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    }
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // double buffer
    glfwSwapBuffers(window);

    glfwPollEvents();
  }
  glfwTerminate();

  return 0;
}
