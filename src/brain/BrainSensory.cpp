// BrainSensory.cpp - Sensory input processing implementation
#include "BrainSensory.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

BrainSensory::BrainSensory() : brain_(nullptr) {}

BrainSensory::~BrainSensory() = default;

void BrainSensory::receiveSensoryInput(const SensoryInput& input) {
    if (!brain_) {
        NLM_LOG_ERROR("BrainSensory: No brain reference set!");
        return;
    }
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons_.size();
    if (numSensory == 0) {
        NLM_LOG_WARNING("BrainSensory: No sensory neurons available");
        return;
    }
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        if (sensoryNeurons_[i]) {
            sensoryNeurons_[i]->injectCurrent(normalizedValue);
            
            // Store in working memory if significant input
            if (brain_->getWorkingMemory() && std::abs(normalizedValue) > 0.5f) {
                brain_->getWorkingMemory()->storeToNeuron(sensoryNeurons_[i]->getId(), 
                    std::abs(normalizedValue) / 10.0f);
            }
        }
    }
}

void BrainSensory::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (!brain_) return;
    
    // Search through all regions for the specified neuron
    for (const auto& region : brain_->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                NLM_LOG_DEBUG("BrainSensory: Injected " + std::to_string(current) + 
                           " mV into neuron " + std::to_string(neuron.index()));
                return;
            }
        }
    }
    
    NLM_LOG_WARNING("BrainSensory: Neuron " + std::to_string(neuron.index()) + " not found");
}

void BrainSensory::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    if (!brain_) return;
    
    // Search through all regions for neurons of specified type
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
    
    NLM_LOG_DEBUG("BrainSensory: Injected " + std::to_string(current) + " mV into " + 
               std::to_string(type) + " neurons");
}

void BrainSensory::collectSensoryMotorNeurons(Brain* brain) {
    brain_ = brain;
    sensoryNeurons_.clear();
    motorNeurons_.clear();
    
    if (!brain_) return;
    
    // Collect sensory and motor neurons from brain regions
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    motorNeurons_.push_back(n);
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    sensoryNeurons_.push_back(n);
                }
            }
        }
    }
    
    NLM_LOG_INFO("BrainSensory: Collected " + std::to_string(sensoryNeurons_.size()) + 
               " sensory neurons and " + std::to_string(motorNeurons_.size()) + 
               " motor neurons");
}

} // namespace nlm