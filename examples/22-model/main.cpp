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
using namespace myopengl;
class myapp : public opengl_app {
public:
  myapp() {
    shader_ = std::make_shared<shader>("../examples/22-model/model_loading.vs",
                                       "../examples/22-model/model_loading.fs");
    m_ = std::make_shared<model>("../res/nanosuit/nanosuit.obj");
  }
  void before_render() override {}
  void render() override {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_->use();
    update_camera_view(*shader_);
    update_projection(*shader_);

    glm::mat4 model_mat(1.f);
    model_mat = glm::translate(model_mat, glm::vec3(0.0f, -1.75f, 0.0f));
    model_mat = glm::scale(model_mat, glm::vec3(0.2f, 0.2f, 0.2f));
    shader_->set_uniform("model", model_mat);
    m_->draw(*shader_);
  }

private:
  std::shared_ptr<shader> shader_;
  std::shared_ptr<model> m_;
};

int main(/*int argc, char **argv*/) {
  myapp app;
  app.run();

  return 0;
}
