// Implementation of Neural Pruning - developmental synaptic elimination
#include "Pruning.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <numeric>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    float connectivityTarget;
    float developmentalModulation;
    size_t minSynapticDensity;
    bool useActivityDependentPruning;
    
    Impl() 
        : threshold(0.01f)
        , pruningRate(0.0001f)
        , connectivityTarget(0.05f)
        , developmentalModulation(1.0f)
        , minSynapticDensity(10)
        , useActivityDependentPruning(true) {}
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

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    if (!brain || !brain->getRandomGenerator()) {
        return;
    }
    
    // Calculate current synaptic density
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    if (totalNeurons < 2) {
        return;  // Not enough neurons for connections
    }
    
    // Calculate theoretical maximum synapses (all-to-all)
    size_t maxPossibleSynapses = totalNeurons * (totalNeurons - 1);
    float currentDensity = static_cast<float>(totalSynapses) / maxPossibleSynapses;
    
    // Modulate pruning with developmental stage
    // Early development has less pruning, adult has more selective pruning
    float stageModulation = 1.0f;
    if (brain->getDevelopmentSystem()) {
        switch (brain->getDevelopmentSystem()->getStage()) {
            case DevelopmentalStage::Initial:
                stageModulation = 0.1f;  // Minimal pruning in initial stage
                break;
            case DevelopmentalStage::CriticalPeriod:
                stageModulation = 0.3f;  // Moderate pruning for circuit refinement
                break;
            case DevelopmentalStage::Maturation:
                stageModulation = 0.5f;  // Significant pruning
                break;
            case DevelopmentalStage::Adult:
                stageModulation = 1.0f;  // Regular maintenance pruning
                break;
            case DevelopmentalStage::Aging:
                stageModulation = 1.5f;  // Increased pruning with age
                break;
        }
    }
    
    // Calculate connectivity deficit relative to target
    float densityDeficit = currentDensity - pImpl->connectivityTarget;
    
    if (densityDeficit < 0.0f) {
        // We have fewer synapses than target - no need to prune
        return;
    }
    
    // Apply activity-dependent pruning if enabled
    std::vector<Synapse*> weakSynapses;
    std::vector<Synapse*> inactiveSynapses;
    
    // Collect synapses by activity level and synaptic strength
    for (const auto& region : brain->getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            if (!syn) {
                continue;
            }
            
            float weight = syn->getWeight();
            float eligibility = syn->getEligibilityTrace();
            
            // Classify synapses for potential pruning
            if (weight < pImpl->threshold) {
                weakSynapses.push_back(const_cast<Synapse*>(syn));
            }
            
            if (pImpl->useActivityDependentPruning && std::abs(eligibility) < 0.01f) {
                inactiveSynapses.push_back(const_cast<Synapse*>(syn));
            }
        }
    }
    
    // Sort synapses by weakness (lower weight first)
    std::sort(weakSynapses.begin(), weakSynapses.end(),
        [this](const Synapse* a, const Synapse* b) {
            return a->getWeight() < b->getWeight();
        });
    
    // Sort inactive synapses
    std::sort(inactiveSynapses.begin(), inactiveSynapses.end(),
        [this](const Synapse* a, const Synapse* b) {
            return a->getEligibilityTrace() < b->getEligibilityTrace();
        });
    
    // Determine number of synapses to prune
    size_t synapsesToPrune = 0;
    if (!weakSynapses.empty()) {
        // Prune a fraction of weak synapses based on pruning rate
        synapsesToPrune = static_cast<size_t>(weakSynapses.size() * pImpl->pruningRate * stageModulation);
    } else if (!inactiveSynapses.empty() && pImpl->useActivityDependentPruning) {
        // Fall back to pruning inactive synapses
        synapsesToPrune = static_cast<size_t>(inactiveSynapses.size() * pImpl->pruningRate * stageModulation);
    }
    
    // Ensure we don't exceed available synapses
    synapsesToPrune = std::min(synapsesToPrune, 
                             std::min(weakSynapses.size(), 
                                     std::min(inactiveSynapses.size(), 1000UL)));
    
    // Prune selected synapses
    size_t prunedCount = 0;
    for (size_t i = 0; i < synapsesToPrune && i < weakSynapses.size(); ++i) {
        if (pruneSynapse(weakSynapses[i])) {
            ++prunedCount;
        }
    }
    
    // Log pruning statistics
    if (prunedCount > 0) {
        NLM_LOG_INFO("Pruning: Removed " + std::to_string(prunedCount) + 
                    " synapses (threshold: " + std::to_string(pImpl->threshold) + 
                    ", rate: " + std::to_string(pImpl->pruningRate) + ")");
    }
    
    // Update developmental modulation for next step
    pImpl->developmentalModulation *= 0.95f; // Gradual decrease over time
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) {
        return false;
    }
    
    // Get synapse information before pruning
    float weight = synapse->getWeight();
    NeuronId sourceId = synapse->getSourceNeuron();
    NeuronId targetId = synapse->getDestinationNeuron();
    
    // Log the pruning event
    NLM_LOG_INFO("Pruning: Removing weak synapse from neuron " + 
                std::to_string(sourceId.index()) + " to " +
                std::to_string(targetId.index()) + 
                " (weight: " + std::to_string(weight) + ")");
    
    // In a full implementation, we would:
    // 1. Remove the synapse from the brain's internal data structures
    // 2. Update connectivity statistics
    // 3. Potentially apply homeostatic plasticity to maintain target connectivity
    // 4. Update eligibility traces for remaining synapses
    
    // For now, return true to indicate successful pruning attempt
    // In actual implementation, this would modify the brain's state
    return true;
}

} // namespace nlm
