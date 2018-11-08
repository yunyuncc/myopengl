#include "camera.hpp"
#include "model.hpp"
#include "shader.hpp"
#include "util.hpp"
#include "common/string.hpp"
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

const size_t screen_width = 1280;
const size_t screen_height = 720;
// resize window callback
void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  // update user view
  glViewport(0, 0, width, height);
}

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
void update_sky_view(const myopengl::shader &shader_) {
  //  glm::mat4 view = glm::mat4(glm::mat3(get_camera().get_view()));
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
  // init user view
  glViewport(0, 0, screen_width, screen_height);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  return window;
}
std::vector<std::string> cube_faces{
    "../res/skybox/right.jpg", "../res/skybox/left.jpg",
    "../res/skybox/top.jpg",   "../res/skybox/bottom.jpg",
    "../res/skybox/front.jpg", "../res/skybox/back.jpg"};
unsigned int setup_skybuffer() {
  unsigned int VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, skybox_vertices.size() * sizeof(float),
               skybox_vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindVertexArray(0);
  auto e_opt = glCheckError();
  if (e_opt) {
    throw_exception(e_opt.value());
  }
  return VAO;
}
void draw_skybox(const myopengl::shader &sky_shader_, unsigned int sky_texture,
                 unsigned int sky_vao) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when
                          // values are equal to depth buffer's content
  sky_shader_.use();
  sky_shader_.set_uniform("skybox", 0);
  update_sky_view(sky_shader_);
  update_projection(sky_shader_);
  glBindVertexArray(sky_vao);
  glBindTexture(GL_TEXTURE_CUBE_MAP, sky_texture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glDepthFunc(GL_LESS); // set depth function back to default
}
struct vec3_hash {
  size_t operator()(const glm::vec3 &vec) const {
    std::hash<std::string> h;
    return h(myopengl::to_string(vec));
  }
};
class texture_loader {
public:
  explicit texture_loader(const std::filesystem::path &root_path) {
    auto skybox_images_dir = root_path / "matterport_skybox_images";
    auto camera_poses_dir = root_path / "matterport_camera_poses";
    if (!std::filesystem::exists(root_path)) {
      throw_exception(root_path.string() + " not exists");
    }
    load_poses(camera_poses_dir);
    load_skybox_texture(skybox_images_dir);
  }
  unsigned int get_texture_from_postion(const glm::vec3 &pos) {
    vector<float> distances;
    for (const auto &p : all_poses_) {
      float dis = glm::distance(p, pos);
      distances.push_back(dis);
    }
    auto it = min_element(distances.begin(), distances.end());
    size_t idx = it - distances.begin();
    glm::vec3 nearest = all_poses_[idx];
    cout << "cur pos :" << myopengl::to_string(pos)
         << "  nearest:" << myopengl::to_string(nearest) << endl;
    std::string hash = pos_hash_[nearest];
    if (hash_texture_.count(hash) == 0) {
      throw_exception("hash :" + hash + " not has texture");
    }
    return hash_texture_[hash];
  }

private:
  void load_poses(const std::filesystem::path &dir) {
    if (!std::filesystem::exists(dir)) {
      throw_exception(dir.string() + " not exists");
    }
    for (const auto &f : std::filesystem::directory_iterator(dir)) {
      std::filesystem::path full_path = f;
      auto f_name = full_path.filename().string();
      // TODO check end_with .txt
      auto hash = f_name.substr(0, f_name.find_first_of("_"));
      glm::vec3 pos = get_pos_from_file(full_path);
      all_poses_.push_back(pos);
      pos_hash_[pos] = hash;
    }
    cout << "all_poses size = " << all_poses_.size()
         << "  pos_hash size = " << pos_hash_.size() << endl;
  }
  void load_skybox_texture(const std::filesystem::path &dir) {
    if (!std::filesystem::exists(dir)) {
      throw_exception(dir.string() + " not exists");
    }
    std::set<std::string> hashes;
    for (const auto &f : std::filesystem::directory_iterator(dir)) {
      std::filesystem::path full_path = f;
      auto f_name = full_path.filename().string();
      // TODO check end_with .txt
      auto hash = f_name.substr(0, f_name.find_first_of("_"));
      hashes.insert(hash);
    }
    for (auto &h : hashes) {
      auto faces = get_cube_faces(dir.string(), h);
      unsigned int texture_id = load_cubemap(faces);
      hash_texture_[h] = texture_id;
    }
    cout << "has_texture_ size = " << hash_texture_.size() << endl;
  }
  std::vector<std::string> get_cube_faces(const std::string &path,
                                          const std::string &hash) {
    // /home/wyy/Downloads/3d/17DRP5sb8fy/17DRP5sb8fy/matterport_skybox_images
    // 0f37bd0737e349de9d536263a4bdd60d_skybox1_sami.jpg
    std::vector<std::string> faces;
    std::string pre = path + "/";
    for (size_t i = 0; i < 6; i++) {
      std::string name =
          pre + hash + "_skybox" + std::to_string(i) + "_sami.jpg";
      faces.push_back(name);
    }
    std::vector<std::string> order_faces{
        faces[2], // right
        faces[4], // left
        faces[0], // top
        faces[5], // bottom
        faces[1], // front
        faces[3]  // back
    };
    return order_faces;
  }

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

  glm::vec3 get_pos_from_file(const std::filesystem::path &p) {
    std::fstream fs(p);
    if (!fs.is_open()) {
      throw_exception("open " + p.string() + " fail");
    }
    glm::vec3 pos;
    for (size_t i = 0; i < 3; i++) {
      std::string line;
      getline(fs, line);
      auto nums = yunyuncc::split(line, ' ');
      if (nums.size() != 4) {
        throw_exception("should be 4 but is " + std::to_string(nums.size()) +
                        p.string() + " i=" + std::to_string(i) +
                        " line:" + line);
      }
      std::string last = nums.back();
      float num = std::stof(last);
      pos[i] = num;
    }
    return pos;
  }

  std::vector<glm::vec3> all_poses_;
  std::unordered_map<glm::vec3, std::string, vec3_hash> pos_hash_;
  std::map<std::string, unsigned int> hash_texture_;
};

void test() {}

int main(/*int argc, char **argv*/) {
  test();
  auto window = init();
  myopengl::shader model_shader_("../tmp/model_loading.vs",
                                 "../tmp/model_loading.fs");
  myopengl::shader sky_shader_("../tmp/cubemaps.vs", "../tmp/cubemaps.fs");
  // myopengl::model
  // m("/home/wyy/Downloads/3d/17DRP5sb8fy/17DRP5sb8fy/matterport_mesh/bed1a77d92d64f5cbbaaae4feed64ec1/bed1a77d92d64f5cbbaaae4feed64ec1.obj");
  // render loop
  auto sky_vao = setup_skybuffer();
  texture_loader loader("/home/wyy/Downloads/3d/17DRP5sb8fy/17DRP5sb8fy");
  //  auto sky_texture1 = load_cubemap(get_cube_faces(
  //			  "/home/wyy/Downloads/3d/17DRP5sb8fy/17DRP5sb8fy/matterport_skybox_images",
  //			  "30c97842da204e6290ac32904c924e17"));
  //  auto sky_texture2 = load_cubemap(get_cube_faces(
  //			  "/home/wyy/Downloads/3d/17DRP5sb8fy/17DRP5sb8fy/matterport_skybox_images",
  //			  "77a1a11978b04e9cbf74914c98578ab8"));
  //  auto sky_texture = load_cubemap(cube_faces);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    auto pos = get_camera().get_pos();
    unsigned int sky_texture = loader.get_texture_from_postion(pos);

    draw_skybox(sky_shader_, sky_texture, sky_vao);

    // double buffer
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}
