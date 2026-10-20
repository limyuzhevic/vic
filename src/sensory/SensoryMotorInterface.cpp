#include "SensoryMotorInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"

namespace nlm {

struct SensoryMotorInterface::Impl {
    std::shared_ptr<Config> config;
    Brain* brain;
    
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), brain(nullptr), inputMin(-10.0f), inputMax(10.0f), outputMin(-1.0f), outputMax(1.0f) {}
    
    ~Impl() = default;
};

SensoryMotorInterface::SensoryMotorInterface(std::shared_ptr<Config> config) : 
    pImpl(new Impl(config)),
    inputMin(-10.0f),
    inputMax(10.0f),
    outputMin(-1.0f),
    outputMax(1.0f) {}

SensoryMotorInterface::~SensoryMotorInterface() {
    delete pImpl;
}

bool SensoryMotorInterface::initialize(Brain* brain) {
    NLM_LOG_INFO("Initializing SensoryMotorInterface...");
    
    pImpl->brain = brain;
    
    // Collect sensory and motor neurons
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            auto* sensoryPop = pop.get(); // Need to fix this - should use getPopulation
            auto* motorPop = pop.get(); // Need to fix this
            
            // For now, just log that we need to access populations
            // In a complete implementation, we would properly access the populations
            NLM_LOG_INFO("SensoryMotorInterface: Found population of type " + std::to_string(static_cast<int>(pop->getNeuronType())));
        }
    }
    
    // NOTE: The current implementation needs access to region->getPopulation methods
    // This will be implemented in the complete version
    
    NLM_LOG_INFO("SensoryMotorInterface initialized successfully");
    return true;
}

void SensoryMotorInterface::receiveSensoryInput(const SensoryInput& input) {
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [inputMin, inputMax]
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]);
        }
        
        // Scale to input range
        float scaledValue = inputMin + (normalizedValue * (inputMax - inputMin));
        
        // Inject current into this sensory neuron
        if (pImpl->sensoryNeurons[i]) {
            pImpl->sensoryNeurons[i]->injectCurrent(scaledValue);
        }
    }
}

void SensoryMotorInterface::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (!pImpl->brain) return;
    
    for (auto& region : pImpl->brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void SensoryMotorInterface::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    if (!pImpl->brain) return;
    
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::unique_ptr<Action> SensoryMotorInterface::produceAction() {
    // Simple action selection based on motor neuron activity
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    return std::make_unique<Action>(type);
}

void SensoryMotorInterface::reset() {
    NLM_LOG_INFO("Resetting SensoryMotorInterface...");
    NLM_LOG_INFO("SensoryMotorInterface reset complete");
}

} // namespace nlm