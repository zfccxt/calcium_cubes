#include "chunk_generator.hpp"

#include <cmath>
#include <stdlib.h>
#include <simplex.h>

#include "chunk.hpp"
#include "chunk_constants.hpp"
#include "world.hpp"

const float kGradientSampleDistance = 0.01f; // Rate at which foothills transition to mountains. Smaller number = smoother gradients
const float kMountainHeight = 10.0f;         // Maximum height/depth of terrain features
const float kDirtDepth = 4.0f;               // Average depth of the top layer of dirt and grass before transitioning to stone
const float kDirtDepthSampleDistance = 0.2f; // Rate at which dirt depth varies. Smaller = smoother transitions

namespace ChunkGenerator {

Block* GenerateChunk(World* world, int chunk_x, int chunk_y, int chunk_z) {
  Block* blocks = new Block[ChunkConstants::kChunkSize * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize];

  for (int x = 0; x < ChunkConstants::kChunkSize; ++x) {
    float block_x = chunk_x * (float)ChunkConstants::kChunkSize + x;

    for (int y = 0; y < ChunkConstants::kChunkSize; ++y) {
    float block_y = chunk_y * (float)ChunkConstants::kChunkSize + y;

      for (int z = 0; z < ChunkConstants::kChunkSize; ++z) {
        float block_z = chunk_z * (float)ChunkConstants::kChunkSize + z;

        int index = x + y * ChunkConstants::kChunkSize + z * ChunkConstants::kChunkSize * ChunkConstants::kChunkSize;

        // If we are above surface height, the block must be air
        float surface_height = (float)simplex_noise2d(block_x * kGradientSampleDistance, block_z * kGradientSampleDistance) * kMountainHeight;
        if (block_y < surface_height) {
          blocks[index] = Block::kAir;
          continue;
        }

        // Default block type is stone
        // TODO: Select stone type based on region
        blocks[index] = Block::kBasalt;

        // Replace top layer of stone with dirt
        float dirt_depth = (float)abs(simplex_noise3d(block_x * kGradientSampleDistance, block_z * kGradientSampleDistance, 967.15f) + 0.9) * kDirtDepth;
        if (block_y < surface_height + dirt_depth) {
          blocks[index] = Block::kDirt;
        }

        // Replace top layer of dirt with grass
        if (block_y < surface_height + 1.0f) {
          blocks[index] = Block::kGrass;
        }
      }
    }
  }

  return blocks;
}

}
