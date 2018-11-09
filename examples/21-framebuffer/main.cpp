#include "camera.hpp"
#include "opengl_app.hpp"
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
// three vertices of a triangle in Normalized Device Coordinates
vector<float> cubeVertices = {
    // positions          // texture Coords
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

    -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

vector<float> planeVertices{
    // positions          // texture Coords (note we set these higher than 1
    // (together with GL_REPEAT as texture wrapping mode). this will cause the
    // floor texture to repeat)
    5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, 5.0f,
    0.0f, 0.0f,  -5.0f, -0.5f, -5.0f, 0.0f,  2.0f,

    5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, -5.0f,
    0.0f, 2.0f,  5.0f,  -0.5f, -5.0f, 2.0f,  2.0f};

vector<float> quadVertices{
    // positions   // texCoords
    -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};
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

  // screen quad VAO
  unsigned int quadVAO, quadVBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * quadVertices.size(),
               quadVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  return {cubeVAO, planeVAO, quadVAO};
}

void set_floor_model_and_draw(const myopengl::shader &light_shader_) {
  light_shader_.set_uniform("model", glm::mat4(1.0f));
  glDrawArrays(GL_TRIANGLES, 0, 6);
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

using namespace myopengl;

class myapp : public opengl_app {
public:
  explicit myapp(const std::string &screen_fs_path) {
    shader_ =
        std::make_shared<shader>("../examples/21-framebuffer/depth_testing.vs",
                                 "../examples/21-framebuffer/depth_testing.fs");
    screen_shader_ = std::make_shared<shader>(
        "../examples/21-framebuffer/screen.vs", screen_fs_path);
  }
  void before_render() override {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    VAOs_ = setup_buffer();
    cubeTexture_ = load_texture("../img/container.jpg", GL_TEXTURE0);
    floorTexture_ = load_texture("../img/metal.png", GL_TEXTURE0);
    shader_->use();
    shader_->set_uniform("texture1", 0 /*use texture unit 0*/);
    screen_shader_->use();
    screen_shader_->set_uniform("screenTexture", 0);
    auto [framebuffer, tex_color_buffer, renderbuffer] = setup_framebuffer();
    framebuffer_ = framebuffer;
    tex_color_buffer_ = tex_color_buffer;
    renderbuffer_ = renderbuffer;
  }
  void render() override {
    // bind to framebuffer and draw scene as we normally would to color texture
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_->use();
    update_camera_view(*shader_);
    update_projection(*shader_);

    glBindTexture(GL_TEXTURE_2D, cubeTexture_);
    glBindVertexArray(VAOs_[0]);
    set_cube_model_and_draw(*shader_, 1.0f);
    // draw floar
    shader_->use();
    glBindTexture(GL_TEXTURE_2D, floorTexture_);
    glBindVertexArray(VAOs_[1]);
    set_floor_model_and_draw(*shader_);

    // now bind back to default framebuffer and draw a quad plane with the
    // attached framebuffer color texture
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't
                              // discarded due to depth test.
    // clear all relevant buffers
    glClearColor(
        1.0f, 1.0f, 1.0f,
        1.0f); // set clear color to white (not really necessery actually, since
               // we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);

    screen_shader_->use();
    glBindVertexArray(VAOs_[2]);
    glBindTexture(GL_TEXTURE_2D,
                  tex_color_buffer_); // use the color attachment texture as the
                                      // texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  tuple<unsigned int, unsigned int, unsigned int> setup_framebuffer() {
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // 生成纹理
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // 将它附加到当前绑定的帧缓冲对象
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texColorBuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width,
                          screen_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                << std::endl;
      throw std::runtime_error("framebuffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return {framebuffer, texColorBuffer, rbo};
  }

private:
  std::shared_ptr<shader> shader_;
  std::shared_ptr<shader> screen_shader_;
  std::vector<unsigned int> VAOs_;
  unsigned int cubeTexture_ = -1;
  unsigned int floorTexture_ = -1;
  unsigned int framebuffer_;
  unsigned int tex_color_buffer_;
  unsigned int renderbuffer_;
};

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "usage :" << argv[0] << " screen.fs" << endl;
    return 0;
  }
  std::string screen_fs_path(argv[1]);
  myapp app(screen_fs_path);
  app.run();
  return 0;
}
