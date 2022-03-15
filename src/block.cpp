#include "block.hpp"

namespace BlockProps {

bool IsSolid(Block b) {
  switch (b) {
    case Block::kAir:       return false;
    case Block::kUndefined: return false;
    default:                return true;
  }
}

float GetTextureIndex(Block b, BlockFace f) {
  switch (b) {
    case Block::kDirt:      return  0.0f;
    case Block::kGrass: {
      switch (f) {
        case BlockFace::kTop:    return 1.0f;
        case BlockFace::kBottom: return 0.0f;
        default:                 return 2.0f; 
      }
    }
    case Block::kBasalt:    return  9.0f;
    case Block::kLimestone: return 11.0f;
    default:                return  0.0f;
  }
}

}
