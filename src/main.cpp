#include <calcium.hpp>

#include "key_bindings.hpp"
#include "world.hpp"


int main() {
  auto context = cl::CreateContext(cl::Backend::kOpenGL);
  cl::WindowCreateInfo window_info;
  window_info.enable_backface_cull = true;
  window_info.front_face = cl::WindingOrder::kCounterClockwise;
  auto window = context->CreateWindow(window_info);

  auto camera = std::make_shared<Camera>();
  camera->CalculateProjection(window->GetAspectRatio());
  window->SetResizeCallback([&](){ camera->CalculateProjection(window->GetAspectRatio()); });
  window->SetKeyPressCallback(KeyBindings::key_inventory, [&](){ window->ToggleCursorLock(); });
  window->SetKeyPressCallback(KeyBindings::key_quit, [&](){ window->Close(); });

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