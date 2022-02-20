#pragma once

#include <memory>

#include <calcium.hpp>

#include "block.hpp"
#include "chunk_constants.hpp"

class World;

class Chunk {
public:
  Chunk(World* world, int chunk_x, int chunk_y, int chunk_z, std::shared_ptr<cl::Context> context);
  ~Chunk();

  void Render() const;
  void RecreateMesh();

  inline int GetX() const { return chunk_x_; }
  inline int GetY() const { return chunk_y_; }
  inline int GetZ() const { return chunk_z_; }

  inline Block GetBlockAt(int x, int y, int z) const { return blocks_[x + y * ChunkConstants::kChunkSize + z * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize]; };

private:
  void CreateMesh();
  void DestroyMesh();

private:
  World* world_;
  Block* blocks_;
  int chunk_x_, chunk_y_, chunk_z_;

  bool is_loaded_ = false;

  std::shared_ptr<cl::Context> context_;
  std::shared_ptr<cl::Mesh> mesh_;
};
