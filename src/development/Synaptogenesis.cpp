#include "Synaptogenesis.hpp"

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f) {}
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
    // Phase 2: Real synaptogenesis implementation
    // Form new synapses based on rate and target density
    if (!brain) return;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    const float formationRate = getFormationRate();
    const float targetDensity = getTargetDensity();
    
    // Get current connectivity statistics
    size_t maxPossibleSynapses = 0;
    auto neurons = region->getAllNeurons();
    
    // Count all possible connections (excluding self-connections)
    for (size_t i = 0; i < neurons.size(); ++i) {
        for (size_t j = i + 1; j < neurons.size(); ++j) {
            maxPossibleSynapses++;
        }
    }
    
    // Get actual synapse count
    size_t currentSynapseCount = region->getSynapseCount();
    
    // Calculate how many new synapses we should form
    float currentDensity = (maxPossibleSynapses > 0) ? 
        static_cast<float>(currentSynapseCount) / maxPossibleSynapses : 0.0f;
    
    float densityDeficit = targetDensity - currentDensity;
    size_t synapsesToForm = static_cast<size_t>(densityDeficit * formationRate * neurons.size());
    
    // Form new synapses
    size_t formedCount = 0;
    for (size_t attempts = 0; attempts < synapsesToForm * 10 && formedCount < synapsesToForm; ++attempts) {
        // Randomly select source and destination neurons
        if (neurons.size() < 2) break;
        
        size_t sourceIdx = rng.get<size_t>(0, neurons.size() - 1);
        size_t destIdx = rng.get<size_t>(0, neurons.size() - 1);
        
        // Avoid self-connections
        if (sourceIdx == destIdx) continue;
        
        // Check if connection already exists
        auto synapses = region->getSynapsesFrom(neurons[sourceIdx]);
        bool exists = false;
        for (auto* syn : synapses) {
            if (syn->getOwner() == region && 
                syn->getSourceNeuron() == neurons[sourceIdx] &&
                syn->getTargetNeuron() == neurons[destIdx]) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            // Form new synapse
            region->addSynapse(
                neurons[sourceIdx]->getId(),
                neurons[destIdx]->getId(),
                0.5f + rng.getFloat() * 0.5f,  // Random weight between 0.5 and 1.0
                1  // Default delay
            );
            formedCount++;
        }
    }
    
    if (formedCount > 0) {
        NLM_LOG_INFO("Formed " + std::to_string(formedCount) + " new synapses");
    }
}

} // namespace nlm
