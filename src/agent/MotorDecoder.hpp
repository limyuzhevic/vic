#pragma once

#include "../core/Types/Types.hpp"
#include "../agent/AgentBody.hpp"
#include "../core/Config/Config.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

// Forward declarations
class Brain;
class ExplorationAgent;

// MotorDecoder: Handles motor activity decoding
// Converts neural activity into motor commands with curiosity-based exploration
class MotorDecoder {
public:
    MotorDecoder(std::shared_ptr<Config> config);
    ~MotorDecoder();
    
    // Configure with agent body and exploration agent
    void initialize(std::shared_ptr<Brain> brain, const AgentBody& body, 
                   std::shared_ptr<ExplorationAgent> explorationAgent);
    
    // Get expected motor output size based on configuration
    size_t getMotorOutputSize() const;
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Decode motor command from neuron activity
    MotorCommand decodeFromMotorNeurons();
    
    // Select motor command with curiosity-based exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Only act if there's meaningful activity
    bool hasMeaningfulActivity(const std::vector<float>& activities) const;
    
private:
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Brain reference
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups (owned by brain, just references)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Exploration agent for curiosity-based actions
    std::shared_ptr<ExplorationAgent> explorationAgent_;
    
    // Calculate average activity in a neuron group
    float calculateActivity(const std::vector<Neuron*>& neurons) const;
};

} // namespace nlm
