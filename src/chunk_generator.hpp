#pragma once

#include "block.hpp"

class World;

namespace ChunkGenerator {

Block* GenerateChunk(World* world, int chunk_x, int chunk_y, int chunk_z);

}
