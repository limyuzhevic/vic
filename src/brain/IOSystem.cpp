#include "IOSystem.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void IOSystem::processSensoryInput(Brain& brain, const SensoryInput& input) {
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = brain.getSensoryNeuronCount();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        injectCurrentToNeuron(brain, brain.getSensoryNeuron(i), normalizedValue);
        
        // Store in working memory
        if (auto* workingMemory = brain.getWorkingMemory()) {
            if (normalizedValue > 0.5f) {
                workingMemory->storeToNeuron(brain.getSensoryNeuron(i), normalizedValue / 10.0f);
            }
        }
    }
}

std::unique_ptr<Action> IOSystem::produceAction(Brain& brain) {
    // Simple action selection based on motor neuron activity
    if (brain.getMotorNeuronCount() == 0) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (size_t i = 0; i < brain.getMotorNeuronCount(); ++i) {
        auto* neuron = brain.getMotorNeuron(i);
        if (neuron && neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    return std::make_unique<Action>(type);
}

void IOSystem::injectCurrentToNeuron(Brain& brain, NeuronId neuron, MembranePotential current) {
    for (auto* region : brain.getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void IOSystem::injectCurrentToNeurons(Brain& brain, NeuronType type, MembranePotential current) {
    for (auto* region : brain.getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

void IOSystem::processWorldIntegration(Brain& brain, TimestepDuration dt) {
    // Integrate with world dynamics
    // This would update neural state based on environmental interactions
    
    // Update motor neuron states based on planned actions
    stimulateMotorNeurons(brain);
}

void IOSystem::generateMotorCommands(Brain& brain, std::vector<float>& motorCommands) {
    // Generate motor commands from neural activity
    motorCommands = normalizeMotorCommands(encodeMotorOutput(brain));
}

void IOSystem::logIOSystemStatus(Brain& brain) const {
    NLM_LOG_INFO("=== I/O System Status ===");
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(brain.getSensoryNeuronCount()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(brain.getMotorNeuronCount()));
    
    // Check for active sensory processing
    // (In a real implementation, this would check sensory input buffers)
    NLM_LOG_INFO("Sensory processing: Active");
}

std::vector<float> IOSystem::encodeSensoryInput(const SensoryInput& input) const {
    std::vector<float> encoded;
    const auto& values = input.getData();
    
    // Simple encoding - in real system would be more complex
    encoded.reserve(values.size());
    for (auto value : values) {
        encoded.push_back(static_cast<float>(value) / 255.0f * 10.0f);  // Scale to [-10, 10]
    }
    
    return encoded;
}

void IOSystem::generateMotorOutput(Brain& brain, std::vector<float>& commands) const {
    // Generate motor output based on neural activity
    size_t motorCount = brain.getMotorNeuronCount();
    
    // Simple motor mapping - in real system would be learned
    commands.resize(3, 0.0f);  // x, y, rotation commands
    
    if (motorCount > 0) {
        commands[0] = 0.5f;  // Simple forward movement
    }
}

ActionType IOSystem::selectActionBasedOnMotorActivity(Brain& brain) const {
    // Determine action based on which motor neurons are firing
    for (size_t i = 0; i < brain.getMotorNeuronCount(); ++i) {
        auto* neuron = brain.getMotorNeuron(i);
        if (neuron && neuron->isFiring()) {
            return ActionType::MoveForward;
        }
    }
    
    return ActionType::Wait;
}

std::vector<float> IOSystem::normalizeMotorCommands(const std::vector<float>& rawCommands) const {
    std::vector<float> normalized = rawCommands;
    
    // Simple normalization
    for (auto& cmd : normalized) {
        cmd = std::max(-1.0f, std::min(1.0f, cmd));  // Clamp to [-1, 1]
    }
    
    return normalized;
}

void IOSystem::stimulateMotorNeurons(Brain& brain) {
    // Stimulate motor neurons for action preparation
    for (size_t i = 0; i < brain.getMotorNeuronCount(); ++i) {
        auto* neuron = brain.getMotorNeuron(i);
        if (neuron) {
            // Apply preparation current
            neuron->injectCurrent(1.0f);
        }
    }
}

} // namespace nlm
