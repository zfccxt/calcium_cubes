#include "key_bindings.hpp"

#include <calcium.hpp>

namespace KeyBindings {

cl::KeyCode key_forward   = cl::KeyCode::kW;
cl::KeyCode key_back      = cl::KeyCode::kS;
cl::KeyCode key_left      = cl::KeyCode::kA;
cl::KeyCode key_right     = cl::KeyCode::kD;
               
cl::KeyCode key_jump      = cl::KeyCode::kSpace;
cl::KeyCode key_crouch    = cl::KeyCode::kLeftShift;

cl::KeyCode key_inventory = cl::KeyCode::kE;

cl::KeyCode key_rot_up    = cl::KeyCode::kUp;
cl::KeyCode key_rot_down  = cl::KeyCode::kDown;
cl::KeyCode key_rot_left  = cl::KeyCode::kLeft;
cl::KeyCode key_rot_right = cl::KeyCode::kRight;

}
