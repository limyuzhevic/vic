#pragma once

namespace nlm {

// MotorCommand: Low-level motor output enumeration
// These represent the 6 main motor commands that the agent can execute
enum class MotorCommand {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    Interact,
    Wait,
    Look  // Special command for looking around
};

} // namespace nlm
