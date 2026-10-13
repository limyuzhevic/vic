#include "BrainSensoryMotorProcessor.hpp"
#include "../Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

BrainSensoryMotorProcessor::BrainSensoryMotorProcessor(std::shared_ptr<Config> config)
    : sensoryEncodingScalingFactor(1.0f)
    , motorDecodingThreshold(0.5f)
    , reinforcementLearningEnabled(false)
    , imitationLearningEnabled(false)
    , socialLearningEnabled(false)
{
}

void BrainSensoryMotorProcessor::processSensoryInput(Brain* brain, const SensoryInput& input) {
    storeSensoryToWorkingMemory(brain, input);
}

void BrainSensoryMotorProcessor::storeSensoryToWorkingMemory(Brain* brain, const SensoryInput& input) {
    // Process sensory input using brainInitializer's sensory neurons
    auto& sensoryNeurons = brain->pImpl->brainInitializer->getSensoryNeurons();
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * sensoryEncodingScalingFactor;
        }
        
        // Inject current into this sensory neuron
        if (sensoryNeurons[i]) {
            sensoryNeurons[i]->injectCurrent(normalizedValue);
            
            // Also store in working memory
            auto* workingMemory = brain->pImpl->brainInitializer->getWorkingMemory();
            if (workingMemory && std::abs(normalizedValue) > motorDecodingThreshold) {
                workingMemory->storeToNeuron(sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            }
        }
    }
}

std::unique_ptr<Action> BrainSensoryMotorProcessor::generateAction(Brain* brain) {
    // Simple action selection based on motor neuron activity
    auto& motorNeurons = brain->pImpl->brainInitializer->getMotorNeurons();
    
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    return action;
}

void BrainSensoryMotorProcessor::applyMotorCommand(Brain* brain, const Action& action, Timestamp time) {
    // Apply motor command to world (interface through Brain)
    // This would typically involve calling methods on the environment/world system
}

void BrainSensoryMotorProcessor::injectCurrentToNeuron(Brain* brain, NeuronId neuron, MembranePotential current) {
    brain->injectCurrent(neuron, current);
}

void BrainSensoryMotorProcessor::injectCurrentToNeurons(Brain* brain, NeuronType type, MembranePotential current) {
    brain->injectCurrentToNeurons(type, current);
}

void BrainSensoryMotorProcessor::updateSensoryNeurons(Brain* brain) {
    // Update sensory neuron states for new input
}

void BrainSensoryMotorProcessor::updateMotorNeurons(Brain* brain) {
    // Update motor neuron states based on brain activity
}

void BrainSensoryMotorProcessor::configureSensoryEncoding(Brain* brain, float scalingFactor) {
    sensoryEncodingScalingFactor = scalingFactor;
}

void BrainSensoryMotorProcessor::configureMotorDecoding(Brain* brain, float threshold) {
    motorDecodingThreshold = threshold;
}

void BrainSensoryMotorProcessor::resetIOLogic(Brain* brain) {
    clearSensoryBuffers(brain);
    clearMotorBuffers(brain);
}

void BrainSensoryMotorProcessor::clearSensoryBuffers(Brain* brain) {
    // Clear any buffered sensory data
}

void BrainSensoryMotorProcessor::clearMotorBuffers(Brain* brain) {
    // Clear any buffered motor data
}

void BrainSensoryMotorProcessor::enableReinforcementLearning(Brain* brain, bool enabled) {
    reinforcementLearningEnabled = enabled;
    if (enabled) {
        enableReinforcementLearning(brain);
    }
}

void BrainSensoryMotorProcessor::enableImitationLearning(Brain* brain, bool enabled) {
    imitationLearningEnabled = enabled;
    if (enabled) {
        enableImitationLearning(brain);
    }
}

void BrainSensoryMotorProcessor::enableSocialLearning(Brain* brain, bool enabled) {
    socialLearningEnabled = enabled;
    if (enabled) {
        enableSocialLearning(brain);
    }
}

void BrainSensoryMotorProcessor::encodeSensoryInput(Brain* brain, const SensoryInput& input) {
    // Convert sensory input to neural representation
}

void BrainSensoryMotorProcessor::distributeInputAcrossSensoryNeurons(Brain* brain, const SensoryInput& input) {
    // Distribute sensory input across multiple sensory neurons
}

void BrainSensoryMotorProcessor::normalizeSensoryValues(Brain* brain, std::vector<float>& values) {
    // Normalize sensory values to consistent range
}

void BrainSensoryMotorProcessor::decodeMotorOutput(Brain* brain) {
    // Decode motor output from neural activity
}

void BrainSensoryMotorProcessor::updateMotorNeuronsFromBrainActivity(Brain* brain) {
    // Update motor neurons based on brain activity patterns
}

void BrainSensoryMotorProcessor::selectMotorAction(Brain* brain) {
    // Select appropriate motor action based on neural activity
}

void BrainSensoryMotorProcessor::integrateSensoryWithMemory(Brain* brain) {
    // Integrate sensory input with memory systems
}

void BrainSensoryMotorProcessor::integrateMotorWithWorld(Brain* brain) {
    // Integrate motor output with world simulation
}

void BrainSensoryMotorProcessor::integrateLearningWithIOLogic(Brain* brain) {
    // Integrate learning mechanisms with I/O processing
}

void BrainSensoryMotorProcessor::enableReinforcementLearning(Brain* brain) {
    // Enable reinforcement learning for I/O
}

void BrainSensoryMotorProcessor::enableImitationLearning(Brain* brain) {
    // Enable imitation learning for I/O
}

void BrainSensoryMotorProcessor::enableSocialLearning(Brain* brain) {
    // Enable social learning for I/O
}

void BrainSensoryMotorProcessor::updateIOStatistics(Brain* brain) {
    // Update I/O related statistics
}

float BrainSensoryMotorProcessor::calculateSensoryEntropy(Brain* brain) const {
    // Calculate entropy of sensory input
    return 0.0f;
}

float BrainSensoryMotorProcessor::calculateMotorOutputUncertainty(Brain* brain) const {
    // Calculate uncertainty of motor output
    return 0.0f;
}