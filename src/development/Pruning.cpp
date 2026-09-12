#include "Pruning.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <set>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    std::set<SynapseId> recentlyPruned;
    float activityThreshold;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f), activityThreshold(0.05f) {}
    
    ~Impl() {
        recentlyPruned.clear();
    }
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    if (threshold < 0.0f) throw std::invalid_argument("Pruning threshold cannot be negative");
    pImpl->threshold = threshold;
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    if (rate < 0.0f || rate > 1.0f) throw std::invalid_argument("Pruning rate must be between 0 and 1");
    pImpl->pruningRate = rate;
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    // Get all synapses in the brain
    std::vector<Synapse*> allSynapses;
    for (const auto& region : brain->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            allSynapses.push_back(syn.get());
        }
    }
    
    if (allSynapses.empty()) {
        return;
    }
    
    // Sort synapses by weight for targeted pruning
    std::sort(allSynapses.begin(), allSynapses.end(),
        [](Synapse* a, Synapse* b) { return a->getWeight() < b->getWeight(); });
    
    // Prune based on threshold
    size_t synapsesToPrune = 0;
    for (const auto& syn : allSynapses) {
        if (syn->getWeight() < pImpl->threshold) {
            synapsesToPrune++;
        }
    }
    
    // Determine actual number to prune based on rate
    if (!allSynapses.empty()) {
        size_t maxToPrune = static_cast<size_t>(allSynapses.size() * pImpl->pruningRate);
        synapsesToPrune = std::min(synapsesToPrune, maxToPrune);
    }
    
    // Prune the weakest synapses
    for (size_t i = 0; i < synapsesToPrune; ++i) {
        Synapse* synapseToRemove = allSynapses[i];
        
        // Check if synapse is recently used (not to be pruned immediately)
        bool recentlyUsed = false;
        if (!pImpl->recentlyPruned.empty()) {
            // In a real implementation, check synapse usage history
            // For now, assume all synapses are eligible if below threshold
            recentlyUsed = false;
        }
        
        if (!recentlyUsed && pruneSynapse(synapseToRemove)) {
            // Successfully removed synapse
            pImpl->recentlyPruned.insert(synapseToRemove->getId());
            
            // Update region's synapse list
            for (auto& region : const_cast<std::vector<std::unique_ptr<NeuralRegion>>&>(brain->getRegions())) {
                // Remove from region (this would need a removeSynapse method)
                // For now, we'll implement pruning at the region level separately
            }
        }
    }
    
    // Clean up old pruning records
    auto currentTime = std::chrono::steady_clock::now();
    for (auto it = pImpl->recentlyPruned.begin(); it != pImpl->recentlyPruned.end(); ) {
        // In a real implementation, check when synapse was actually removed
        // For now, remove all records after each pruning cycle
        it = pImpl->recentlyPruned.erase(it);
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) {
        throw std::invalid_argument("Synapse pointer cannot be null");
    }
    
    // Check if synapse weight is below threshold
    if (synapse->getWeight() >= pImpl->threshold) {
        return false;  // Don't prune synapses above threshold
    }
    
    // Additional check: only prune if synapse has low activity
    // This would check synaptic activity history if available
    
    // Get the presynaptic and postsynaptic neurons
    Neuron* preNeuron = synapse->getPreNeuron();
    Neuron* postNeuron = synapse->getPostNeuron();
    
    if (!preNeuron || !postNeuron) {
        throw std::invalid_argument("Synapse has invalid neuron pointers");
    }
    
    // Find the region containing this synapse
    NeuralRegion* containingRegion = nullptr;
    for (auto& region : const_cast<std::vector<std::unique_ptr<NeuralRegion>>&>(brain->getRegions())) {
        for (const auto& syn : region->getSynapses()) {
            if (syn.get() == synapse) {
                containingRegion = region.get();
                break;
            }
        }
        if (containingRegion) break;
    }
    
    if (containingRegion) {
        // Remove synapse from region
        // This would need a removeSynapse method in NeuralRegion
        // For now, we'll simulate removal
        
        // Create a list of synapses without the one to remove
        std::vector<std::unique_ptr<Synapse>> remainingSynapses;
        for (const auto& syn : containingRegion->getSynapses()) {
            if (syn.get() != synapse) {
                remainingSynapses.push_back(syn->clone());  // Assume clone method exists
            }
        }
        
        // Replace the synapse list
        containingRegion->clearSynapses();
        for (auto& syn : remainingSynapses) {
            containingRegion->addSynapse(std::move(syn));
        }
        
        return true;
    }
    
    }
    
    return false;  // Synapse not found in any region
}

} // namespace nlm
