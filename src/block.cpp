#include "block.hpp"

namespace BlockProps {

bool IsSolid(Block b) {
  switch (b) {
    case Block::kAir:       return false;
    case Block::kUndefined: return false;
    default:                return true;
  }
}

float GetTextureU(Block b, BlockFace f) {
  switch (b) {
    case Block::kGrass: {
      switch (f) {
        case BlockFace::kTop:    return 3.0f;
        case BlockFace::kBottom: return 0.0f;
        default:                 return 2.0f; 
      }
    }
    case Block::kDirt:      return  0.0f;
    case Block::kLimestone: return 10.0f;
    case Block::kBasalt:    return  1.0f;
    default:                return  0.0f;
  }
}

float GetTextureV(Block b, BlockFace f) {
  switch (b) {
    case Block::kGrass:     return  0.0f;
    case Block::kDirt:      return  0.0f;
    case Block::kLimestone: return  0.0f;
    case Block::kBasalt:    return  0.0f;
    default:                return  0.0f;
  }
}

}
