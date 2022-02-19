#include <calcium.hpp>

int main() {
  auto context = cl::CreateContext(cl::Backend::kOpenGL);
  auto window = context->CreateWindow();
  window->SetClearColour({0,0.4,0,1});

  while (window->IsOpen()) {
    cl::PollWindowEvents();
    window->Clear();
    window->SwapBuffers();
  }
}