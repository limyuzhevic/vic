#include "Pruning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Pruning::Impl {
    float pruningRate;
    float threshold;
    float connectionCost;
    std::vector<SynapseId> activeSynapses;
    
    Impl() : pruningRate(0.00001f), threshold(0.1f), connectionCost(0.5f) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

void Pruning::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Pruning system initialized");
}

void Pruning::evaluateConnections(class Brain* brain) {
    if (!brain) return;
    
    pImpl->activeSynapses.clear();
    
    // Scan all regions for synapses to evaluate
    for (auto* region : brain->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            float cost = calculateConnectionCost(syn);
            
            // Prune low-weight, high-cost connections
            if (std::abs(weight) < pImpl->threshold && cost > pImpl->connectionCost) {
                continue; // Keep but mark for potential pruning
            }
            
            // Mark for pruning if weight is very low and connection is costly
            if (std::abs(weight) < 0.01f && cost > pImpl->connectionCost) {
                pImpl->activeSynapses.push_back(syn->getId());
                NLM_LOG_DEBUG("Synapse marked for pruning: weight = " + 
                              std::to_string(weight) + ", cost = " + std::to_string(cost));
            }
        }
    }
    
    // Actually prune a percentage of marked synapses
    size_t toPrune = static_cast<size_t>(pImpl->activeSynapses.size() * pImpl->pruningRate);
    toPrune = std::min(toPrune, pImpl->activeSynapses.size());
    
    if (toPrune > 0) {
        NLM_LOG_INFO("Pruning " + std::to_string(toPrune) + " synapses");
        // In a real implementation, we would actually remove these synapses
        // For now, we'll just log the intention
        for (size_t i = 0; i < toPrune; ++i) {
            NLM_LOG_DEBUG("Pruning synapse: " + std::to_string(pImpl->activeSynapses[i].index()));
        }
    }
}

float Pruning::calculateConnectionCost(const Synapse* synapse) const {
    if (!synapse) return 1.0f;
    
    // Cost based on weight magnitude (lower weight = higher cost)
    float weight = std::abs(synapse->getWeight());
    return 1.0f / (weight + 0.01f);
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = std::max(0.0f, std::min(rate, 0.01f));
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = std::max(0.0f, threshold);
}

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setConnectionCost(float cost) {
    pImpl->connectionCost = std::max(0.0f, cost);
}

float Pruning::getConnectionCost() const {
    return pImpl->connectionCost;
}

void Pruning::reset() {
    pImpl->activeSynapses.clear();
}

} // namespace nlm
