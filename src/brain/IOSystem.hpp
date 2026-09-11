#pragma once

#include "Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <vector>

namespace nlm {

class IOSystem {
public:
    IOSystem() = default;
    
    void processSensoryInput(Brain& brain, const SensoryInput& input);
    std::unique_ptr<Action> produceAction(Brain& brain);
    
    void injectCurrentToNeuron(Brain& brain, NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(Brain& brain, NeuronType type, MembranePotential current);
    
    void processWorldIntegration(Brain& brain, TimestepDuration dt);
    void generateMotorCommands(Brain& brain, std::vector<float>& motorCommands);
    
    void logIOSystemStatus(Brain& brain) const;
    
private:
    // Process and encode sensory input
    std::vector<float> encodeSensoryInput(const SensoryInput& input) const;
    
    // Generate motor commands based on neural activity
    void generateMotorOutput(Brain& brain, std::vector<float>& commands) const;
    
    // Simple action selection based on motor neuron activity
    ActionType selectActionBasedOnMotorActivity(Brain& brain) const;
    
    // Motor command scaling and normalization
    std::vector<float> normalizeMotorCommands(const std::vector<float>& rawCommands) const;
    
    // Interneuron stimulation for motor preparation
    void stimulateMotorNeurons(Brain& brain);
};

} // namespace nlm
