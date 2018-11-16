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

	0.0, 0.0, 0.0,
	0.5, 0.0, 0.0,
	0.0, 0.5, 0.0,
	0.0, 0.0, 0.5
};

using myopengl::get_camera;

// three vertices of a triangle in Normalized Device Coordinates




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
        "../examples/24-advanced_GLSL/model_loading.vs",
        "../examples/24-advanced_GLSL/model_loading.fs");
    sky_shader_ = std::make_shared<myopengl::shader>(
        "../examples/24-advanced_GLSL/cubemaps.vs",
        "../examples/24-advanced_GLSL/cubemaps.fs");

    glEnable(GL_PROGRAM_POINT_SIZE);
    sky_vao_ = setup_skybuffer();



//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glCheckError();
  }
  void render() override {
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model_shader_->use();
    update_camera_view(*model_shader_);
    update_projection(*model_shader_);

    glm::mat4 model_mat(1.f);
    model_shader_->set_uniform("model", model_mat);
    glBindVertexArray(sky_vao_);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_POINTS, 0, 4);
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
