#include "camera.hpp"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "control_settings.hpp"
#include "key_bindings.hpp"

Camera::Camera(std::shared_ptr<cl::Window>& window) {
  auto recalc_projection = [&](){ CalculateProjection(window->GetAspectRatio()); };
  window->SetResizeCallback(recalc_projection);
  window->SetKeyPressCallback(cl::KeyCode::kE, [&](){ window->ToggleCursorLock(); });
  recalc_projection();
  window->SetControllerDeadzone(0.3f);

  window->SetMouseMoveCallback([&](float dx, float dy){
    if (window->IsCursorLocked()) {
      rot_.x += dx * ControlSettings::camera_rot_speed * 0.03f;
      rot_.y -= dy * ControlSettings::camera_rot_speed * 0.03f;
      flag_recalc_ = true;
    }
  });
}

void Camera::UploadTo(std::shared_ptr<cl::Shader>& shader) {
  if (flag_recalc_) {
    view_ = glm::rotate(glm::mat4(1.0f), rot_.y, glm::vec3(1.0f, 0.0f, 0.0f));
    view_ = glm::rotate(view_, rot_.x, glm::vec3(0.0f, 1.0f, 0.0f));
    view_ = glm::translate(view_, pos_);
    flag_recalc_ = false;
    cached_vpmatrix_ = proj_ * view_;
  }
  shader->UploadUniform("u_viewprojection", &cached_vpmatrix_);
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
  if (window->IsKeyDown(KeyBindings::key_forward)) { --zm; flag_recalc_ = true; }
  if (window->IsKeyDown(KeyBindings::key_back))    { ++zm; flag_recalc_ = true; }
  if (window->IsKeyDown(KeyBindings::key_left))    { --xm; flag_recalc_ = true; }
  if (window->IsKeyDown(KeyBindings::key_right))   { ++xm; flag_recalc_ = true; }

  bool move_pressed = window->IsKeyDown(KeyBindings::key_forward) || window->IsKeyDown(KeyBindings::key_back)
                   || window->IsKeyDown(KeyBindings::key_left)    || window->IsKeyDown(KeyBindings::key_right);

  if (move_pressed) {
    float dd = xm * xm + zm * zm;
    dd = (dd > 0.0f) ? sqrt(dd) : 1.0f;
    xm /= dd;
    zm /= dd;

    pos_.x -= (xm * cos(-rot_.x) + zm * sin(-rot_.x)) * ControlSettings::camera_free_speed;
    pos_.z -= (zm * cos(-rot_.x) - xm * sin(-rot_.x)) * ControlSettings::camera_free_speed;
  }
  else {
    const float lstick_x = window->ControllerLeftStickX();
    const float lstick_y = window->ControllerLeftStickY();
    if (lstick_x != 0.0f || lstick_y != 0.0f) {
      pos_.x -= (lstick_x * cos(-rot_.x) + lstick_y * sin(-rot_.x)) * ControlSettings::camera_free_speed;
      pos_.z -= (lstick_y * cos(-rot_.x) - lstick_x * sin(-rot_.x)) * ControlSettings::camera_free_speed;
      flag_recalc_ = true;
    }
  }
  
  if (window->IsKeyDown(KeyBindings::key_jump)   || window->IsControllerButtonDown(KeyBindings::con_jump))   { pos_.y += ControlSettings::camera_free_speed; flag_recalc_ = true; }
  if (window->IsKeyDown(KeyBindings::key_crouch) || window->IsControllerButtonDown(KeyBindings::con_crouch)) { pos_.y -= ControlSettings::camera_free_speed; flag_recalc_ = true; }

  if (window->IsKeyDown(KeyBindings::key_rot_up))    { rot_.y += ControlSettings::camera_rot_speed; flag_recalc_ = true; }
  if (window->IsKeyDown(KeyBindings::key_rot_down))  { rot_.y -= ControlSettings::camera_rot_speed; flag_recalc_ = true; }
  if (window->IsKeyDown(KeyBindings::key_rot_left))  { rot_.x -= ControlSettings::camera_rot_speed; flag_recalc_ = true; }
  if (window->IsKeyDown(KeyBindings::key_rot_right)) { rot_.x += ControlSettings::camera_rot_speed; flag_recalc_ = true; }

  const float rstick_x = window->ControllerRightStickX();
  const float rstick_y = window->ControllerRightStickY();
  if (rstick_x != 0.0f || rstick_y != 0.0f) {
    rot_.x += rstick_x * ControlSettings::camera_rot_speed;
    rot_.y -= rstick_y * (ControlSettings::invert_y ? -1.0f : 1.0f) * ControlSettings::camera_rot_speed;
    flag_recalc_ = true;
  }

  rot_.y = std::clamp(rot_.y, -1.55f, 1.55f);
}
