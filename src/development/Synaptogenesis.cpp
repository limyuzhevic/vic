#include "Synaptogenesis.hpp"
#include "../../brain/Brain.hpp"
#include "../../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float maturityThreshold;
    float maxNewSynapses;
    float minimumConnectionDistance;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f),
             maturityThreshold(0.5f), maxNewSynapses(50.0f),
             minimumConnectionDistance(5.0f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(std::make_unique<Impl>()) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = std::max(0.0f, std::min(rate, 1.0f));
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = std::max(0.0f, std::min(density, 1.0f));
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain || pImpl->formationRate <= 0.0f) {
        return;
    }
    
    auto regions = brain->getRegions();
    if (regions.empty()) {
        return;
    }
    
    // Calculate current synapse density
    size_t totalSynapses = 0;
    size_t totalPotentialConnections = 0;
    
    for (const auto& region : regions) {
        const auto& neurons = region->getAllNeurons();
        size_t regionSynapses = region->getSynapseCount();
        totalSynapses += regionSynapses;
        
        // Calculate potential connections (all-to-all within region)
        size_t potential = neurons.size() * (neurons.size() - 1) / 2;
        totalPotentialConnections += potential;
    }
    
    if (totalPotentialConnections == 0) {
        return;
    }
    
    float currentDensity = static_cast<float>(totalSynapses) / totalPotentialConnections;
    float densityTarget = pImpl->targetDensity;
    
    // If we're already at or above target density, don't create new synapses
    if (currentDensity >= densityTarget) {
        return;
    }
    
    // Calculate how many new synapses we should create
    float deficit = densityTarget - currentDensity;
    size_t potentialNewSynapses = static_cast<size_t>(deficit * totalPotentialConnections * pImpl->formationRate);
    size_t newSynapsesToCreate = std::min(potentialNewSynapses, static_cast<size_t>(pImpl->maxNewSynapses));
    
    // Create new synapses
    for (size_t created = 0; created < newSynapsesToCreate; ++created) {
        // Select a region for synaptogenesis
        size_t regionIndex = rng.uniformInt(0, regions.size() - 1);
        auto* region = regions[regionIndex].get();
        
        if (!region) continue;
        
        const auto& neurons = region->getAllNeurons();
        if (neurons.size() < 2) continue;
        
        // Find potential synapse partners (neurons within minimum distance)
        std::vector<std::pair<size_t, size_t>> potentialPartners;
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = i + 1; j < neurons.size(); ++j) {
                // Check if neurons are within reasonable distance
                // This is a simplified distance calculation
                NeuronId neuronId1 = neurons[i]->getId();
                NeuronId neuronId2 = neurons[j]->getId();
                
                // For now, use a simple proximity check
                // In a real implementation, you would use actual spatial positions
                potentialPartners.emplace_back(i, j);
            }
        }
        
        if (potentialPartners.empty()) {
            continue;
        }
        
        // Select a random partner from the potential list
        size_t partnerIndex = rng.uniformInt(0, potentialPartners.size() - 1);
        size_t preIdx = potentialPartners[partnerIndex].first;
        size_t postIdx = potentialPartners[partnerIndex].second;
        
        Neuron* preNeuron = neurons[preIdx].get();
        Neuron* postNeuron = neurons[postIdx].get();
        
        if (!preNeuron || !postNeuron) continue;
        
        // Create a new synapse with initial weight
        float initialWeight = rng.uniformFloat(0.1f, 0.5f); // Excitatory synapses
        
        // Check if synapse already exists
        if (region->hasSynapse(preNeuron->getId(), postNeuron->getId())) {
            continue;
        }
        
        // Create new synapse
        region->createSynapse(preNeuron->getId(), postNeuron->getId(), initialWeight);
        
        // Mark both neurons as mature if they aren't already
        if (preNeuron->getMaturationProgress() < pImpl->maturityThreshold) {
            preNeuron->setMaturationProgress(pImpl->maturityThreshold);
        }
        
        if (postNeuron->getMaturationProgress() < pImpl->maturityThreshold) {
            postNeuron->setMaturationProgress(pImpl->maturityThreshold);
        }
    }
}

} // namespace nlm
