#pragma once

namespace nlm {
namespace motor {

class AgentBrainMotor {
public:
    static MotorCommand decodeMotorCommand(AgentBrain& agentBrain);
};

} // namespace motor
} // namespace nlm
