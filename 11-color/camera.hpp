#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace myopengl {

class camera {
public:
  struct config {
    float yaw{-90.f};
    float pitch{0.f};
    float speed{2.5f};
    float sensitivity{0.1f};
    float zoom{45.f};
    config() = default;
  };
  enum class move { forward = 0, back, left, right };
  camera(const glm::vec3 &pos, const glm::vec3 &world_up, const config &cfg)
      : pos_(pos), world_up_(world_up), cfg_(cfg) {
    update();
  }
  glm::mat4 get_view() const { return glm::lookAt(pos_, pos_ + front_, up_); }
  float get_zoom() const { return cfg_.zoom; }
  glm::vec3 get_pos() const { return pos_; }
  glm::vec3 get_front() const { return front_; }
  void deal_keyboard(move m, float delta_time) {
    float v = cfg_.speed * delta_time;
    if (m == move::forward) {
      pos_ += front_ * v;
    }
    if (m == move::back) {
      pos_ -= front_ * v;
    }
    if (m == move::left) {
      pos_ -= right_ * v;
    }
    if (m == move::right) {
      pos_ += right_ * v;
    }
    update();
  }
  void deal_mouse_move(float xoffset, float yoffset,
                       bool constrain_pitch = true) {
    xoffset *= cfg_.sensitivity;
    yoffset *= cfg_.sensitivity;

    cfg_.yaw += xoffset;
    cfg_.pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch) {
      if (cfg_.pitch > 89.0f)
        cfg_.pitch = 89.0f;
      if (cfg_.pitch < -89.0f)
        cfg_.pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    update();
  }

  void deal_mouse_scroll(float yoffset) {
    if (cfg_.zoom >= 1.0f && cfg_.zoom <= 45.0f)
      cfg_.zoom -= yoffset;
    if (cfg_.zoom <= 1.0f)
      cfg_.zoom = 1.0f;
    if (cfg_.zoom >= 45.0f)
      cfg_.zoom = 45.0f;
  }

private:
  void update() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(cfg_.yaw)) * cos(glm::radians(cfg_.pitch));
    front.y = sin(glm::radians(cfg_.pitch));
    front.z = sin(glm::radians(cfg_.yaw)) * cos(glm::radians(cfg_.pitch));
    front_ = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
  }
  glm::vec3 pos_;
  glm::vec3 up_;
  glm::vec3 right_;
  glm::vec3 front_;
  glm::vec3 world_up_;
  config cfg_{};
};
} // namespace myopengl
