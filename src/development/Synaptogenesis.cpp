#include "Synaptogenesis.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float homeostaticAdjustment;
    float maturationTime;
    float criticalPeriod;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), homeostaticAdjustment(0.1f),
             maturationTime(10.0f), criticalPeriod(100.0f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = rate;
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = density;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    // TODO PHASE 2: Implement real synaptogenesis
    // PLACEHOLDER: New synapses form probabilistically
    
    if (!brain) return;
    
    // Get current network state
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t currentSynapses = brain->getTotalSynapseCount();
    
    // Calculate current synaptic density
    float currentDensity = currentSynapses / static_cast<float>(totalNeurons * totalNeurons);
    
    // Homeostatic regulation: target optimal density
    float densityError = pImpl->targetDensity - currentDensity;
    
    // Adjusted formation rate based on homeostatic needs
    float adjustedRate = pImpl->formationRate * (1.0f + pImpl->homeostaticAdjustment * densityError);
    
    // Stochastic synapse formation
    float formationProbability = std::min(1.0f, adjustedRate);
    
    // For each neuron pair, consider forming new synapses
    auto regions = brain->getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = i + 1; j < neurons.size(); ++j) {
                // Check if neurons are already connected
                bool alreadyConnected = false;
                for (const auto& synapse : neurons[i]->getOutgoingSynapses()) {
                    if (synapse->getTarget() == neurons[j]->getId()) {
                        alreadyConnected = true;
                        break;
                    }
                }
                
                if (!alreadyConnected) {
                    // Random chance of forming new synapse
                    if (rng.uniformReal(0.0f, 1.0f) < formationProbability) {
                        // Create new synapse
                        auto newSynapse = std::make_unique<Synapse>(neurons[i]->getId(), neurons[j]->getId());
                        
                        // Initialize synapse parameters
                        newSynapse->setWeight(rng.uniformReal(-1.0f, 1.0f));
                        newSynapse->enablePlasticity(true, true, false);
                        
                        // Add synapse to both neurons
                        neurons[i]->addOutgoingSynapse(newSynapse.get());
                        neurons[j]->addIncomingSynapse(newSynapse.get());
                        
                        // Add synapse to region
                        region->addSynapse(std::move(newSynapse));
                    }
                }
            }
        }
    }
}

} // namespace nlm
