#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <string>
namespace myopengl {
inline std::string to_string(const glm::vec3 &vec) {
  std::stringstream ss;
  ss << "[" << vec.x << "," << vec.y << "," << vec.z << "]";
  return ss.str();
}

} // namespace myopengl
