#include "camera.hpp"
#include "model.hpp"
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

std::vector<float> skybox_vertices{
    // positions
    -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
    -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

    1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

using myopengl::get_camera;

// three vertices of a triangle in Normalized Device Coordinates

std::vector<std::string> cube_faces{
    "../res/skybox/right.jpg", "../res/skybox/left.jpg",
    "../res/skybox/top.jpg",   "../res/skybox/bottom.jpg",
    "../res/skybox/front.jpg", "../res/skybox/back.jpg"};

unsigned int load_cubemap(const std::vector<std::string> &faces) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  for (size_t i = 0; i < faces.size(); i++) {
    if (!std::filesystem::exists(faces[i])) {
      throw_exception("file " + faces[i] + " not exists");
    }
    cv::Mat img = cv::imread(faces[i], cv::IMREAD_UNCHANGED);
    auto width = img.cols;
    auto height = img.rows;
    auto channels = img.channels();
    if (channels != 3) {
      throw_exception("img channel should be 3, but is " +
                      std::to_string(channels));
    }
    cv::cvtColor(img, img, CV_BGR2RGB);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  return textureID;
}


class gl_app : public myopengl::opengl_app {
public:
  gl_app() {}
  void update_sky_view(const myopengl::shader &shader_) {
    glm::mat4 view = glm::mat4(glm::mat3(get_camera().get_view()));
    shader_.set_uniform("view", view);
  }

  unsigned int setup_skybuffer() {
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, skybox_vertices.size() * sizeof(float),
                 skybox_vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);

    glBindVertexArray(0);
    auto e_opt = glCheckError();
    if (e_opt) {
      throw_exception(e_opt.value());
    }
    return VAO;
  }

  void before_render() override {
    model_shader_ = std::make_shared<myopengl::shader>(
        "../examples/23-cubemaps/model_loading.vs",
        "../examples/23-cubemaps/model_loading.fs");
    sky_shader_ = std::make_shared<myopengl::shader>(
        "../examples/23-cubemaps/cubemaps.vs",
        "../examples/23-cubemaps/cubemaps.fs");
    m_ = std::make_shared<myopengl::model>("../res/nanosuit/nanosuit.obj");

    sky_vao_ = setup_skybuffer();
    glCheckError();
    sky_texture_ = load_cubemap(cube_faces);
    glCheckError();
  }
  void render() override {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model_shader_->use();
    update_camera_view(*model_shader_);
    update_projection(*model_shader_);

    glm::mat4 model_mat(1.f);
    model_mat = glm::translate(model_mat, glm::vec3(0.0f, -1.75f, 0.0f));
    model_mat = glm::scale(model_mat, glm::vec3(0.2f, 0.2f, 0.2f));
    model_shader_->set_uniform("model", model_mat);
    m_->draw(*model_shader_);

    glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when
                            // values are equal to depth buffer's content
    sky_shader_->use();
    sky_shader_->set_uniform("skybox", 0);
    update_sky_view(*sky_shader_);
    update_projection(*sky_shader_);
    glBindVertexArray(sky_vao_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sky_texture_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS); // set depth function back to default
  }

private:
  std::shared_ptr<myopengl::shader> model_shader_;
  std::shared_ptr<myopengl::shader> sky_shader_;
  std::shared_ptr<myopengl::model> m_;
  unsigned int sky_vao_ = -1;
  unsigned int sky_texture_ = -1;
};
int main(/*int argc, char **argv*/) {
  gl_app app;
  app.run();
  return 0;
}
