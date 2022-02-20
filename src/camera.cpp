#include "camera.hpp"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "control_settings.hpp"
#include "key_bindings.hpp"

void Camera::UploadTo(std::shared_ptr<cl::Shader>& shader) {
  if (flag_recalc_) {
    view_ = glm::rotate(glm::mat4(1.0f), rot_.y, glm::vec3(1.0f, 0.0f, 0.0f));
    view_ = glm::rotate(view_, rot_.x, glm::vec3(0.0f, 1.0f, 0.0f));
    view_ = glm::translate(view_, pos_);
    glm::mat4 vpmatrix = proj_ * view_;
    shader->UploadUniform("u_viewprojection", &vpmatrix);
    flag_recalc_ = false;
  }
}

void Camera::CalculateProjection(float aspect_ratio) {
  proj_ = glm::perspective(glm::radians(ControlSettings::camera_fov), aspect_ratio, 0.1f, 1000.0f);
  flag_recalc_ = true;
}

void Camera::SetPosition(const glm::vec3& pos) {
  pos_ = pos;
  flag_recalc_ = true;
}

void Camera::SetPosition(float x, float y, float z) {
  SetPosition(glm::vec3(x, y, z));
}

void Camera::SetRotation(const glm::vec3& rot) {
  rot_ = rot;
  flag_recalc_ = true;
}

void Camera::SetRotation(float x, float y, float z) {
  SetRotation(glm::vec3(x, y, z));
}

void Camera::FreeControl(std::shared_ptr<cl::Window>& window) {
  float xm = 0.0f;
  float zm = 0.0f;
  if (window->IsKeyDown(KeyBindings::key_forward)) { --zm; }
  if (window->IsKeyDown(KeyBindings::key_back))    { ++zm; }
  if (window->IsKeyDown(KeyBindings::key_left))    { --xm; }
  if (window->IsKeyDown(KeyBindings::key_right))   { ++xm; }

  float dd = xm * xm + zm * zm;
  dd = (dd > 0.0f) ? sqrt(dd) : 1.0f;
  xm /= dd;
  zm /= dd;

  pos_.x -= (xm * cos(-rot_.x) + zm * sin(-rot_.x)) * ControlSettings::camera_free_speed;
  pos_.z -= (zm * cos(-rot_.x) - xm * sin(-rot_.x)) * ControlSettings::camera_free_speed;

  if (window->IsKeyDown(KeyBindings::key_jump))   { pos_.y += ControlSettings::camera_free_speed; }
  if (window->IsKeyDown(KeyBindings::key_crouch)) { pos_.y -= ControlSettings::camera_free_speed; }

  if (window->IsKeyDown(KeyBindings::key_rot_up))    { rot_.y += ControlSettings::camera_rot_speed; }
  if (window->IsKeyDown(KeyBindings::key_rot_down))  { rot_.y -= ControlSettings::camera_rot_speed; }
  if (window->IsKeyDown(KeyBindings::key_rot_left))  { rot_.x -= ControlSettings::camera_rot_speed; }
  if (window->IsKeyDown(KeyBindings::key_rot_right)) { rot_.x += ControlSettings::camera_rot_speed; }

  if (window->IsCursorLocked()) {
    rot_.x += window->CursorDeltaX() * ControlSettings::camera_rot_speed * 0.03f;
    rot_.y -= window->CursorDeltaY() * ControlSettings::camera_rot_speed * 0.03f;
  }

  rot_.y = std::clamp(rot_.y, -1.55f, 1.55f);

  flag_recalc_ = true;
}