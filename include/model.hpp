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
  void draw(const ::myopengl::shader &shader_) const {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (size_t i = 0; i < textures_.size(); i++) {
      glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
      // 获取纹理序号（diffuse_textureN 中的 N）
      std::string number;
      std::string name = textures_[i].type;
      if (name == "texture_diffuse")
        number = std::to_string(diffuseNr++);
      else if (name == "texture_specular")
        number = std::to_string(specularNr++);
      try {
        shader_.set_uniform(name + number, static_cast<int>(i));
      } catch (const std::exception &e) {
        std::cerr << "set uniform fail:" << e.what() << std::endl;
      }
      glBindTexture(GL_TEXTURE_2D, textures_[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // 绘制网格
    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

private:
  void setup_mesh() {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);

    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(vertex),
                 vertices_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices_.size() * sizeof(unsigned int), indices_.data(),
                 GL_STATIC_DRAW);
    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (void *)offsetof(vertex, normal));
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (void *)offsetof(vertex, texcoords));
    glBindVertexArray(0);
    auto e_opt = glCheckError();
    if (e_opt) {
      throw_exception(e_opt.value());
    }
  }

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
      m.draw(shader_);
    }
  }

private:
  /*  模型数据  */
  std::vector<mesh> meshes_;
  std::filesystem::path directory_;
  std::set<std::string> textures_loaded_;
  /*  函数   */
  void load_model(const std::filesystem::path &path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(
        path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      throw_exception("assimp::" + import.GetErrorString());
    }
    directory_ = path.parent_path();

    process_node(scene->mRootNode, scene);
  }
  void process_node(aiNode *node, const aiScene *scene) {
    // 处理节点所有的网格（如果有的话）
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      meshes_.push_back(process_mesh(mesh, scene));
    }
    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
      process_node(node->mChildren[i], scene);
    }
  }
  mesh process_mesh(aiMesh *m, const aiScene *scene) {
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<texture> textures;

    for (unsigned int i = 0; i < m->mNumVertices; i++) {
      vertex ver;
      glm::vec3 pos;
      pos.x = m->mVertices[i].x;
      pos.y = m->mVertices[i].y;
      pos.z = m->mVertices[i].z;
      ver.position = pos;
      // 处理顶点位置、法线和纹理坐标
      glm::vec3 normal;
      normal.x = m->mNormals[i].x;
      normal.y = m->mNormals[i].y;
      normal.z = m->mNormals[i].z;
      ver.normal = normal;

      if (m->mTextureCoords[0]) {
        glm::vec2 tex;
        tex.x = m->mTextureCoords[0][i].x;
        tex.y = m->mTextureCoords[0][i].y;
        ver.texcoords = tex;
      } else {
        ver.texcoords = glm::vec2(0.0f, 0.0f);
      }
      vertices.push_back(ver);
    }
    // 处理索引
    for (unsigned int i = 0; i < m->mNumFaces; i++) {
      const auto &face = m->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++)
        indices.push_back(face.mIndices[j]);
    }
    // 处理材质
    aiMaterial *material = scene->mMaterials[m->mMaterialIndex];
    std::vector<texture> diffuseMaps = load_material_textures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<texture> specularMaps = load_material_textures(
        material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    return mesh(vertices, indices, textures);
  }
  std::vector<texture> load_material_textures(aiMaterial *mat,
                                              aiTextureType type,
                                              const std::string &type_name) {
    std::vector<texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
      aiString str;
      mat->GetTexture(type, i, &str);
      if (textures_loaded_.count(str.C_Str()) == 0) {
        texture tex;
        tex.id = texture_from_file(str.C_Str(), directory_);
        tex.type = type_name;
        tex.file_name = str.C_Str();
        textures.push_back(tex);
        textures_loaded_.insert(tex.file_name); // 添加到已加载的纹理中
      }
    }
    return textures;
  }
};
unsigned int texture_from_file(const std::string &file_name,
                               const std::filesystem::path &directory) {
  std::filesystem::path full_path = directory;
  full_path.append(file_name);
  if (!std::filesystem::exists(full_path)) {
    throw_exception(full_path.string() + " not exists");
  }
  unsigned int textureID;
  glGenTextures(1, &textureID);

  cv::Mat img = cv::imread(full_path.string(), cv::IMREAD_UNCHANGED);
  auto width = img.cols;
  auto height = img.rows;
  auto channels = img.channels();
  GLenum format;
  if (channels == 1) {
    format = GL_RED;
  } else if (channels == 3) {
    cv::cvtColor(img, img, CV_BGR2RGB);
    format = GL_RGB;
  } else if (channels == 4) {
    cv::cvtColor(img, img, CV_BGRA2RGBA);
    format = GL_RGBA;
  }

  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, img.data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return textureID;
}
} // namespace myopengl
