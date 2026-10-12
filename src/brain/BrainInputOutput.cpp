#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../memory/NeuralWorkingMemory.hpp"

namespace nlm {

// Brain input/output methods

void Brain::receiveSensoryInput(const SensoryInput& input) {
    NLM_LOG_DEBUG("Receiving sensory input from " + input.getSensorName());
    
    // Inject current into sensory neurons based on input pattern
    for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
        Neuron* neuron = pImpl->sensoryNeurons[i];
        
        // Get sensory encoding based on input type
        float sensoryEncoding = 0.0f;
        
        switch (input.getType()) {
            case SensoryInputType::Visual:
                sensoryEncoding = input.getValue() * 0.8f; // Strong visual influence
                break;
            case SensoryInputType::Auditory:
                sensoryEncoding = input.getValue() * 0.6f; // Moderate auditory influence
                break;
            case SensoryInputType::Tactile:
                sensoryEncoding = input.getValue() * 0.5f; // Lower tactile influence
                break;
            case SensoryInputType::Olfactory:
                sensoryEncoding = input.getValue() * 0.3f; // Minimal olfactory influence
                break;
            case SensoryInputType::Gustatory:
                sensoryEncoding = input.getValue() * 0.2f; // Minimal gustatory influence
                break;
            case SensoryInputType::Interoceptive:
                sensoryEncoding = input.getValue() * 0.4f; // Low interoceptive influence
                break;
        }
        
        // Inject current into sensory neuron
        neuron->injectCurrent(sensoryEncoding);
        
        // Store in working memory for prediction error calculation
        if (pImpl->workingMemory) {
            pImpl->workingMemory->storeSensoryInput(i, sensoryEncoding);
        }
    }
    
    NLM_LOG_DEBUG("Applied sensory input to " + std::to_string(pImpl->sensoryNeurons.size()) + 
                 " sensory neurons");
}

void Brain::injectCurrent(NeuronId neuronId, MembranePotential current) {
    NLM_LOG_DEBUG("Injecting current " + std::to_string(current) + " into neuron " + 
                 std::to_string(neuronId));
    
    // Find and inject current into specific neuron
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == neuronId) {
                neuron->injectCurrent(current);
                NLM_LOG_DEBUG("Successfully injected current into neuron " + std::to_string(neuronId));
                return;
            }
        }
    }
    
    NLM_LOG_WARNING("Attempted to inject current into non-existent neuron " + std::to_string(neuronId));
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    NLM_LOG_DEBUG("Injecting current " + std::to_string(current) + " into all " + 
                 neuronTypeToString(type) + " neurons");
    
    size_t injectedCount = 0;
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            // Check if population type matches target type
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                    injectedCount++;
                }
            }
        }
    }
    
    NLM_LOG_DEBUG("Injected current into " + std::to_string(injectedCount) + " " + 
                 neuronTypeToString(type) + " neurons");
}

std::unique_ptr<Action> Brain::produceAction() {
    NLM_LOG_DEBUG("Producing action based on motor neuron activity");
    
    if (pImpl->motorNeurons.empty()) {
        NLM_LOG_WARNING("No motor neurons available for action production");
        return nullptr;
    }
    
    // Find the most active motor neuron
    Neuron* mostActiveNeuron = nullptr;
    float maxActivity = -1.0f;
    
    for (Neuron* neuron : pImpl->motorNeurons) {
        const auto& state = neuron->getState();
        float activity = std::abs(state.membranePotential - state.restingPotential);
        
        if (activity > maxActivity) {
            maxActivity = activity;
            mostActiveNeuron = neuron;
        }
    }
    
    if (!mostActiveNeuron) {
        NLM_LOG_DEBUG("No active motor neurons detected");
        return nullptr;
    }
    
    // Create action based on which region the neuron belongs to
    size_t actionSource = 0;
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        if (std::find(pImpl->motorNeurons.begin(), pImpl->motorNeurons.end(), 
                     mostActiveNeuron) != pImpl->motorNeurons.end()) {
            actionSource = i;
            break;
        }
    }
    
    // Create action
    auto action = std::make_unique<Action>();
    action->setActionType(ActionType::Movement);
    action->setStrength(maxActivity);
    action->setSourceRegion(RegionId(actionSource + 1)); // Region IDs are 1-based
    
    NLM_LOG_DEBUG("Produced action: type=" + actionTypeToString(action->getActionType()) + 
                 ", strength=" + std::to_string(action->getStrength()) + 
                 ", source_region=" + std::to_string(action->getSourceRegion()));
    
    return action;
}

// Helper function for neuron type to string conversion
std::string Brain::neuronTypeToString(NeuronType type) {
    switch (type) {
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Internal: return "Internal";
        case NeuronType::Motor: return "Motor";
        default: return "Unknown";
    }
}

// Helper function for action type to string conversion
std::string Brain::actionTypeToString(ActionType type) {
    switch (type) {
        case ActionType::Movement: return "Movement";
        case ActionType::Exploration: return "Exploration";
        case ActionType::Interaction: return "Interaction";
        default: return "Unknown";
    }
}

} // namespace nlm