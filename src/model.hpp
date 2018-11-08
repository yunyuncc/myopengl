#pragma once
#include "shader.hpp"
#include "util.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <opencv2/opencv.hpp>
#include <set>
#include <string>
namespace myopengl {

struct vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoords;
};

struct texture {
  unsigned int id;
  std::string type;
  std::string file_name;
};
unsigned int texture_from_file(const std::string &file_name,
                               const std::filesystem::path &directory);
class mesh {
public:
  mesh(const std::vector<vertex> &vertices,
       const std::vector<unsigned int> &indices,
       const std::vector<texture> &textures)
      : vertices_(vertices), indices_(indices), textures_(textures) {
    setup_mesh();
  }
  void draw(const ::myopengl::shader &shader_) const;
  std::vector<texture> get_textures() const { return textures_; }
  std::vector<vertex> get_vertices() const { return vertices_; }
  std::vector<unsigned int> get_indices() const { return indices_; }

private:
  void setup_mesh();

  /*  渲染数据  */
  unsigned int VAO_, VBO_, EBO_;
  /*  网格数据  */
  std::vector<vertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<texture> textures_;
};

class model {
public:
  /*  函数   */
  model(const std::filesystem::path &path) { load_model(path); }
  void draw(const ::myopengl::shader &shader_) const {
    for (const auto &m : meshes_) {
      m->draw(shader_);
    }
  }
  std::vector<std::shared_ptr<mesh>> get_meshes() { return meshes_; }

private:
  /*  模型数据  */
  std::vector<std::shared_ptr<mesh>> meshes_;
  std::filesystem::path directory_;
  std::set<std::string> textures_loaded_;
  /*  函数   */
  void load_model(const std::filesystem::path &path);
  void process_node(aiNode *node, const aiScene *scene);
  std::shared_ptr<mesh> process_mesh(aiMesh *m, const aiScene *scene);

  std::vector<texture> load_material_textures(aiMaterial *mat,
                                              aiTextureType type,
                                              const std::string &type_name);
};
unsigned int texture_from_file(const std::string &file_name,
                               const std::filesystem::path &directory);
} // namespace myopengl
