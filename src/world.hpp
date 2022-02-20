#pragma once

#include <memory>
#include <vector>

#include <calcium.hpp>

#include "camera.hpp"
#include "chunk.hpp"

class World {
public:
  World(std::shared_ptr<cl::Context>& context);

  void Render(const std::shared_ptr<Camera>& camera);
  Chunk* GetChunkAt(int chunk_x, int chunk_y, int chunk_z);

private:
  std::vector<std::unique_ptr<Chunk>> chunks_;
};
