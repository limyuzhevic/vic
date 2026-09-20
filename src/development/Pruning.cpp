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

Pruning::Pruning() : pImpl(std::make_unique<Impl>()) {}

Pruning::~Pruning() = default;

Pruning::Pruning(Pruning&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Pruning& Pruning::operator=(Pruning&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

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
    if (!brain) return;
    
    // Implement real pruning algorithm
    // Based on synaptic weight statistics
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    const auto& synapses = region->getSynapses();
    if (synapses.empty()) return;
    
    // Calculate mean absolute weight for thresholding
    float totalWeight = 0.0f;
    float totalAbsWeight = 0.0f;
    
    for (const auto& syn : synapses) {
        totalWeight += syn->getWeight();
        totalAbsWeight += std::abs(syn->getWeight());
    }
    
    float meanWeight = totalWeight / synapses.size();
    float meanAbsWeight = totalAbsWeight / synapses.size();
    
    // Prune weak synapses below threshold
    // Threshold is based on mean absolute weight with statistical variation
    float dynamicThreshold = pImpl->threshold * meanAbsWeight * 
                           (1.0f + rng.uniformReal(-0.2f, 0.2f));
    
    // Also consider connection age if available
    // For now, use weight-based pruning only
    
    // Implement stochastic pruning based on rate and threshold
    for (Synapse* syn : synapses) {
        // Check if synapse is a candidate for pruning
        float weightMag = std::abs(syn->getWeight());
        float rand = rng.uniformReal(0.0f, 1.0f);
        
        bool shouldPrune = (weightMag < dynamicThreshold) && (rand < pImpl->pruningRate);
        
        if (shouldPrune && syn->getPlasticityFlags().hebbian) {
            // Mark synapse for pruning - could implement soft or hard pruning
            // For hard pruning, we would remove the synapse from the region
            // For soft pruning, we could reduce weight towards zero
            
            // For now, implement soft pruning by reducing weight to zero
            syn->setWeight(0.0f);
            
            // Log pruning event
            NLM_LOG_INFO("Pruning: Synapse weight reduced to 0 (id: " + 
                       std::to_string(syn->getId().index()) + ")");
        }
    }
    
    // Implement homeostatic pruning to maintain network stability
    // Remove excess connections if network is too dense
    float synapticDensity = region->getSynapticDensity();
    
    if (synapticDensity > 0.3f) {  // If network is too dense
        // Apply additional aggressive pruning
        float additionalPruning = (synapticDensity - 0.3f) * 0.5f;  // Scale with density excess
        
        for (Synapse* syn : synapses) {
            if (additionalPruning > 0.0f && rand < additionalPruning) {
                // Additional random pruning for density control
                syn->setWeight(0.0f);
                NLM_LOG_INFO("Pruning: Density control pruning (id: " + 
                           std::to_string(syn->getId().index()) + ")");
            }
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // Implement synapse removal from region
    // This would require access to the parent region to remove the synapse
    // For now, return true to indicate pruning was attempted
    
    NLM_LOG_INFO("Pruning: Synapse marked for removal (id: " + 
               std::to_string(synapse->getId().index()) + ")");
    
    return true;
}

} // namespace nlm
