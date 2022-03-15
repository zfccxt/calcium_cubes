#include "chunk.hpp"

#include <cstdint>
#include <vector>

#include "chunk_generator.hpp"
#include "world.hpp"

const size_t kVertexSize         = 7;
const size_t kNumIndicesPerFace  = 6;

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
  cl::MeshCreateInfo mesh_info;
  mesh_info.vertex_input_layout = { cl::ShaderDataType::kFloat3, cl::ShaderDataType::kFloat2, cl::ShaderDataType::kFloat };

  mesh_info.vertices.resize(ChunkConstants::kChunkSize * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize * 6 * 4 * kVertexSize);
  mesh_info.indices.resize(ChunkConstants::kChunkSize * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize * 6 * kNumIndicesPerFace);

  size_t current_vertex = 0;
  size_t current_index = 0;
  uint16_t num_faces = 0;

  for (int x = 0; x < ChunkConstants::kChunkSize; ++x) {
    for (int y = 0; y < ChunkConstants::kChunkSize; ++y) {
      for (int z = 0; z < ChunkConstants::kChunkSize; ++z) {
        Block b = blocks_[x + y * ChunkConstants::kChunkSize + z * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize];
        if (b == Block::kAir || b == Block::kUndefined) {
          continue;
        }
     
        // TODO: Occlusion culling across chunk boundaries
        Block south_block  = z < ChunkConstants::kChunkSize - 1 ? blocks_[x + y *       ChunkConstants::kChunkSize
          + (z + 1) * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : Block::kUndefined;
        Block north_block  = z > 0 ?                              blocks_[x + y *       ChunkConstants::kChunkSize
          + (z - 1) * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : Block::kUndefined;
        Block west_block   = x < ChunkConstants::kChunkSize - 1 ? blocks_[(x + 1) + y * ChunkConstants::kChunkSize
          + z *       ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : Block::kUndefined;
        Block east_block   = x > 0 ?                              blocks_[(x - 1) + y * ChunkConstants::kChunkSize
          + z *       ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : Block::kUndefined;
        Block up_block     = y < ChunkConstants::kChunkSize - 1 ? blocks_[x + (y + 1) * ChunkConstants::kChunkSize
          + z *       ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : Block::kUndefined;
        Block down_block   = y > 0 ?                              blocks_[x + (y - 1) * ChunkConstants::kChunkSize
          + z *       ChunkConstants::kChunkSize * ChunkConstants::kChunkSize] : Block::kUndefined;

        if (!BlockProps::IsSolid(south_block)) {
          // south face (+z)
          // top left
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kSouth);
          mesh_info.vertices[current_vertex++] = kLightSouth;

          // top right
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kSouth);
          mesh_info.vertices[current_vertex++] = kLightSouth;

          // bottom left
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kSouth);
          mesh_info.vertices[current_vertex++] = kLightSouth;

          // bottom right
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kSouth);
          mesh_info.vertices[current_vertex++] = kLightSouth;

          // indices
          mesh_info.indices[current_index++] = 0 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 3 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }

        if (!BlockProps::IsSolid(north_block)) {
          // north face (-z)
          // top left
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kNorth);
          mesh_info.vertices[current_vertex++] = kLightNorth;

          // top right
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kNorth);
          mesh_info.vertices[current_vertex++] = kLightNorth;

          // bottom left
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kNorth);
          mesh_info.vertices[current_vertex++] = kLightNorth;

          // bottom right
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kNorth);
          mesh_info.vertices[current_vertex++] = kLightNorth;

          // indices
          mesh_info.indices[current_index++] = 0 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 3 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }

        if (!BlockProps::IsSolid(west_block)) {
          // west face (+x)
          // top left
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kWest);
          mesh_info.vertices[current_vertex++] = kLightWest;

          // top right
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kWest);
          mesh_info.vertices[current_vertex++] = kLightWest;

          // bottom left
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kWest);
          mesh_info.vertices[current_vertex++] = kLightWest;

          // bottom right
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kWest);
          mesh_info.vertices[current_vertex++] = kLightWest;

          // indices
          mesh_info.indices[current_index++] = 0 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 3 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }

        if (!BlockProps::IsSolid(east_block)) {
          // east face (-x)
          // top left
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kEast);
          mesh_info.vertices[current_vertex++] = kLightEast;

          // top right
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kEast);
          mesh_info.vertices[current_vertex++] = kLightEast;

          // bottom left
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kEast);
          mesh_info.vertices[current_vertex++] = kLightEast;

          // bottom right
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kEast);
          mesh_info.vertices[current_vertex++] = kLightEast;

          // indices
          mesh_info.indices[current_index++] = 0 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 3 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }

        if (!BlockProps::IsSolid(up_block)) {
          // bottom face (-y)
          // top left
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kBottom);
          mesh_info.vertices[current_vertex++] = kLightBottom;

          // top right
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kBottom);
          mesh_info.vertices[current_vertex++] = kLightBottom;

          // bottom left
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kBottom);
          mesh_info.vertices[current_vertex++] = kLightBottom;

          // bottom right
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kBottom);
          mesh_info.vertices[current_vertex++] = kLightBottom;

          // indices
          mesh_info.indices[current_index++] = 0 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 3 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }

        if (!BlockProps::IsSolid(down_block)) {
          // top face (+y)
          // top left
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kTop);
          mesh_info.vertices[current_vertex++] = kLightTop;

          // top right
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kTop);
          mesh_info.vertices[current_vertex++] = kLightTop;

          // bottom left
          mesh_info.vertices[current_vertex++] = 0.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kTop);
          mesh_info.vertices[current_vertex++] = kLightTop;

          // bottom right
          mesh_info.vertices[current_vertex++] = 1.0f + x + chunk_x_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + y + chunk_y_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 0.0f + z + chunk_z_ * ChunkConstants::kChunkSize;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = 1.0f;
          mesh_info.vertices[current_vertex++] = BlockProps::GetTextureIndex(b, BlockFace::kTop);
          mesh_info.vertices[current_vertex++] = kLightTop;

          // indices
          mesh_info.indices[current_index++] = 0 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          mesh_info.indices[current_index++] = 1 + num_faces * 4;
          mesh_info.indices[current_index++] = 3 + num_faces * 4;
          mesh_info.indices[current_index++] = 2 + num_faces * 4;
          ++num_faces;
        }
      }
    }
  }

  if (current_index == 0) {
    return;
  }

  mesh_info.vertices.resize(current_vertex);
  mesh_info.indices.resize(current_index);

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