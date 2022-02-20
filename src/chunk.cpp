#include "chunk.hpp"

#include <cstdint>
#include <vector>

#include "chunk_generator.hpp"
#include "world.hpp"

const size_t kVertexSize         = 6;
const size_t kNumIndicesPerFace  = 6;
const float  kTextureElementSize = 1.0f / 16.0f;

const float kLightTop    = 0.8f;
const float kLightNorth  = 0.7f;
const float kLightSouth  = 0.6f;
const float kLightEast   = 0.5f;
const float kLightWest   = 0.4f;
const float kLightBottom = 0.3f;

Chunk::Chunk(World* world, int chunk_x, int chunk_y, int chunk_z, std::shared_ptr<cl::Context> context) 
    : world_(world), chunk_x_(chunk_x), chunk_y_(chunk_y), chunk_z_(chunk_z), context_(context) {
  // TODO: If chunk previously generated, load it from map file

  // Else, generate the chunk
  blocks_ = ChunkGenerator::GenerateChunk(world, chunk_x, chunk_y, chunk_z);
  // TODO: Add chunk to map file

  CreateMesh();
}

Chunk::~Chunk() {
  DestroyMesh();

  delete[] blocks_;
}

void Chunk::CreateMesh() {
  std::vector<float> vertices(ChunkConstants::kChunkSize * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize * 6 * 4 * kVertexSize);
  std::vector<uint32_t> indices(ChunkConstants::kChunkSize * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize * 6 * kNumIndicesPerFace);

  size_t current_vertex = 0;
  size_t current_index = 0;
  uint16_t num_faces = 0;

  Chunk* north_chunk = world_->GetChunkAt(chunk_x_, chunk_y_, chunk_z_ - 1);
  Chunk* south_chunk = world_->GetChunkAt(chunk_x_, chunk_y_, chunk_z_ + 1);
  Chunk* east_chunk  = world_->GetChunkAt(chunk_x_ - 1, chunk_y_, chunk_z_);
  Chunk* west_chunk  = world_->GetChunkAt(chunk_x_ + 1, chunk_y_, chunk_z_);
  Chunk* up_chunk    = world_->GetChunkAt(chunk_x_, chunk_y_ - 1, chunk_z_);
  Chunk* down_chunk  = world_->GetChunkAt(chunk_x_, chunk_y_ + 1, chunk_z_);

  for (int x = 0; x < ChunkConstants::kChunkSize; ++x) {
    for (int y = 0; y < ChunkConstants::kChunkSize; ++y) {
      for (int z = 0; z < ChunkConstants::kChunkSize; ++z) {
        Block b = blocks_[x + y * ChunkConstants::kChunkSize + z * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize];
        if (b == Block::kAir || b == Block::kUndefined) {
          continue;
        }
     
        // TODO: Fix this
        Block south_block  = z < ChunkConstants::kChunkSize - 1 ? blocks_[x + y *       ChunkConstants::kChunkSize + (z + 1) * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : south_chunk ? south_chunk->GetBlockAt(x, y, ChunkConstants::kChunkSize - 1) : Block::kUndefined;
        Block north_block  = z > 0 ?                              blocks_[x + y *       ChunkConstants::kChunkSize + (z - 1) * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : north_chunk ? north_chunk->GetBlockAt(x, y, 0)                              : Block::kUndefined;
        Block west_block   = x < ChunkConstants::kChunkSize - 1 ? blocks_[(x + 1) + y * ChunkConstants::kChunkSize + z *       ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : west_chunk  ? west_chunk->GetBlockAt(ChunkConstants::kChunkSize - 1, y, z)  : Block::kUndefined;
        Block east_block   = x > 0 ?                              blocks_[(x - 1) + y * ChunkConstants::kChunkSize + z *       ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : east_chunk  ? east_chunk->GetBlockAt(0, y, z)                               : Block::kUndefined;
        Block up_block     = y < ChunkConstants::kChunkSize - 1 ? blocks_[x + (y + 1) * ChunkConstants::kChunkSize + z *       ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : up_chunk    ? up_chunk->GetBlockAt(x, ChunkConstants::kChunkSize - 1, z)    : Block::kUndefined;
        Block down_block   = y > 0 ?                              blocks_[x + (y - 1) * ChunkConstants::kChunkSize + z *       ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : down_chunk  ? down_chunk->GetBlockAt(x, 0, z)                               : Block::kUndefined;

        float u = BlockProps::GetTextureU(b, BlockFace::kSouth) * kTextureElementSize;
        float v = BlockProps::GetTextureV(b, BlockFace::kSouth) * kTextureElementSize;
        if (!BlockProps::IsSolid(south_block)) {
          // south face (+z)
          // top left
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightSouth;

          // top right
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightSouth;

          // bottom left
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightSouth;

          // bottom right
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightSouth;

          // indices
          indices[current_index++] = 0 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 3 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }

        u = BlockProps::GetTextureU(b, BlockFace::kNorth) * kTextureElementSize;
        v = BlockProps::GetTextureV(b, BlockFace::kNorth) * kTextureElementSize;
        if (!BlockProps::IsSolid(north_block)) {
          // north face (-z)
          // top left
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightNorth;

          // top right
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightNorth;

          // bottom left
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightNorth;

          // bottom right
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightNorth;

          // indices
          indices[current_index++] = 0 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 3 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }

        u = BlockProps::GetTextureU(b, BlockFace::kWest) * kTextureElementSize;
        v = BlockProps::GetTextureV(b, BlockFace::kWest) * kTextureElementSize;
        if (!BlockProps::IsSolid(west_block)) {
          // west face (+x)
          // top left
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightWest;

          // top right
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightWest;

          // bottom left
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightWest;

          // bottom right
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightWest;

          // indices
          indices[current_index++] = 0 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 3 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }

        u = BlockProps::GetTextureU(b, BlockFace::kEast) * kTextureElementSize;
        v = BlockProps::GetTextureV(b, BlockFace::kEast) * kTextureElementSize;
        if (!BlockProps::IsSolid(east_block)) {
          // east face (-x)
          // top left
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightEast;

          // top right
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightEast;

          // bottom left
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightEast;

          // bottom right
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightEast;

          // indices
          indices[current_index++] = 0 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 3 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }

        u = BlockProps::GetTextureU(b, BlockFace::kBottom) * kTextureElementSize;
        v = BlockProps::GetTextureV(b, BlockFace::kBottom) * kTextureElementSize;
        if (!BlockProps::IsSolid(up_block)) {
          // bottom face (-y)
          // top left
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightBottom;

          // top right
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightBottom;

          // bottom left
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightBottom;

          // bottom right
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightBottom;

          // indices
          indices[current_index++] = 0 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 3 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }

        u = BlockProps::GetTextureU(b, BlockFace::kTop) * kTextureElementSize;
        v = BlockProps::GetTextureV(b, BlockFace::kTop) * kTextureElementSize;
        if (!BlockProps::IsSolid(down_block)) {
          // top face (+y)
          // top left
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightTop;

          // top right
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v;
          vertices[current_vertex++] = kLightTop;

          // bottom left
          vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightTop;

          // bottom right
          vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          vertices[current_vertex++] = u + kTextureElementSize;
          vertices[current_vertex++] = v + kTextureElementSize;
          vertices[current_vertex++] = kLightTop;

          // indices
          indices[current_index++] = 0 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          indices[current_index++] = 1 + num_faces * 4;
          indices[current_index++] = 3 + num_faces * 4;
          indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }
      }
    }
  }

  if (current_index == 0) {
    return;
  }

  vertices.resize(current_vertex);
  indices.resize(current_index);

  cl::MeshCreateInfo mesh_info;
  mesh_info.vertex_data_layout = { cl::ShaderDataType::kFloat3, cl::ShaderDataType::kFloat2, cl::ShaderDataType::kFloat };
  mesh_info.vertices = vertices.data();
  mesh_info.num_vertices = vertices.size();
  mesh_info.indices = indices.data();
  mesh_info.num_indices = indices.size();
  mesh_ = context_->CreateMesh(mesh_info);

  is_loaded_ = true;
}

void Chunk::DestroyMesh() {
  if (is_loaded_) {
    // TODO
  }
  is_loaded_ = false;
}

void Chunk::RecreateMesh() {
  DestroyMesh();
  CreateMesh();
}

void Chunk::Render() const {
  if (is_loaded_) {
    mesh_->Draw();
  }
}