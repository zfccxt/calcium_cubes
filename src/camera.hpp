#pragma once

#include <memory>

#include <calcium.hpp>
#include <glm/glm.hpp>

class Camera {
public:
  void UploadTo(std::shared_ptr<cl::Shader>& shader);
  void CalculateProjection(float aspect_ratio);

  void SetPosition(const glm::vec3& pos);
  void SetPosition(float x, float y, float z);

  void SetRotation(const glm::vec3& rot);
  void SetRotation(float x, float y, float z);

  void FreeControl(std::shared_ptr<cl::Window>& window);

private:
  bool flag_recalc_ = true;
  
  glm::vec3 pos_ = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 rot_ = glm::vec3(0.0f, 0.0f, 0.0f);

  glm::mat4 proj_= glm::mat4(1.0f);
  glm::mat4 view_= glm::mat4(1.0f);
};
