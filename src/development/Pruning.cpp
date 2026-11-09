#include "Pruning.hpp"

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    // TODO PHASE 2: Implement real pruning
    // PLACEHOLDER: Weak synapses are removed probabilistically
    
    if (!brain) return;
    
    // Calculate stage-based pruning rate
    float baseRate = pImpl->pruningRate;
    float stageMultiplier = 1.0f;
    
    switch (brain->getDevelopmentalStage()) {
        case DevelopmentalStage::Initial:
            stageMultiplier = 1.5f;  // More aggressive pruning in development
            break;
        case DevelopmentalStage::CriticalPeriod:
            stageMultiplier = 1.2f;  // Continued pruning
            break;
        case DevelopmentalStage::Maturation:
            stageMultiplier = 1.0f;  // Normal pruning
            break;
        case DevelopmentalStage::Adult:
            stageMultiplier = 0.3f;  // Minimal pruning in stability
            break;
        case DevelopmentalStage::Aging:
            stageMultiplier = 0.5f;  // Moderate pruning in aging
            break;
    }
    
    float pruningRate = baseRate * stageMultiplier;
    
    // Prune weak synapses across all regions
    for (auto& region : brain->getRegions()) {
        auto& synapses = region.getSynapses();
        
        // Create a list of synapses to potentially prune (to avoid modifying during iteration)
        std::vector<Synapse*> synapsesToPrune;
        
        for (auto& syn : synapses) {
            if (!syn->isPlasticityEnabled()) continue;
            
            // Check synapse weight (weak synapses are more likely to be pruned)
            float weight = std::abs(syn->getWeight());
            if (weight < pImpl->threshold) {
                // Random pruning based on rate
                float randomValue = rng.random01();
                if (randomValue < pruningRate) {
                    synapsesToPrune.push_back(syn.get());
                }
            }
            
            // Also prune rarely used synapses (eligibility trace based)
            float eligibility = syn->getEligibilityTrace();
            if (eligibility < 0.1f && weight < pImpl->threshold * 1.5f) {
                float randomValue = rng.random01();
                if (randomValue < pruningRate * 0.5f) {
                    synapsesToPrune.push_back(syn.get());
                }
            }
        }
        
        // Actually prune the selected synapses
        for (Synapse* syn : synapsesToPrune) {
            // Find the synapse in the region's synapse list
            auto& regionSynapses = region.getSynapses();
            regionSynapses.erase(
                std::remove_if(regionSynapses.begin(), regionSynapses.end(),
                    [syn](const std::unique_ptr<Synapse>& s) { return s.get() == syn; }),
                regionSynapses.end()
            );
            
            NLM_LOG_INFO("Pruned synapse between neurons " << 
                         syn->getSourceNeuron().index() << " -> " << 
                         syn->getDestinationNeuron().index());
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    // TODO PHASE 2: Implement real synapse removal
    if (!synapse) {
        NLM_LOG_ERROR("Cannot prune null synapse");
        return false;
    }
    
    // For now, we'll just log the pruning attempt
    // In a real implementation, this would:
    // 1. Remove the synapse from all synaptic lists
    // 2. Update synaptic delivery systems
    // 3. Record the pruning event in memory systems
    // 4. Apply developmental effects
    
    NLM_LOG_INFO("Synapse pruning requested between neurons " << 
                 synapse->getSourceNeuron().index() << " -> " << 
                 synapse->getDestinationNeuron().index() << 
                 " (weight: " << synapse->getWeight() << ")");
    
    // Placeholder: Always return false for now
    return false;
}

} // namespace nlm
