#include "InputProcessor.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

InputProcessor::InputProcessor(std::shared_ptr<Brain> brain)
    : brain(brain)
    , sensoryNeuronCount(0)
    , motorNeuronCount(0)
    , lastInputTime(-1.0) {
    
    if (!brain) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    extractSensorimotorNeurons();
}

void InputProcessor::processInput(const SensoryInput& input) {
    if (!brain) {
        return;
    }
    
    const auto& values = input.getData();
    if (values.empty()) {
        return;
    }
    
    if (sensoryNeuronCount == 0) {
        return;
    }
    
    // Update input timestamp
    lastInputTime = input.getTimestamp();
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < sensoryNeuronCount && i < values.size(); ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = static_cast<float>(values[i]) * 10.0f;
        
        // Find the i-th sensory neuron and inject current
        size_t neuronIndex = 0;
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuronIndex == i && neuron->getType() == NeuronType::Sensory) {
                        neuron->injectCurrent(normalizedValue);
                        
                        // Store in working memory if significant
                        if (brain->getWorkingMemory() && std::abs(normalizedValue) > 0.5f) {
                            brain->getWorkingMemory()->storeToNeuron(neuron->getId(), std::abs(normalizedValue) / 10.0f);
                        }
                        
                        return;
                    }
                    neuronIndex++;
                }
            }
        }
    }
}

void InputProcessor::extractSensorimotorNeurons() {
    if (!brain) {
        return;
    }
    
    sensoryNeuronCount = 0;
    motorNeuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                sensoryNeuronCount += pop->getNeuronCount();
            } else if (pop->getNeuronType() == NeuronType::Motor) {
                motorNeuronCount += pop->getNeuronCount();
            }
        }
    }
}

} // namespace nlm