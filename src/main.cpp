#include <calcium.hpp>

#include "key_bindings.hpp"
#include "world.hpp"

std::shared_ptr<cl::Window> window;
auto camera = std::make_shared<Camera>();

int main() {
  auto context = cl::CreateContext(cl::Backend::kOpenGL);
  window = context->CreateWindow();
  camera->CalculateProjection(window->GetAspectRatio());
  window->SetResizeCallback([](){ camera->CalculateProjection(window->GetAspectRatio()); });
  window->SetKeyPressCallback(KeyBindings::key_inventory, [](){ window->ToggleCursorLock(); });

  auto chunk_shader = context->CreateShader("res/shaders/chunk_shader.vert.spv", "res/shaders/chunk_shader.frag.spv");

  cl::TextureCreateInfo texture_info;
  texture_info.file_path = "res/textures/block_texture_atlas.png";
  texture_info.filter = cl::TextureFilter::kNearest;
  auto block_texture = context->CreateTexture(texture_info);

  World world(context);

  chunk_shader->BindTexture("u_texture", block_texture);
  while (window->IsOpen()) {
    window->PollEvents();
    context->BeginRenderPass(chunk_shader);

    camera->FreeControl(window);
    camera->UploadTo(chunk_shader);

    world.Render(camera);

    context->EndRenderPass();
  }
}