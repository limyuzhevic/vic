// BrainIO.cpp - Implementation of Brain I/O handler
#include "BrainIO.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <algorithm>

namespace nlm {

BrainIO::BrainIO(Brain* brain) : brain_(brain) {
}

BrainIO::~BrainIO() = default;

void BrainIO::receiveSensoryInput(const SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = brain_->getSensoryNeuronCount();
    if (numSensory == upuffered) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        brain_->storeSensoryNeuronAt(i)->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (auto* workingMemory = brain_->getWorkingMemory() && normalizedValue > 0.5f) {
            workingMemory->storeToNeuron(brain_->getSensoryNeuronAt(i)->getId(), normalizedValue / 10.0f);
        }
    }
}

void BrainIO::injectCurrent(NeuronId neuron, MembranePotential current) {
    auto it = brain_->getImpl()->neuronById.find(neuron.index());
    if (it != brain_->getImpl()->neuronById.end()) {
        it->second->injectCurrent(current);
        return;
    }
    for (auto& region : brain_->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void BrainIO::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::unique_ptr<Action> BrainIO::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (brain_->getMotorNeuronCount() == 0) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : brain_->getMotorNeurons()) {
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

void BrainIO::applyNeuromodulation(const Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (auto* stdp = brain_->getSTDP()) {
        stdp->setLTPWeight(0.01f * modulation);
        stdp->setLTDWeight(0.012f * modulation);
    }
}

void BrainIO::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void BrainIO::develop() {
    // Development updates structural plasticity
    if (auto* structuralPlasticity = brain_->getStructuralPlasticity()) {
        structuralPlasticity->update(brain_, *brain_->getRandomGenerator());
    }
}

void BrainIO::reset() {
    // Reset brain state
    brain_->getImpl()->currentStep = 0;
    brain_->getImpl()->currentTime = 0.0;
    brain_->getImpl()->totalSpikesThisStep = 0;
    brain_->getImpl()->totalSpikesTotal = 0;
    brain_->getImpl()->isResting = false;
    brain_->getImpl()->stepsSinceLastEpisode = 0;
    
    for (auto& region : brain_->getRegions()) {
        region->reset();
    }
    
    if (auto* spikeSystem = brain_->getSpikeSystem()) {
        spikeSystem->reset();
    }
    brain_->getImpl()->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (auto* workingMemory = brain_->getWorkingMemory()) workingMemory->clear();
    if (auto* episodicMemory = brain_->getEpisodicMemory()) episodicMemory->clear();
    if (auto* associativeMemory = brain_->getAssociativeMemory()) associativeMemory->clear();
    if (auto* attention = brain_->getAttention()) attention->reset();
}

} // namespace nlm
