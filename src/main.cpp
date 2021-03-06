#include <chrono>

#include <calcium.hpp>

#include "key_bindings.hpp"
#include "world.hpp"

int main() {
  auto context = cl::Context::CreateContext(cl::Backend::kOpenGL);

  cl::WindowCreateInfo window_info;
  window_info.clear_colour = 0x87ceebff;
  window_info.enable_backface_cull = true;
  window_info.front_face = cl::WindingOrder::kCounterClockwise;
  window_info.enable_vsync = true;
  auto window = context->CreateWindow(window_info);

  auto camera = std::make_shared<Camera>(window);
  camera->CalculateProjection(window->GetAspectRatio());
  window->SetResizeCallback([&](){ camera->CalculateProjection(window->GetAspectRatio()); });
  window->SetKeyPressCallback(KeyBindings::key_inventory, [&](){ window->ToggleCursorLock(); });
  window->SetKeyPressCallback(KeyBindings::key_quit, [&](){ window->Close(); });

  auto chunk_shader = context->CreateShader("res/shaders/chunk_shader.vert.spv", "res/shaders/chunk_shader.frag.spv");

  cl::TextureArrayCreateInfo texture_array_info;
  texture_array_info.AddFile("res/textures/dirt.png");          //  0
  texture_array_info.AddFile("res/textures/grass.png");         //  1
  texture_array_info.AddFile("res/textures/grass_side.png");    //  2
  texture_array_info.AddFile("res/textures/log.png");           //  3
  texture_array_info.AddFile("res/textures/log_side.png");      //  4
  texture_array_info.AddFile("res/textures/leaves_1.png");      //  5
  texture_array_info.AddFile("res/textures/leaves_2.png");      //  6
  texture_array_info.AddFile("res/textures/leaves_3.png");      //  7
  texture_array_info.AddFile("res/textures/leaves_opaque.png"); //  8
  texture_array_info.AddFile("res/textures/basalt.png");        //  9
  texture_array_info.AddFile("res/textures/andesite.png");      // 10
  texture_array_info.AddFile("res/textures/limestone.png");     // 11
  texture_array_info.AddFile("res/textures/rhyolite.png");      // 12
  texture_array_info.filter = cl::TextureFilter::kNearest;
  auto block_texture_array = context->CreateTextureArray(texture_array_info);

  World world(context);

  auto start_time = std::chrono::high_resolution_clock::now();

  chunk_shader->BindTextureArray("u_block_texture_array", block_texture_array);
  while (window->IsOpen()) {
    window->PollEvents();

    camera->FreeControl(window);
    camera->UploadTo(chunk_shader);

    context->BeginFrame();

    chunk_shader->Bind();
    world.Render(camera);

    context->EndFrame();
  }
}
