#include "BrainIO.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainIO::receiveSensoryInput(Brain::Impl& pImpl, const SensoryInput& input) {
    try {
        // Validate inputs
        if (!pImpl->config) {
            NLM_LOG_ERROR("Failed to receive sensory input: Configuration is null");
            return;
        }
        
        // Inject current into sensory neurons based on input
        // This is a simple mapping - sensory encoding
        
        const auto& values = input.getData();
        if (values.empty()) {
            NLM_LOG_DEBUG("Empty sensory input received");
            return;
        }
        
        size_t numSensory = pImpl->sensoryNeurons.size();
        if (numSensory == 0) {
            NLM_LOG_WARNING("No sensory neurons available to process input");
            return;
        }
        
        // Distribute input across sensory neurons
        for (size_t i = 0; i < numSensory; ++i) {
            // Normalize input value to range [-10, 10] mV
            float normalizedValue = 0.0f;
            if (i < values.size()) {
                normalizedValue = static_cast<float>(values[i]) * 10.0f;
            }
            
            // Validate neuron pointer
            if (!pImpl->sensoryNeurons[i]) {
                NLM_LOG_WARNING("Null sensory neuron at index " + std::to_string(i));
                continue;
            }
            
            // Inject current into this sensory neuron
            pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
            
            // Also store in working memory
            if (pImpl->workingMemory && normalizedValue > 0.5f) {
                try {
                    pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to store sensory input in working memory: " + e.what());
                }
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during sensory input processing: ") + e.what());
    }
}

std::unique_ptr<Action> BrainIO::produceAction(const Brain::Impl& pImpl) {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
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

void BrainIO::logIOStats(const Brain::Impl& pImpl) const {
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl.sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl.motorNeurons.size()));
}

void BrainIO::injectCurrentFromSensoryInput(Brain::Impl& pImpl, const SensoryInput& input) {
    receiveSensoryInput(pImpl, input);
}

void BrainIO::updateMotorNeurons(Brain::Impl& pImpl) {
    // Update motor neuron activity based on brain state
    for (auto* neuron : pImpl.motorNeurons) {
        // Motor neuron dynamics would be updated here
        // For now, this is a placeholder
    }
}

void BrainIO::handleActionSelection(Brain::Impl& pImpl) {
    // Handle action selection logic
    // This would integrate with cognitive systems for more sophisticated action selection
}

} // namespace nlm
