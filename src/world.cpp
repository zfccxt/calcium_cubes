#include "world.hpp"

#include <simplex.h>

World::World(std::shared_ptr<cl::Context>& context) {
  simplex_init();

  // TODO: Yeet all this
  int draw_distance = 1;

  for (int x = -draw_distance; x <= draw_distance; ++x) {
    for (int y = -draw_distance; y <= draw_distance; ++y) {
      for (int z = -draw_distance; z <= draw_distance; ++z) {
        chunks_.push_back(std::make_unique<Chunk>(this, x, y, z, context));
      }
    }
  }
}

void World::Render(const std::shared_ptr<Camera>& camera) {
  for (const auto& chunk : chunks_) {
    chunk->Render();
  }
}

Chunk* World::GetChunkAt(int chunk_x, int chunk_y, int chunk_z) {
  for (size_t i = 0; i < chunks_.size(); ++i) {
    Chunk* chunk = chunks_[i].get();
    if (chunk_x == chunk->GetX() && chunk_y == chunk->GetY() && chunk_z == chunk->GetZ()) {
      return chunk;
    }
  }
  return nullptr;
}