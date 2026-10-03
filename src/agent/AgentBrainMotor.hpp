#ifndef NLM_AGENT_BRAIN_MOTOR_HPP
#define NLM_AGENT_BRAIN_MOTOR_HPP

#include "../brain/Neuron.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
#include "AgentBrainConfig.hpp"
#include "AgentBrainError.hpp"
#include <memory>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace nlm {

class AgentBrainMotor {
public:
    AgentBrainMotor(std::shared_ptr<AgentBrainConfig> config = nullptr);
    ~AgentBrainMotor() = default;
    
    // Distribute motor neurons from brain - returns error if brain is null
    AgentBrainResult<void> distributeMotorNeurons(const Brain& brain);
    
    // Decode command from brain - returns error if brain is null or invalid
    AgentBrainResult<MotorCommand> decodeCommand(const Brain& brain);
    
    // Select command with curiosity - returns error if brain is null
    AgentBrainResult<MotorCommand> selectWithCuriosity(MotorCommand defaultCmd, const Brain& brain);
    
    // Get motor output size - always returns valid result
    AgentBrainResult<size_t> getMotorOutputSize() const;
    
    // Reset motor component - returns error if config is null
    AgentBrainResult<void> reset();
    
private:
    // Configuration
    std::shared_ptr<AgentBrainConfig> config_;
    
    // Motor neuron groups (forward, backward, left, right, interact, wait)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    float curiosityLevel_;
};

} // namespace nlm

#endif // NLM_AGENT_BRAIN_MOTOR_HPP