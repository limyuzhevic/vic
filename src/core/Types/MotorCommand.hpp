#pragma once

namespace nlm {

// Motor commands for action selection
enum class MotorCommand : uint8_t {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    LookLeft,
    LookRight,
    Interact,
    Wait,
    Eat,
    Drink,
    Rest,
    Custom
};

} // namespace nlm
