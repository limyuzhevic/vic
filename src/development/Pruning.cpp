#include "Pruning.hpp"
#include "../../brain/Brain.hpp"
#include "../../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    float minimumConnectionAge;
    float strengthPenalty;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f),
             minimumConnectionAge(10.0), strengthPenalty(0.5f) {}
};

Pruning::Pruning() : pImpl(std::make_unique<Impl>()) {}

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
    pImpl->pruningRate = std::max(0.0f, std::min(rate, 1.0f));
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    if (!brain || pImpl->pruningRate <= 0.0f) {
        return;
    }
    
    auto regions = brain->getRegions();
    if (regions.empty()) {
        return;
    }
    
    // Calculate synapse statistics for intelligent pruning
    std::vector<std::pair<NeuralRegion*, std::vector<Synapse*>>> regionSynapses;
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto synapses = region->getSynapses();
        regionSynapses.emplace_back(region.get(), synapses);
    }
    
    if (regionSynapses.empty()) {
        return;
    }
    
    // Sort synapses by strength (weakest first) for efficient pruning
    std::vector<std::pair<Synapse*, NeuralRegion*>> allSynapses;
    
    for (auto& pair : regionSynapses) {
        NeuralRegion* region = pair.first;
        auto& synapses = pair.second;
        
        for (auto* synapse : synapses) {
            if (!synapse) continue;
            
            allSynapses.emplace_back(synapse, region);
        }
    }
    
    if (allSynapses.empty()) {
        return;
    }
    
    // Sort by synaptic strength (weakest first)
    std::sort(allSynapses.begin(), allSynapses.end(),
              [](const std::pair<Synapse*, NeuralRegion*>& a, const std::pair<Synapse*, NeuralRegion*>& b) {
                  return a.first->getWeight() < b.first->getWeight();
              });
    
    // Calculate how many synapses to prune
    size_t totalSynapses = allSynapses.size();
    size_t synapsesToPrune = static_cast<size_t>(totalSynapses * pImpl->pruningRate);
    
    // Prune weakest synapses that meet the criteria
    size_t prunedCount = 0;
    
    for (size_t i = 0; i < allSynapses.size() && prunedCount < synapsesToPrune; ++i) {
        Synapse* synapse = allSynapses[i].first;
        NeuralRegion* region = allSynapses[i].second;
        
        if (!synapse || !region) continue;
        
        // Check pruning criteria
        bool shouldPrune = false;
        
        // Criterion 1: Weak synapse strength
        if (synapse->getWeight() < pImpl->threshold) {
            shouldPrune = true;
        }
        
        // Criterion 2: Low efficacy
        if (synapse->getEfficacy() < 0.1f) {
            shouldPrune = true;
        }
        
        // Criterion 3: Check synapse age (if available)
        // In a real implementation, you would track synapse creation time
        // For now, we'll assume all synapses are eligible for pruning
        
        if (shouldPrune) {
            // Attempt to remove the synapse
            bool removed = region->removeSynapse(synapse->getPreNeuronId(), synapse->getPostNeuronId());
            
            if (removed) {
                prunedCount++;
            }
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) {
        return false;
    }
    
    // Additional pruning logic for individual synapses
    // This could include more sophisticated criteria like:
    // - Synaptic history
    // - Recent activity patterns
    // - Neuromodulatory influences
    
    // For now, return false as we handle pruning in the update loop
    return false;
}

} // namespace nlm
