// BrainIntegration.cpp - Implementation of Brain integration functions

#include "BrainIntegration.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Integrate sensory input into the brain system
void integrateSensoryInput(Brain::Impl* impl, const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = impl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        impl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (impl->workingMemory && normalizedValue > 0.5f) {
            impl->workingMemory->storeToNeuron(impl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

// Inject current directly into a specific neuron
void injectCurrentToNeuron(Brain::Impl* impl, NeuronId neuron, MembranePotential current) {
    for (auto& region : impl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

// Inject current into all neurons of a specific type
void injectCurrentToNeurons(Brain::Impl* impl, NeuronType type, MembranePotential current) {
    for (auto& region : impl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

// Produce motor/action output based on neural activity
std::unique_ptr<class Action> produceAction(Brain::Impl* impl) {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (impl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : impl->motorNeurons) {
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

// Apply neuromodulatory signals to plasticity
void applyNeuromodulation(Brain::Impl* impl, const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    impl->stdp->setLTPWeight(0.01f * modulation);
    impl->stdp->setLTDWeight(0.012f * modulation);
}

// Update plasticity rules (called automatically in step)
void updatePlasticity(Brain::Impl* impl) {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
    // All plasticity is handled in the step function
}

// Apply developmental changes (called automatically in step)
void develop(Brain::Impl* impl) {
    // Development updates structural plasticity
    impl->structuralPlasticity->update(nullptr, *impl->rng);  // Note: would need Brain pointer
}

} // namespace nlm