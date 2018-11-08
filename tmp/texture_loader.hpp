#pragma once
#include "util.hpp"
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <string>
#include <unordered_map>

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
  unsigned int get_texture_from_postion(const glm::vec3 &pos) const;
  std::map<std::string, unsigned int> get_textures() const {
    return hash_texture_;
  }
  glm::vec3 get_pos_from_texture(unsigned int texture) const {
	std::string hash;
  	for(const auto& hash_tex : hash_texture_){
		if(texture == hash_tex.second){
			hash = hash_tex.first;
		}
	}
	if(hash.empty()){
		throw_exception("can not find hash from texture:" + std::to_string(texture));
	}
	std::vector<glm::vec3> poses;
	for(const auto& pos_has : pos_hash_){
		if(hash == pos_has.second){
			poses.push_back(pos_has.first);
		}
	}
	if(poses.size() != 18){
		throw_exception("can not find pos by hash:" + hash + "  texture:" + std::to_string(texture) + " poses size:" + std::to_string(poses.size()));
	}
	return poses[0];
  }
private:
  void load_poses(const std::filesystem::path &dir);

  void load_skybox_texture(const std::filesystem::path &dir);
  std::vector<std::string> get_cube_faces(const std::string &path,
                                          const std::string &hash);

  unsigned int load_cubemap(const std::vector<std::string> &faces);

  glm::vec3 get_pos_from_file(const std::filesystem::path &p);

  std::vector<glm::vec3> all_poses_;
  std::unordered_map<glm::vec3, std::string, vec3_hash> pos_hash_;
  std::map<std::string, unsigned int> hash_texture_;
};
