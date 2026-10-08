#include "Pruning.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <limits>
#include <stdexcept>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    float targetDensity;
    SimulationStep lastPruningStep;
    bool enableWeakPruning;
    bool enableHighFrequencyPruning;
    
    Impl() 
        : threshold(0.01f), pruningRate(0.0001f), targetDensity(0.05f)
        , lastPruningStep(0), enableWeakPruning(true), enableHighFrequencyPruning(false) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = std::max(0.0f, threshold);
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = std::max(0.0f, rate);
}

float Pruning::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Pruning::setTargetDensity(float density) {
    pImpl->targetDensity = std::max(0.0f, density);
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    if (!brain || pImpl->pruningRate <= 0.0f) return;
    
    const SimulationStep currentStep = brain->getTotalSpikeCount() / 100;
    const SimulationStep stepsSincePruning = currentStep - pImpl->lastPruningStep;
    
    if (stepsSincePruning < 100) return; // Update every 100 steps
    
    pImpl->lastPruningStep = currentStep;
    
    // Calculate current synapse density
    size_t totalSynapses = 0;
    for (const auto& region : brain->getRegions()) {
        totalSynapses += region->getSynapseCount();
    }
    
    size_t targetSynapses = static_cast<size_t>(brain->getTotalNeuronCount() * pImpl->targetDensity);
    
    if (totalSynapses <= targetSynapses) return; // Target already met
    
    size_t excessSynapses = totalSynapses - targetSynapses;
    
    NLM_LOG_INFO(std::to_string(excessSynapses) + " excess synapses to prune");
    
    // Perform pruning based on rate
    size_t synapsesToPrune = static_cast<size_t>(pImpl->pruningRate * static_cast<float>(excessSynapses));
    
    if (synapsesToPrune == 0) return;
    
    // Simple pruning strategy: remove weakest synapses first
    size_t actuallyPruned = 0;
    
    // Collect all synapses with weights below threshold
    std::vector<Synapse*> weakSynapses;
    for (const auto& region : brain->getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            float normalizedWeight = std::abs(syn->getWeight()) / 1.0f;
            if (normalizedWeight < pImpl->threshold) {
                weakSynapses.push_back(const_cast<Synapse*>(syn));
            }
        }
    }
    
    if (weakSynapses.empty()) {
        NLM_LOG_INFO("No weak synapses found for pruning");
        return;
    }
    
    // Randomly select synapses to prune
    std::shuffle(weakSynapses.begin(), weakSynapses.end(), rng);
    size_t toActuallyPrune = std::min(synapsesToPrune, weakSynapses.size());
    
    NLM_LOG_INFO(std::to_string(toActuallyPrune) + " weak synapses selected for pruning");
    
    for (size_t i = 0; i < toActuallyPrune; ++i) {
        // Note: In a real implementation, we would need proper synapse removal
        // from the region's synapse collection. For now, we just log the action.
        NLM_LOG_INFO("Pruning synapse with weak weight: " + std::to_string(weakSynapses[i]->getWeight()));
        actuallyPruned++;
    }
    
    if (actuallyPruned > 0) {
        NLM_LOG_INFO(std::to_string(actuallyPruned) + " synapses pruned");
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) {
        NLM_LOG_ERROR("Cannot prune null synapse");
        return false;
    }
    
    NLM_LOG_INFO(std::to_string(synapse->getId().index()) + 
                " -> " + std::to_string(synapse->getDestinationNeuron().index()) +
                " (weight: " + std::to_string(synapse->getWeight()) + ") pruned");
    
    // Note: Real implementation would need access to the containing region
    // and proper removal from the region's synapse collection.
    // This is a placeholder implementation for demonstration purposes.
    
    return true;
}

} // namespace nlm
