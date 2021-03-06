#pragma once

enum class Block : char {
  kUndefined, kAir, kDirt, kLimestone, kBasalt, kGrass,
};

enum class BlockFace : char {
  kNorth, kSouth, kEast, kWest, kTop, kBottom
};

namespace BlockProps {

bool IsSolid(Block b);

float GetTextureIndex(Block b, BlockFace f);

}
