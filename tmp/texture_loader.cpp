#include "texture_loader.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <set>
using namespace std;
unsigned int
texture_loader::get_texture_from_postion(const glm::vec3 &pos) const {
  std::vector<float> distances;
  for (const auto &p : all_poses_) {
    float dis = glm::distance(p, pos);
    distances.push_back(dis);
  }
  auto it = min_element(distances.begin(), distances.end());
  size_t idx = it - distances.begin();
  glm::vec3 nearest = all_poses_[idx];
  cout << "cur pos :" << myopengl::to_string(pos)
       << "  nearest:" << myopengl::to_string(nearest) << endl;
  std::string hash = pos_hash_.at(nearest);
  if (hash_texture_.count(hash) == 0) {
    throw_exception("hash :" + hash + " not has texture");
  }
  return hash_texture_.at(hash);
}

void texture_loader::load_poses(const std::filesystem::path &dir) {
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

void texture_loader::load_skybox_texture(const std::filesystem::path &dir) {
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

std::vector<std::string>
texture_loader::get_cube_faces(const std::string &path,
                               const std::string &hash) {
  // /home/wyy/Downloads/3d/17DRP5sb8fy/17DRP5sb8fy/matterport_skybox_images
  // 0f37bd0737e349de9d536263a4bdd60d_skybox1_sami.jpg
  std::vector<std::string> faces;
  std::string pre = path + "/";
  for (size_t i = 0; i < 6; i++) {
    std::string name = pre + hash + "_skybox" + std::to_string(i) + "_sami.jpg";
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

unsigned int
texture_loader::load_cubemap(const std::vector<std::string> &faces) {
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

glm::vec3 texture_loader::get_pos_from_file(const std::filesystem::path &p) {
  std::fstream fs(p);
  if (!fs.is_open()) {
    throw_exception("open " + p.string() + " fail");
  }
  glm::vec3 pos;
  for (size_t i = 0; i < 3; i++) {
    std::string line;
    getline(fs, line);
    auto nums = myopengl::split(line, ' ');
    if (nums.size() != 4) {
      throw_exception("should be 4 but is " + std::to_string(nums.size()) +
                      p.string() + " i=" + std::to_string(i) + " line:" + line);
    }
    std::string last = nums.back();
    float num = std::stof(last);
    pos[i] = num;
  }
  return pos;
}
